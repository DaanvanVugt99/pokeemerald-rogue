#!/usr/bin/env python3
"""Reconcile approved rebalance decisions with preprocessed species/profile data.

The pinned starting commit supplies preservation defaults, not a second tracked
learnset snapshot. Only explicitly approved overrides belong in the manifest.
Partial checkpoints are useful, but --require-complete must pass before signoff.
"""
from __future__ import annotations

import argparse
import hashlib
import io
import json
import re
import shutil
import subprocess
import tarfile
import tempfile
from pathlib import Path

from generate_species_change_report import (
    REPO_ROOT, evaluate_expression, extract_species_records, find_matching_brace,
    load_baseline, preprocess_species_table, species_key_by_id,
)

MANIFEST = REPO_ROOT / "tools/data/rebalance_manifest.json"
LEDGER = REPO_ROOT / "docs/rebalance_review.md"
PROFILE = "src/data/rogue_pokemon_profiles.h"


def digest(text):
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def unique_json_keys(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise ValueError("Duplicate manifest key: " + key)
        result[key] = value
    return result


def ledger_rows(text):
    """Identify every decision table row, including shared-design audit tables."""
    rows = {}
    section = ""
    in_table = False
    for line in text.splitlines():
        if line.startswith("## "):
            section = line[3:]
        if not line.startswith("|"):
            in_table = False
            continue
        if not in_table:
            in_table = True  # first row is the table header
            continue
        cells = [cell.strip() for cell in line.strip("|").split("|")]
        if all(re.fullmatch(r"[:\- ]+", cell) for cell in cells):
            continue
        key = section + " | " + cells[0]
        if key in rows:
            raise ValueError("Duplicate ledger decision: " + key)
        rows[key] = digest(" | ".join(cells))
    return rows


def audit_digest(text):
    # Prose audits supersede table rows. Require deliberate re-review on drift;
    # implementation-status edits in the first section do not change decisions.
    start = text.index("## Move-distribution audit")
    end = text.index("## Remaining Upstream Revised Pokemon")
    shared = text[text.index("## Shared design rules audit"):]
    return digest(text[start:end].strip() + "\n\n" + shared.strip())


def parse_profiles(text):
    levels, tutors, profiles = {}, {}, {}
    for name, body in re.findall(
        r"static struct LevelUpMove const (\w+)\[\]\s*=\s*\{(.*?)\n\};", text, re.S
    ):
        levels[name] = sorted((move, int(level)) for move, level in re.findall(
            r"\.move\s*=\s*(MOVE_\w+)\s*,\s*\.level\s*=\s*(\d+)", body
        ) if move != "MOVE_NONE")
    for name, body in re.findall(
        r"static u16 const (sTutorMoves_\w+)\[\]\s*=\s*\{(.*?)\n\};", text, re.S
    ):
        tutors[name] = set(re.findall(r"\bMOVE_\w+\b", body)) - {"MOVE_NONE"}
    for species, body in re.findall(r"\[(SPECIES_\w+)\]\s*=\s*\{([^{}]*)\}", text):
        level = re.search(r"\.levelUpMoves\s*=\s*(\w+)", body)
        tutor = re.search(r"\.tutorMoves\s*=\s*(\w+)", body)
        if level and tutor:
            profiles[species] = (levels[level[1]], tutors[tutor[1]])
    if not profiles:
        raise ValueError("No generated profiles found")
    return profiles


def normalize_profile_aliases(profiles, records, species_header):
    aliases = dict(re.findall(r"^#define\s+(SPECIES_\w+)\s+(SPECIES_\w+)\s*$", species_header, re.M))
    result = {}
    for species, profile in profiles.items():
        seen = set()
        while species not in records and species in aliases and species not in seen:
            seen.add(species)
            species = aliases[species]
        if species in result and result[species] != profile:
            raise ValueError("Conflicting profile aliases for " + species)
        result[species] = profile
    return result


def git_show(ref, path):
    return subprocess.check_output(["git", "show", f"{ref}:{path}"], cwd=REPO_ROOT).decode("utf-8")


def evolution_edges(preprocessed, species_keys):
    """Read actual compiled evolution targets, including Rogue regional branches."""
    declaration = re.search(r"const struct SpeciesInfo gSpeciesInfo\[\]\s*=\s*\{", preprocessed)
    if declaration is None:
        raise ValueError("Missing preprocessed gSpeciesInfo")
    opening = preprocessed.index("{", declaration.start())
    table = preprocessed[opening + 1:find_matching_brace(preprocessed, opening)]
    edges = set()
    cursor = 0
    pattern = re.compile(r"\[\s*([^\]]+)\s*\]\s*=\s*\{")
    while match := pattern.search(table, cursor):
        opening = table.index("{", match.start())
        closing = find_matching_brace(table, opening)
        entry = table[opening + 1:closing]
        cursor = closing + 1
        source = species_keys.get(evaluate_expression(match[1]))
        evolution = re.search(r"\.evolutions\s*=\s*\(const struct Evolution\[\]\)\s*\{", entry)
        if not source or not evolution:
            continue
        start = entry.index("{", evolution.start())
        body = entry[start + 1:find_matching_brace(entry, start)]
        for fields in re.findall(r"\{([^{}]+)\}", body):
            fields = fields.split(",")
            if len(fields) < 3:  # EVO_END sentinel
                continue
            target = species_keys.get(evaluate_expression(fields[2]))
            if target and target != source:
                edges.add((source, target))
    return edges


def move_access(profile):
    return {move for move, _ in profile[0]} | profile[1]


def accumulation_gaps(edges, profiles, original_profiles, covered):
    """Only newly acquired access is a regression; keep historical gaps separate."""
    new_gaps, historical_gaps = [], []
    adjacency = {}
    for source, target in edges:
        adjacency.setdefault(source, set()).add(target)
    for source in sorted(covered & profiles.keys() & original_profiles.keys()):
        access = move_access(profiles[source])
        additions = access - move_access(original_profiles[source])
        pending = list(adjacency.get(source, ()))
        visited = {source}
        while pending:
            target = pending.pop()
            if target in visited:
                continue
            visited.add(target)
            pending.extend(adjacency.get(target, ()))
            if target not in profiles:
                new_gaps.append({"source": source, "target": target,
                                 "missing_profile": True, "covered": target in covered})
                continue
            missing = access - move_access(profiles[target])
            for move in sorted(missing):
                gap = {"source": source, "target": target, "move": move,
                       "covered": target in covered}
                (new_gaps if move in additions else historical_gaps).append(gap)
    key = lambda gap: (gap["source"], gap["target"], gap.get("move", ""))
    return sorted(new_gaps, key=key), sorted(historical_gaps, key=key)


def check_learnable_move_ids(before, after):
    # Learnable IDs are explicit decimal definitions. Z/Max IDs are derived
    # battle-only ranges after MOVES_COUNT and are not serialized move slots.
    pattern = r"^#define\s+(MOVE_\w+)\s+(\d+)\s*$"
    old = {name: int(value) for name, value in re.findall(pattern, before, re.M)}
    new = {name: int(value) for name, value in re.findall(pattern, after, re.M)}
    errors = [f"Serialized move ID changed: {name} ({value} -> {new.get(name)})"
              for name, value in old.items() if new.get(name) != value]
    for name in new.keys() - old.keys():
        if new[name] in old.values():
            errors.append(f"New move {name} reuses an existing ID: {new[name]}")
    return errors


def starting_species(ref):
    """Extract pinned source definitions; never checkout/overwrite the worktree."""
    paths = ["include", "src/data/pokemon", "src/data/graphics/pokemon.h",
             "src/data/pokemon_graphics", "tools/Pokabbie/PokemonQueryBaker/PokemonQueryBaker/Inc"]
    archive = subprocess.check_output(["git", "archive", ref, *paths], cwd=REPO_ROOT)
    with tempfile.TemporaryDirectory(prefix="rebalance-baseline-") as temporary:
        root = Path(temporary)
        with tarfile.open(fileobj=io.BytesIO(archive)) as source:
            # Local pinned Git data only; reject links/traversal regardless.
            for member in source.getmembers():
                target = (root / member.name).resolve()
                if not target.is_relative_to(root.resolve()) or not (member.isfile() or member.isdir()):
                    raise ValueError("Unsafe baseline archive member: " + member.name)
            source.extractall(root)
        generated = REPO_ROOT / "include/constants/generated"
        if generated.exists():
            shutil.copytree(generated, root / "include/constants/generated", dirs_exist_ok=True)
        return extract_species_records(root)


def expected_profile(original, rule):
    levels, tutors = original
    replace = rule.get("level_moves", {})
    remove_levels = set(rule.get("remove_level_moves", [])) | set(replace)
    levels = [(move, level) for move, level in levels if move not in remove_levels]
    levels += [(move, level) for move, level in replace.items()]
    tutors = (tutors | set(rule.get("add_tutors", []))) - set(rule.get("remove_tutors", []))
    return sorted(levels), tutors


def check_species(species, rule, current, original, canonical, profiles, original_profiles):
    errors = []
    if species not in current or species not in original:
        return [species + ": missing runtime or starting species"]
    before, actual = original[species], current[species]
    stats = rule.get("stats", before.stats)
    if "stat_delta" in rule:
        if "stats" in rule or len(rule["stat_delta"]) != 6 or species not in canonical:
            return [species + ": invalid canonical stat delta"]
        stats = [base + delta for base, delta in zip(canonical[species].stats, rule["stat_delta"])]
    for field, expected in (("stats", stats), ("types", rule.get("types", before.types)),
                            ("abilities", rule.get("abilities", before.abilities))):
        if tuple(expected) != getattr(actual, field):
            errors.append(f"{species}: {field}: expected {list(expected)}, got {list(getattr(actual, field))}")
    if species not in profiles or species not in original_profiles:
        errors.append(species + ": missing current or starting learnset")
    else:
        expected = expected_profile(original_profiles[species], rule)
        for label, want, got in zip(("level moves", "tutors"), expected, profiles[species]):
            if want != got:
                errors.append(f"{species}: {label}: missing {sorted(set(want)-set(got))}; unexpected {sorted(set(got)-set(want))}")
    return errors


def check_invariants(invariants, current, canonical):
    """Check approved relationships independently of individual species targets.

    Groups are explicit: regional/Mega exceptions must not be inferred away by
    a blanket rule that all forms share types, Abilities or absolute stats.
    """
    errors, seen = [], set()
    value_fields = {"bst": "value", "distinct_abilities": "value",
                    "second_ability": "value", "matching_stat_deltas": None}
    for rule in invariants:
        name, kind = rule.get("id"), rule.get("kind")
        if not isinstance(name, str) or not name or name in seen:
            errors.append("Missing or duplicate invariant ID: " + str(name))
            continue
        seen.add(name)
        prefix = "Invariant " + name + ": "
        if kind not in value_fields:
            errors.append(prefix + "unknown kind " + str(kind))
            continue
        allowed = {"id", "kind", "species"} | ({"value"} if value_fields[kind] else set())
        if set(rule) != allowed:
            errors.append(prefix + "unexpected or missing fields")
            continue
        species = rule["species"]
        if (not isinstance(species, list) or not species
                or not all(isinstance(key, str) for key in species)
                or len(set(species)) != len(species)):
            errors.append(prefix + "species must be a nonempty list of distinct keys")
            continue
        missing = set(species) - current.keys()
        if missing:
            errors.append(prefix + "missing runtime species " + str(sorted(missing)))
            continue
        if kind == "matching_stat_deltas":
            missing = set(species) - canonical.keys()
            if missing or len(species) < 2:
                errors.append(prefix + "requires at least two canonical species; missing " + str(sorted(missing)))
                continue
            deltas = {key: tuple(a - b for a, b in zip(current[key].stats, canonical[key].stats))
                      for key in species}
            if len(set(deltas.values())) != 1:
                errors.append(prefix + "canonical stat deltas differ: " + str(deltas))
            continue
        value = rule["value"]
        if kind == "second_ability":
            if not isinstance(value, str) or not re.fullmatch(r"ABILITY_[A-Z0-9_]+", value) or value == "ABILITY_NONE":
                errors.append(prefix + "invalid second Ability")
                continue
        elif type(value) is not int or value <= 0 or (kind == "distinct_abilities" and value > 3):
            errors.append(prefix + "invalid positive count")
            continue
        for key in species:
            record = current[key]
            if kind == "bst":
                actual = sum(record.stats)
            elif kind == "distinct_abilities":
                actual = len(set(record.abilities) - {"ABILITY_NONE"})
            else:
                actual = record.abilities[1]
                if actual in (record.abilities[0], record.abilities[2]):
                    errors.append(prefix + key + " second Ability is not distinct")
            if actual != value:
                errors.append(prefix + f"{key}: expected {value}, got {actual}")
    return errors


def run_check(manifest, ledger, current, original, canonical, profiles, original_profiles):
    rows = ledger_rows(ledger)
    errors, covered, checked = [], set(), set()
    if manifest["audit_sha256"] != audit_digest(ledger):
        errors.append("Superseding audit decisions changed; re-review the normalized manifest")
    allowed = {"stats", "stat_delta", "types", "abilities", "level_moves", "remove_level_moves", "add_tutors", "remove_tutors"}
    for batch in manifest["batches"]:
        for key, fingerprint in batch["decisions"].items():
            if key in covered:
                errors.append("Decision covered twice: " + key)
            covered.add(key)
            if rows.get(key) != fingerprint:
                errors.append("Missing or changed ledger decision: " + key)
        if not batch["species"]:
            errors.append(batch["id"] + ": no runtime species targets")
        for species, rule in batch["species"].items():
            if species in checked:
                errors.append("Species covered twice: " + species)
            checked.add(species)
            if set(rule) - allowed:
                errors.append(species + ": unsupported rule fields: " + str(sorted(set(rule) - allowed)))
            errors.extend(check_species(species, rule, current, original, canonical, profiles, original_profiles))
    invariants = manifest.get("invariants", [])
    errors.extend(check_invariants(invariants, current, canonical))
    return {"covered_decisions": len(covered & rows.keys()), "total_decisions": len(rows),
            "checked_invariants": len(invariants),
            "checked_species": len(checked), "pending_decisions": sorted(rows.keys() - covered),
            "errors": errors, "complete": not errors and covered == rows.keys()}


def uncovered_changes(current, original, profiles, original_profiles, covered):
    """Catch inherited/shared-macro changes outside explicit manifest targets."""
    gaps = []
    keys = current.keys() | original.keys() | profiles.keys() | original_profiles.keys()
    for species in sorted(keys - covered):
        fields = []
        if species not in current or species not in original:
            if current.get(species) != original.get(species):
                fields.append("species presence")
        else:
            fields.extend(field for field in ("stats", "types", "abilities")
                          if getattr(current[species], field) != getattr(original[species], field))
        if profiles.get(species) != original_profiles.get(species):
            fields.append("learnset")
        if fields:
            gaps.append({"species": species, "fields": fields})
    return gaps


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--require-complete", action="store_true")
    args = parser.parse_args()
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"), object_pairs_hook=unique_json_keys)
    if manifest["schema_version"] != 1 or manifest["stat_order"] != ["HP", "Atk", "Def", "SpA", "SpD", "Spe"]:
        raise ValueError("Unsupported manifest schema or stat order")
    ref = manifest["starting_commit"]
    if not re.fullmatch(r"[0-9a-f]{40}", ref):
        raise ValueError("Starting commit must be an immutable full Git hash")
    preprocessed = preprocess_species_table(REPO_ROOT)
    current, original = extract_species_records(REPO_ROOT, preprocessed), starting_species(ref)
    species_header = "include/constants/species.h"
    profiles = normalize_profile_aliases(
        parse_profiles((REPO_ROOT / PROFILE).read_text(encoding="utf-8")), current,
        (REPO_ROOT / species_header).read_text(encoding="utf-8"))
    original_profiles = normalize_profile_aliases(
        parse_profiles(git_show(ref, PROFILE)), original, git_show(ref, species_header))
    report = run_check(manifest, LEDGER.read_text(encoding="utf-8"),
                       current, original, load_baseline()[1], profiles, original_profiles)
    covered = {species for batch in manifest["batches"] for species in batch["species"]}
    report["uncovered_changes"] = uncovered_changes(current, original, profiles, original_profiles, covered)
    for change in report["uncovered_changes"]:
        report["errors"].append("Uncovered runtime change: " + str(change))
    report["new_evolution_move_gaps"], report["historical_evolution_move_gaps"] = accumulation_gaps(
        evolution_edges(preprocessed, species_key_by_id(REPO_ROOT)), profiles, original_profiles, covered)
    for gap in report["new_evolution_move_gaps"]:
        if gap["covered"]:
            report["errors"].append("New evolution move gap: " + str(gap))
    move_header = "include/constants/moves.h"
    report["errors"].extend(check_learnable_move_ids(
        git_show(ref, move_header), (REPO_ROOT / move_header).read_text(encoding="utf-8")))
    report["complete"] = report["complete"] and not report["errors"] and not report["new_evolution_move_gaps"]
    output = REPO_ROOT / "build/rebalance/verification.json"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(f"Covered {report['covered_decisions']}/{report['total_decisions']} ledger decisions; checked {report['checked_species']} species")
    for error in report["errors"]:
        print("FAIL: " + error)
    print("COMPLETE" if report["complete"] else "INCOMPLETE: pending decisions or failed checks remain")
    print("Report: " + str(output))
    return int(bool(report["errors"]) or (args.require_complete and not report["complete"]))


if __name__ == "__main__":
    raise SystemExit(main())
