#!/usr/bin/env python3
"""Generate the canonical-vs-Divergence species data report.

The extractor preprocesses the same SpeciesInfo table used by QueryBaker so
shared macros, configured generation updates, and form data are resolved before
comparison. The canonical baseline is committed as JSON to keep routine report
generation deterministic and independent of remote branches or web services.
"""

from __future__ import annotations

import argparse
import csv
import io
import os
import re
import shutil
import subprocess
import sys
import tarfile
import tempfile
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable


SCRIPT_PATH = Path(__file__).resolve()
REPO_ROOT = SCRIPT_PATH.parent.parent
BASELINE_PATH = REPO_ROOT / "tools" / "data" / "canonical_species_data.csv"
MARKDOWN_PATH = REPO_ROOT / "docs" / "generated" / "species_changes.md"
CSV_PATH = REPO_ROOT / "docs" / "generated" / "species_changes.csv"
DEFAULT_BASELINE_REF = "91a4feca60251da39ac320bac91d7c59a8a7dd9a"

STAT_FIELDS = (
    "baseHP",
    "baseAttack",
    "baseDefense",
    "baseSpAttack",
    "baseSpDefense",
    "baseSpeed",
)
STAT_LABELS = ("HP", "Atk", "Def", "SpA", "SpD", "Spe")
SPECIES_ENTRY_RE = re.compile(r"\[\s*(SPECIES_[A-Z0-9_]+)\s*\]\s*=")
SIMPLE_TERNARY_RE = re.compile(
    r"\(([^()?]+)\?([^?:()]+):([^?()]+)\)"
)
SAFE_EXPRESSION_RE = re.compile(r"^[0-9\s()+\-*/%<>=!&|.]+$")


@dataclass(frozen=True)
class SpeciesRecord:
    key: str
    name: str
    stats: tuple[int, int, int, int, int, int]
    types: tuple[str, str]
    abilities: tuple[str, str, str]

@dataclass(frozen=True)
class SpeciesChange:
    baseline: SpeciesRecord
    current: SpeciesRecord

    @property
    def stats_changed(self) -> bool:
        return self.baseline.stats != self.current.stats

    @property
    def types_changed(self) -> bool:
        return self.baseline.types != self.current.types

    @property
    def abilities_changed(self) -> bool:
        return self.baseline.abilities != self.current.abilities

    def signature(self, category: str) -> tuple[object, ...]:
        if category == "stats":
            return self.baseline.stats, self.current.stats
        if category == "types":
            return self.baseline.types, self.current.types
        if category == "abilities":
            return self.baseline.abilities, self.current.abilities
        raise ValueError(f"Unknown category: {category}")


def run(
    args: list[str],
    *,
    cwd: Path,
    input_text: str | None = None,
    binary: bool = False,
) -> str | bytes:
    result = subprocess.run(
        args,
        cwd=cwd,
        input=input_text if not binary else None,
        text=not binary,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if result.returncode != 0:
        stderr = (
            result.stderr.decode(errors="replace")
            if binary
            else result.stderr
        )
        raise RuntimeError(
            f"Command failed ({' '.join(args)}):\n{stderr.strip()}"
        )
    return result.stdout


def find_preprocessor() -> str:
    override = os.environ.get("SPECIES_REPORT_CPP")
    if override:
        path = shutil.which(override)
        if path:
            return path
        raise RuntimeError(
            f"SPECIES_REPORT_CPP does not resolve to an executable: {override}"
        )

    for candidate in ("clang", "gcc", "cc", "arm-none-eabi-gcc"):
        path = shutil.which(candidate)
        if path:
            return path
    raise RuntimeError(
        "No C preprocessor found; install clang, gcc, or the devkitARM toolchain"
    )


def evaluate_expression(expression: str) -> int:
    expression = expression.strip()

    while "?" in expression:
        match = SIMPLE_TERNARY_RE.search(expression)
        if match:
            condition, when_true, when_false = match.groups()
            replacement = (
                when_true
                if evaluate_expression(condition)
                else when_false
            )
            expression = (
                expression[: match.start()]
                + f"({replacement.strip()})"
                + expression[match.end() :]
            )
            continue

        match = re.fullmatch(r"([^?]+)\?([^:]+):(.+)", expression)
        if not match:
            raise ValueError(f"Unsupported ternary expression: {expression}")
        condition, when_true, when_false = match.groups()
        expression = (
            when_true if evaluate_expression(condition) else when_false
        ).strip()

    if not SAFE_EXPRESSION_RE.fullmatch(expression):
        raise ValueError(f"Unsafe or unsupported expression: {expression}")

    translated = expression.replace("&&", " and ").replace("||", " or ")
    translated = re.sub(r"!(?!=)", " not ", translated)
    return int(eval(translated, {"__builtins__": {}}, {}))


def preprocess(root: Path, source: str, include_dirs: Iterable[Path]) -> str:
    compiler = find_preprocessor()
    args = [
        compiler,
        "-E",
        "-P",
        "-x",
        "c",
        "-DROGUE_BAKING=1",
        "-DROGUE_EXPANSION=1",
    ]
    for include_dir in include_dirs:
        args.append(f"-I{include_dir}")
    args.append("-")
    output = run(args, cwd=root, input_text=source)
    assert isinstance(output, str)
    return output


def find_query_baker_include(root: Path) -> Path:
    candidates = sorted(root.glob("tools/**/BakeHelpers.h"))
    for candidate in candidates:
        contents = candidate.read_text(encoding="utf-8", errors="ignore")
        if "struct SpeciesInfo" in contents and "ROGUE_EXPANSION" in contents:
            return candidate.parent
    raise RuntimeError(f"Could not find QueryBaker's BakeHelpers.h in {root}")


def preprocess_species_table(root: Path) -> str:
    query_baker_include = find_query_baker_include(root)
    source = """
#include "BakeHelpers.h"
const union AnimCmd sAnim_GeneralFrame0[] = {};
#include "data/graphics/pokemon.h"
#include "data/pokemon_graphics/front_pic_anims.h"
#include "data/pokemon/form_change_tables.h"
#include "data/pokemon/form_change_table_pointers.h"
#include "data/pokemon/form_species_tables.h"
#include "data/pokemon/species_info.h"
"""
    return preprocess(
        root,
        source,
        (query_baker_include, root / "include", root / "src"),
    )


def find_matching_brace(text: str, opening_index: int) -> int:
    depth = 0
    in_string = False
    escaped = False

    for index in range(opening_index, len(text)):
        character = text[index]
        if in_string:
            if escaped:
                escaped = False
            elif character == "\\":
                escaped = True
            elif character == '"':
                in_string = False
            continue

        if character == '"':
            in_string = True
        elif character == "{":
            depth += 1
        elif character == "}":
            depth -= 1
            if depth == 0:
                return index

    raise ValueError("Unmatched brace in preprocessed species data")


def macro_names(path: Path, prefix: str) -> list[str]:
    contents = path.read_text(encoding="utf-8", errors="ignore")
    return sorted(
        set(
            re.findall(
                rf"^#define\s+({re.escape(prefix)}[A-Z0-9_]+)\b",
                contents,
                re.MULTILINE,
            )
        )
    )


def resolve_macro_values(
    root: Path,
    header: str,
    names: Iterable[str],
) -> dict[str, int]:
    lines = [
        f'#include "{header}"',
        "#define REPORT_VALUE_INNER(label, value) label value",
        "#define REPORT_VALUE(name) REPORT_VALUE_INNER(#name, name)",
    ]
    lines.extend(f"REPORT_VALUE({name})" for name in names)
    output = preprocess(root, "\n".join(lines), (root / "include", root / "src"))

    values: dict[str, int] = {}
    for match in re.finditer(r'"([A-Z0-9_]+)"\s+(.+)', output):
        name, expression = match.groups()
        values[name] = evaluate_expression(expression)
    return values


def species_key_by_id(root: Path) -> dict[int, str]:
    species_header = root / "include" / "constants" / "species.h"
    names = macro_names(species_header, "SPECIES_")
    values = resolve_macro_values(
        root, "constants/species.h", names
    )

    defined_entries: set[str] = set()
    species_info_root = root / "src" / "data" / "pokemon" / "species_info"
    for path in species_info_root.rglob("*.h"):
        contents = path.read_text(encoding="utf-8", errors="ignore")
        defined_entries.update(SPECIES_ENTRY_RE.findall(contents))

    candidates: dict[int, list[str]] = {}
    for name in defined_entries:
        if name in values:
            candidates.setdefault(values[name], []).append(name)

    output: dict[int, str] = {}
    for species_id, species_names in candidates.items():
        output[species_id] = sorted(
            species_names,
            key=lambda name: (
                "_50" in name,
                "_NORMAL" in name,
                len(name),
                name,
            ),
        )[0]
    return output


def reverse_constant_map(
    root: Path,
    header: str,
    path: Path,
    prefix: str,
) -> dict[int, str]:
    names = macro_names(path, prefix)
    values = resolve_macro_values(root, header, names)
    output: dict[int, str] = {}
    for name in names:
        value = values.get(name)
        if value is None:
            continue
        current = output.get(value)
        if current is None or len(name) < len(current):
            output[value] = name
    return output


def extract_field(entry: str, field: str) -> str:
    match = re.search(rf"\.{re.escape(field)}\s*=\s*([^,}}]+)", entry)
    if not match:
        raise ValueError(f"Species entry is missing .{field}")
    return match.group(1).strip()


def extract_array(entry: str, field: str, count: int) -> tuple[int, ...]:
    match = re.search(
        rf"\.{re.escape(field)}\s*=\s*\{{([^}}]+)\}}",
        entry,
    )
    if not match:
        raise ValueError(f"Species entry is missing .{field}")
    values = [
        evaluate_expression(value)
        for value in match.group(1).split(",")
        if value.strip()
    ]
    if len(values) < count:
        values.extend([0] * (count - len(values)))
    return tuple(values[:count])


def extract_display_name(entry: str, key: str) -> str:
    match = re.search(r"\.speciesName\s*=\s*([^,]+)", entry)
    if match:
        fragments = re.findall(r'"((?:\\.|[^"])*)"', match.group(1))
        if fragments:
            return "".join(fragments).replace(r"\'", "'")
    return prettify_constant(key.removeprefix("SPECIES_"))


def prettify_constant(value: str) -> str:
    special_words = {
        "ALOLAN": "Alolan",
        "GALARIAN": "Galarian",
        "HISUIAN": "Hisuian",
        "PALDEAN": "Paldean",
        "MEGA": "Mega",
        "GMAX": "Gigantamax",
        "PH": "Ph.",
        "D": "D",
    }
    return " ".join(
        special_words.get(part, part.title())
        for part in value.split("_")
    )


def species_label(record: SpeciesRecord) -> str:
    raw_key = record.key.removeprefix("SPECIES_")
    normalized_name = re.sub(r"[^A-Z0-9]+", "_", record.name.upper()).strip("_")
    if not raw_key.startswith(normalized_name):
        return prettify_constant(raw_key)

    suffix = raw_key[len(normalized_name) :].strip("_")
    if not suffix:
        return record.name

    parts = suffix.split("_")
    if parts[0] in {"ALOLAN", "GALARIAN", "HISUIAN", "PALDEAN"}:
        region = prettify_constant(parts[0])
        remainder = prettify_constant("_".join(parts[1:]))
        return f"{region} {record.name}" + (
            f" ({remainder})" if remainder else ""
        )
    if parts[0] == "MEGA":
        remainder = prettify_constant("_".join(parts[1:]))
        return f"Mega {record.name}" + (
            f" {remainder}" if remainder else ""
        )
    if parts[0] == "GMAX":
        return f"Gigantamax {record.name}"
    return f"{record.name} ({prettify_constant(suffix)})"


def extract_species_records(root: Path) -> dict[str, SpeciesRecord]:
    preprocessed = preprocess_species_table(root)
    species_keys = species_key_by_id(root)
    type_names = reverse_constant_map(
        root,
        "constants/pokemon.h",
        root / "include" / "constants" / "pokemon.h",
        "TYPE_",
    )
    ability_names = reverse_constant_map(
        root,
        "constants/abilities.h",
        root / "include" / "constants" / "abilities.h",
        "ABILITY_",
    )

    declaration_match = re.search(
        r"const struct SpeciesInfo gSpeciesInfo\[\]\s*=\s*\{",
        preprocessed,
    )
    if declaration_match is None:
        raise RuntimeError("Preprocessed data does not define gSpeciesInfo")
    table_start = preprocessed.find("{", declaration_match.start())
    table_end = find_matching_brace(preprocessed, table_start)
    table = preprocessed[table_start + 1 : table_end]

    records: dict[str, SpeciesRecord] = {}
    cursor = 0
    entry_start_re = re.compile(r"\[\s*([^\]]+)\s*\]\s*=\s*\{")
    while match := entry_start_re.search(table, cursor):
        species_id = evaluate_expression(match.group(1))
        opening_brace = table.find("{", match.start())
        closing_brace = find_matching_brace(table, opening_brace)
        entry = table[opening_brace + 1 : closing_brace]
        cursor = closing_brace + 1

        key = species_keys.get(species_id)
        if key is None or key in {"SPECIES_NONE", "SPECIES_EGG"}:
            continue

        try:
            stats = tuple(
                evaluate_expression(extract_field(entry, field))
                for field in STAT_FIELDS
            )
            type_ids = extract_array(entry, "types", 2)
            ability_ids = extract_array(entry, "abilities", 3)
        except ValueError:
            continue

        if stats[0] == 0:
            continue

        records[key] = SpeciesRecord(
            key=key,
            name=extract_display_name(entry, key),
            stats=stats,
            types=tuple(
                type_names.get(value, f"TYPE_{value}") for value in type_ids
            ),
            abilities=tuple(
                ability_names.get(value, f"ABILITY_{value}")
                for value in ability_ids
            ),
        )

    if len(records) < 1000:
        raise RuntimeError(
            f"Only extracted {len(records)} species; expected at least 1000"
        )
    return records


def archive_ref(ref: str, destination: Path) -> str:
    commit = run(
        ["git", "rev-parse", f"{ref}^{{commit}}"],
        cwd=REPO_ROOT,
    )
    assert isinstance(commit, str)
    commit = commit.strip()

    process = subprocess.run(
        ["git", "archive", "--format=tar", commit],
        cwd=REPO_ROOT,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    if process.returncode != 0:
        raise RuntimeError(
            f"Unable to archive baseline {ref}: "
            f"{process.stderr.decode(errors='replace').strip()}"
        )
    with tarfile.open(fileobj=io.BytesIO(process.stdout), mode="r:") as archive:
        archive.extractall(destination)

    generated_source = REPO_ROOT / "include" / "constants" / "generated"
    generated_destination = (
        destination / "include" / "constants" / "generated"
    )
    if generated_source.exists():
        shutil.copytree(
            generated_source, generated_destination, dirs_exist_ok=True
        )
    return commit


def write_baseline(ref: str) -> None:
    with tempfile.TemporaryDirectory(prefix="rogue-species-baseline-") as temp:
        root = Path(temp)
        commit = archive_ref(ref, root)
        records = extract_species_records(root)

    output = io.StringIO()
    output.write("# Pokabbie feature-za-expansion canonical species data\n")
    output.write("# schema_version=1\n")
    output.write(f"# source_commit={commit}\n")
    writer = csv.writer(output, lineterminator="\n")
    writer.writerow(
        (
            "species",
            "pokemon",
            *STAT_LABELS,
            "type1",
            "type2",
            "ability1",
            "ability2",
            "hidden_ability",
        )
    )
    for key in sorted(records):
        record = records[key]
        writer.writerow(
            (
                record.key,
                record.name,
                *record.stats,
                *record.types,
                *record.abilities,
            )
        )

    BASELINE_PATH.parent.mkdir(parents=True, exist_ok=True)
    BASELINE_PATH.write_text(output.getvalue(), encoding="utf-8")
    print(f"Wrote {len(records)} canonical species to {BASELINE_PATH}")


def load_baseline() -> tuple[str, dict[str, SpeciesRecord]]:
    if not BASELINE_PATH.exists():
        raise RuntimeError(
            f"Missing {BASELINE_PATH}; run with "
            f"--refresh-baseline {DEFAULT_BASELINE_REF}"
        )
    contents = BASELINE_PATH.read_text(encoding="utf-8")
    commit_match = re.search(r"^# source_commit=([0-9a-f]{40})$", contents, re.MULTILINE)
    if commit_match is None:
        raise RuntimeError(f"{BASELINE_PATH} is missing its source commit")
    commit = commit_match.group(1)

    data_lines = [
        line for line in contents.splitlines() if not line.startswith("#")
    ]
    reader = csv.DictReader(data_lines)
    records: dict[str, SpeciesRecord] = {}
    for row in reader:
        key = row["species"]
        records[key] = SpeciesRecord(
            key=key,
            name=row["pokemon"],
            stats=tuple(int(row[label]) for label in STAT_LABELS),
            types=(row["type1"], row["type2"]),
            abilities=(
                row["ability1"],
                row["ability2"],
                row["hidden_ability"],
            ),
        )
    return commit, records


def clean_type_name(value: str) -> str:
    return prettify_constant(value.removeprefix("TYPE_"))


def format_types(values: tuple[str, str]) -> str:
    visible = [values[0]]
    if values[1] != values[0] and values[1] != "TYPE_NONE":
        visible.append(values[1])
    return " / ".join(clean_type_name(value) for value in visible)


def clean_ability_name(value: str) -> str:
    if value == "ABILITY_NONE":
        return "—"
    return prettify_constant(value.removeprefix("ABILITY_"))


def format_abilities(values: tuple[str, str, str]) -> str:
    return " / ".join(clean_ability_name(value) for value in values)


def format_stats(values: tuple[int, ...]) -> str:
    return f"{'/'.join(str(value) for value in values)} ({sum(values)})"


def format_stat_delta(change: SpeciesChange) -> str:
    deltas = tuple(
        current - baseline
        for baseline, current in zip(
            change.baseline.stats, change.current.stats
        )
    )
    stat_delta = "/".join(f"{value:+d}" for value in deltas)
    return f"{stat_delta} (BST {sum(deltas):+d})"


def group_changes(
    changes: Iterable[SpeciesChange],
    category: str,
) -> list[tuple[str, list[SpeciesChange]]]:
    grouped: dict[tuple[str, tuple[object, ...]], list[SpeciesChange]] = {}
    for change in changes:
        key = (change.current.name, change.signature(category))
        grouped.setdefault(key, []).append(change)

    output: list[tuple[str, list[SpeciesChange]]] = []
    for (name, _), members in grouped.items():
        members.sort(key=lambda change: change.current.key)
        label = (
            species_label(members[0].current)
            if len(members) == 1
            else f"{name} ({len(members)} forms)"
        )
        output.append((label, members))
    output.sort(key=lambda item: item[0])
    return output


def markdown_table(
    headers: tuple[str, ...],
    rows: Iterable[tuple[str, ...]],
) -> list[str]:
    output = [
        "| " + " | ".join(headers) + " |",
        "|" + "|".join("---" for _ in headers) + "|",
    ]
    output.extend("| " + " | ".join(row) + " |" for row in rows)
    return output


def render_markdown(
    baseline_commit: str,
    changes: list[SpeciesChange],
    missing_baseline: list[SpeciesRecord],
) -> str:
    stat_changes = [change for change in changes if change.stats_changed]
    type_changes = [change for change in changes if change.types_changed]
    ability_changes = [
        change for change in changes if change.abilities_changed
    ]

    lines = [
        "# Divergence Species Changes",
        "",
        "This file is generated by "
        "`scripts/generate_species_change_report.py`. Do not edit it by hand.",
        "",
        "Run `make species-report` after changing species data. "
        "`make check-species-report` verifies that the committed report is "
        "current.",
        "",
        f"Canonical baseline: `{baseline_commit}` "
        "(Pokabbie `feature-za-expansion`).",
        "",
        "Stat order: **HP / Atk / Def / SpA / SpD / Spe**. Ability order: "
        "**slot 1 / slot 2 / hidden**.",
        "",
        "Unique Abilities are intentionally excluded because they have no "
        "vanilla field to compare.",
        "",
        "## Summary",
        "",
        f"- Base-stat changes: {len(stat_changes)} species/forms",
        f"- Typing changes: {len(type_changes)} species/forms",
        f"- Normal Ability changes: {len(ability_changes)} species/forms",
        f"- Species/forms without a canonical baseline: {len(missing_baseline)}",
        "",
        "## Base Stats",
        "",
    ]

    stat_rows = []
    for label, members in group_changes(stat_changes, "stats"):
        change = members[0]
        stat_rows.append(
            (
                label,
                format_stats(change.baseline.stats),
                format_stats(change.current.stats),
                format_stat_delta(change),
            )
        )
    lines.extend(
        markdown_table(
            ("Pokémon", "Canonical", "Divergence", "Change"),
            stat_rows,
        )
        if stat_rows
        else ["No base-stat changes."]
    )

    lines.extend(["", "## Typing", ""])
    type_rows = []
    for label, members in group_changes(type_changes, "types"):
        change = members[0]
        type_rows.append(
            (
                label,
                format_types(change.baseline.types),
                format_types(change.current.types),
            )
        )
    lines.extend(
        markdown_table(("Pokémon", "Canonical", "Divergence"), type_rows)
        if type_rows
        else ["No typing changes."]
    )

    lines.extend(["", "## Normal Abilities", ""])
    ability_rows = []
    for label, members in group_changes(ability_changes, "abilities"):
        change = members[0]
        ability_rows.append(
            (
                label,
                format_abilities(change.baseline.abilities),
                format_abilities(change.current.abilities),
            )
        )
    lines.extend(
        markdown_table(
            ("Pokémon", "Canonical", "Divergence"), ability_rows
        )
        if ability_rows
        else ["No normal Ability changes."]
    )

    lines.extend(["", "## Missing Canonical Baseline", ""])
    if missing_baseline:
        lines.append(
            "These species/forms exist in Divergence but not in the pinned "
            "canonical snapshot, so they are not counted as changes:"
        )
        lines.append("")
        lines.extend(
            f"- {record.name} (`{record.key}`)"
            for record in sorted(
                missing_baseline, key=lambda record: (record.name, record.key)
            )
        )
    else:
        lines.append("None.")
    lines.append("")
    return "\n".join(lines)


def render_csv(
    baseline: dict[str, SpeciesRecord],
    current: dict[str, SpeciesRecord],
) -> str:
    output = io.StringIO()
    writer = csv.writer(output, lineterminator="\n")
    writer.writerow(
        (
            "species",
            "pokemon",
            "canonical_hp",
            "canonical_atk",
            "canonical_def",
            "canonical_spa",
            "canonical_spd",
            "canonical_spe",
            "divergence_hp",
            "divergence_atk",
            "divergence_def",
            "divergence_spa",
            "divergence_spd",
            "divergence_spe",
            "canonical_types",
            "divergence_types",
            "canonical_abilities",
            "divergence_abilities",
            "stats_changed",
            "typing_changed",
            "abilities_changed",
        )
    )

    for key in sorted(set(baseline) & set(current)):
        old = baseline[key]
        new = current[key]
        change = SpeciesChange(old, new)
        if not (
            change.stats_changed
            or change.types_changed
            or change.abilities_changed
        ):
            continue
        writer.writerow(
            (
                key,
                new.name,
                *old.stats,
                *new.stats,
                format_types(old.types),
                format_types(new.types),
                format_abilities(old.abilities),
                format_abilities(new.abilities),
                str(change.stats_changed).lower(),
                str(change.types_changed).lower(),
                str(change.abilities_changed).lower(),
            )
        )
    return output.getvalue()


def update_or_check(path: Path, contents: str, check: bool) -> bool:
    existing = path.read_text(encoding="utf-8") if path.exists() else None
    if existing == contents:
        print(f"Up to date: {path.relative_to(REPO_ROOT)}")
        return True
    if check:
        print(f"Out of date: {path.relative_to(REPO_ROOT)}", file=sys.stderr)
        return False
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(contents, encoding="utf-8")
    print(f"Wrote {path.relative_to(REPO_ROOT)}")
    return True


def generate_reports(check: bool) -> bool:
    baseline_commit, baseline = load_baseline()
    current = extract_species_records(REPO_ROOT)
    shared_keys = sorted(set(baseline) & set(current))
    changes = [
        SpeciesChange(baseline[key], current[key]) for key in shared_keys
    ]
    changes = [
        change
        for change in changes
        if (
            change.stats_changed
            or change.types_changed
            or change.abilities_changed
        )
    ]
    missing_baseline = [
        current[key] for key in sorted(set(current) - set(baseline))
    ]

    markdown = render_markdown(
        baseline_commit, changes, missing_baseline
    )
    csv_contents = render_csv(baseline, current)
    markdown_ok = update_or_check(MARKDOWN_PATH, markdown, check)
    csv_ok = update_or_check(CSV_PATH, csv_contents, check)
    return markdown_ok and csv_ok


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--check",
        action="store_true",
        help="fail if the committed reports do not match current species data",
    )
    parser.add_argument(
        "--refresh-baseline",
        metavar="GIT_REF",
        nargs="?",
        const=DEFAULT_BASELINE_REF,
        help="regenerate the canonical baseline from a pinned Git ref",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.check and args.refresh_baseline:
        raise RuntimeError("--check and --refresh-baseline cannot be combined")
    if args.refresh_baseline:
        write_baseline(args.refresh_baseline)
    return 0 if generate_reports(args.check) else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (RuntimeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        raise SystemExit(1)
