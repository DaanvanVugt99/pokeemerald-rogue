#!/usr/bin/env python3
"""Focused regression checks for the rebalance reconciliation tooling."""
import copy
import json
import unittest

import check_rebalance_manifest as checker
from generate_species_change_report import SpeciesRecord


class RebalanceManifestTests(unittest.TestCase):
    def setUp(self):
        self.key = "SPECIES_EXAMPLE"
        self.base = SpeciesRecord(self.key, "Example", (50, 60, 70, 80, 90, 100),
                                  ("TYPE_BUG", "TYPE_BUG"), ("ABILITY_NONE",) * 3)
        self.records = {self.key: self.base}
        self.profiles = {self.key: ([("MOVE_TACKLE", 1)], {"MOVE_PROTECT"})}

    def verify(self, rule, current=None, profiles=None):
        return checker.check_species(self.key, rule, current or self.records, self.records,
                                     self.records, profiles or self.profiles, self.profiles)

    def test_keep_requires_exact_preservation(self):
        self.assertEqual(self.verify({}), [])
        changed = copy.deepcopy(self.profiles)
        changed[self.key][1].add("MOVE_HURRICANE")
        self.assertIn("unexpected", self.verify({}, profiles=changed)[0])

    def test_delta_uses_canonical_not_already_buffed_stats(self):
        changed = SpeciesRecord(self.key, "Example", (50, 60, 70, 90, 90, 100),
                                self.base.types, self.base.abilities)
        already_buffed = SpeciesRecord(self.key, "Example", (50, 60, 70, 85, 90, 100),
                                       self.base.types, self.base.abilities)
        self.assertEqual(checker.check_species(
            self.key, {"stat_delta": [0, 0, 0, 10, 0, 0]}, {self.key: changed},
            {self.key: already_buffed}, self.records, self.profiles, self.profiles), [])
        # SpA is fourth, Speed is sixth; swapping the two must fail.
        self.assertTrue(self.verify({"stat_delta": [0, 0, 0, 0, 0, 10]},
                                    {self.key: changed}))

    def test_missing_or_extra_moves_fail(self):
        self.assertTrue(self.verify({"add_tutors": ["MOVE_POWER_GEM"]}))
        self.assertTrue(self.verify({"level_moves": {"MOVE_TACKLE": 4}}))

    def test_explicit_removals_and_level_replacement(self):
        expected = checker.expected_profile(self.profiles[self.key], {
            "level_moves": {"MOVE_TACKLE": 4}, "remove_tutors": ["MOVE_PROTECT"],
            "add_tutors": ["MOVE_POWER_GEM"]})
        self.assertEqual(expected, ([("MOVE_TACKLE", 4)], {"MOVE_POWER_GEM"}))

    def test_form_redirects_resolve_actual_arrays(self):
        text = '''static struct LevelUpMove const sLevelUpMoves_SPECIES_BASE[] =
{
 { .move=MOVE_TACKLE, .level=1 },
 { .move=MOVE_NONE, .level=0 },
};
static u16 const sTutorMoves_SPECIES_BASE[] =
{
 MOVE_PROTECT,
};
[SPECIES_BASE] = { .levelUpMoves = sLevelUpMoves_SPECIES_BASE, .tutorMoves = sTutorMoves_SPECIES_BASE },
[SPECIES_MEGA] = { .levelUpMoves = sLevelUpMoves_SPECIES_BASE, .tutorMoves = sTutorMoves_SPECIES_BASE },
'''
        profiles = checker.parse_profiles(text)
        self.assertEqual(profiles["SPECIES_BASE"], profiles["SPECIES_MEGA"])
        self.assertEqual(profiles["SPECIES_BASE"], self.profiles[self.key])

    def test_ledger_keys_are_section_specific_and_detect_edits(self):
        text = "## One\n| Pokemon | Decision |\n|---|---|\n| A | Keep |\n\n## Two\n| Pokemon | Decision |\n|---|---|\n| A | Change |\n"
        rows = checker.ledger_rows(text)
        self.assertEqual(set(rows), {"One | A", "Two | A"})
        self.assertNotEqual(rows["One | A"], rows["Two | A"])
        self.assertNotEqual(rows, checker.ledger_rows(text.replace("Keep", "Port")))

    def test_save_move_ids_remain_stable_and_new_ids_cannot_collide(self):
        before = "#define MOVE_OLD 848\n#define MOVE_Z (MOVES_COUNT + 0)\n"
        self.assertEqual(checker.check_learnable_move_ids(before, before + "#define MOVE_NEW 863\n"), [])
        self.assertTrue(checker.check_learnable_move_ids(before, before.replace("848", "849")))
        self.assertTrue(checker.check_learnable_move_ids(before, before + "#define MOVE_NEW 848\n"))

    def test_profile_alias_uses_the_species_table_key(self):
        profiles = {"SPECIES_ALIAS": self.profiles[self.key]}
        aliases = "#define SPECIES_ALIAS SPECIES_EXAMPLE\n"
        self.assertEqual(checker.normalize_profile_aliases(profiles, self.records, aliases), self.profiles)

    def test_duplicate_manifest_targets_are_not_silently_overwritten(self):
        with self.assertRaisesRegex(ValueError, "Duplicate manifest key"):
            json.loads('{"SPECIES_EXAMPLE": {}, "SPECIES_EXAMPLE": {}}',
                       object_pairs_hook=checker.unique_json_keys)

    def test_compiled_evolutions_include_regional_branches_and_ignore_sentinels(self):
        text = '''const struct SpeciesInfo gSpeciesInfo[] = {
 [1] = { .evolutions = (const struct Evolution[]) { {4, 36, 2}, {0, 0, 3}, {0xFFFF}, }, },
 [2] = { .baseHP = 50 },
};'''
        self.assertEqual(checker.evolution_edges(text, {1: "A", 2: "B", 3: "C"}),
                         {("A", "B"), ("A", "C")})

    def test_new_access_must_accumulate_through_all_later_stages(self):
        old = {name: ([("MOVE_TACKLE", 1)], set()) for name in ("A", "B", "C")}
        new = copy.deepcopy(old)
        new["A"][1].add("MOVE_RECOVER")
        new["B"][1].add("MOVE_RECOVER")
        gaps, historical = checker.accumulation_gaps(
            {("A", "B"), ("B", "C")}, new, old, {"A", "B", "C"})
        self.assertEqual({(g["source"], g["target"], g["move"]) for g in gaps},
                         {("A", "C", "MOVE_RECOVER"), ("B", "C", "MOVE_RECOVER")})
        self.assertEqual(historical, [])

    def test_existing_gaps_are_not_claimed_as_new_regressions(self):
        profiles = {"A": ([("MOVE_TACKLE", 1)], {"MOVE_RECOVER"}),
                    "B": ([("MOVE_TACKLE", 10)], set())}
        new, historical = checker.accumulation_gaps({("A", "B")}, profiles, profiles, {"A"})
        self.assertEqual(new, [])
        self.assertEqual(historical, [{"source": "A", "target": "B", "move": "MOVE_RECOVER", "covered": False}])

    def test_bst_invariant_is_independent_of_individual_stat_targets(self):
        rule = {"id": "pair", "kind": "bst", "species": [self.key], "value": 450}
        self.assertEqual(checker.check_invariants([rule], self.records, self.records), [])
        rule["value"] = 465
        self.assertTrue(checker.check_invariants([rule], self.records, self.records))

    def test_ability_count_does_not_count_duplicate_or_empty_slots(self):
        record = SpeciesRecord(self.key, "Example", self.base.stats, self.base.types,
                               ("ABILITY_STATIC", "ABILITY_STATIC", "ABILITY_NONE"))
        rule = {"id": "count", "kind": "distinct_abilities", "species": [self.key], "value": 3}
        self.assertTrue(checker.check_invariants([rule], {self.key: record}, self.records))
        rule["value"] = 1
        self.assertEqual(checker.check_invariants([rule], {self.key: record}, self.records), [])

    def test_starter_second_ability_is_present_and_distinct(self):
        rule = {"id": "starter", "kind": "second_ability", "species": [self.key], "value": "ABILITY_STATIC"}
        record = SpeciesRecord(self.key, "Example", self.base.stats, self.base.types,
                               ("ABILITY_BLAZE", "ABILITY_STATIC", "ABILITY_SOLAR_POWER"))
        self.assertEqual(checker.check_invariants([rule], {self.key: record}, self.records), [])
        duplicate = SpeciesRecord(self.key, "Example", self.base.stats, self.base.types,
                                  ("ABILITY_STATIC", "ABILITY_STATIC", "ABILITY_SOLAR_POWER"))
        self.assertTrue(checker.check_invariants([rule], {self.key: duplicate}, self.records))

    def test_mega_invariant_compares_deltas_not_absolute_stats(self):
        mega = "SPECIES_EXAMPLE_MEGA"
        baseline = {**self.records, mega: SpeciesRecord(mega, "Mega", (50, 100, 90, 100, 110, 100),
                                                      self.base.types, self.base.abilities)}
        current = {key: SpeciesRecord(key, record.name, tuple(n + 5 for n in record.stats),
                                     record.types, record.abilities) for key, record in baseline.items()}
        rule = {"id": "mega", "kind": "matching_stat_deltas", "species": [self.key, mega]}
        self.assertEqual(checker.check_invariants([rule], current, baseline), [])
        current[mega] = baseline[mega]
        self.assertTrue(checker.check_invariants([rule], current, baseline))
        self.assertTrue(checker.check_invariants([rule], current, self.records))

    def test_uncovered_shared_species_changes_fail_coverage(self):
        changed = SpeciesRecord(self.key, "Example", (60, 60, 70, 80, 90, 100),
                                ("TYPE_BUG", "TYPE_FAIRY"), self.base.abilities)
        self.assertEqual(checker.uncovered_changes(
            {self.key: changed}, self.records, self.profiles, self.profiles, set()),
            [{"species": self.key, "fields": ["stats", "types"]}])
        self.assertEqual(checker.uncovered_changes(
            {self.key: changed}, self.records, self.profiles, self.profiles, {self.key}), [])

    def test_uncovered_form_inheritance_is_not_silently_accepted(self):
        profiles = copy.deepcopy(self.profiles)
        profiles[self.key][1].add("MOVE_CRABHAMMER")
        self.assertEqual(checker.uncovered_changes(
            self.records, self.records, profiles, self.profiles, set()),
            [{"species": self.key, "fields": ["learnset"]}])
        self.assertEqual(checker.uncovered_changes(
            self.records, self.records, self.profiles, self.profiles, set()), [])

    def test_malformed_invariants_cannot_silently_pass(self):
        rule = {"id": "one", "kind": "bst", "species": [self.key], "value": 450}
        for change in ({"kind": "unknown"}, {"species": []}, {"species": [self.key, self.key]},
                       {"species": ["SPECIES_MISSING"]}, {"value": True}, {"typo": 450}):
            with self.subTest(change=change):
                self.assertTrue(checker.check_invariants([{**rule, **change}], self.records, self.records))
        self.assertTrue(checker.check_invariants([rule, rule], self.records, self.records))


if __name__ == "__main__":
    unittest.main()
