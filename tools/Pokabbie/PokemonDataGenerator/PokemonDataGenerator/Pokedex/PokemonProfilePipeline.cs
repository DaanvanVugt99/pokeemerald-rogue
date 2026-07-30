using Newtonsoft.Json;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using System.Text.RegularExpressions;

namespace PokemonDataGenerator.Pokedex
{
	internal static class PokemonProfilePipeline
	{
		private const int SchemaVersion = 1;
		private static readonly string[] s_SourceUrls =
		{
			"https://pokeapi.co/api/v2/",
			"https://play.pokemonshowdown.com/data/pokedex.json",
			"https://play.pokemonshowdown.com/data/sets/gen6.json",
			"https://play.pokemonshowdown.com/data/sets/gen7.json",
			"https://play.pokemonshowdown.com/data/sets/gen8.json",
			"https://play.pokemonshowdown.com/data/sets/gen9.json",
		};
		private static readonly JsonSerializerSettings s_JsonSettings = new JsonSerializerSettings
		{
			NullValueHandling = NullValueHandling.Ignore,
		};
		private static HashSet<string> s_ValidMonFlagTiers;

		private class CachedSourceBundle
		{
			public int SchemaVersion;
			public string RefreshedAtUtc;
			public string SourceAggregateSha256;
			public PokemonProfileGenerator.ProfileBundle Bundle;
		}

		private class SourceLock
		{
			public int SchemaVersion;
			public string RefreshedAtUtc;
			public string SourceAggregateSha256;
			public string CachedBundleSha256;
			public string[] Sources;
			public Dictionary<string, int> TransformationMatches = new Dictionary<string, int>();
		}

		private class CuratedSetRuleFile
		{
			public int SchemaVersion { get; set; }
			public List<CuratedSetRule> Sets { get; set; } = new List<CuratedSetRule>();
		}

		private class CuratedSetRule
		{
			public string StableId { get; set; }
			public string Species { get; set; }
			public string RogueRole { get; set; }
			public string Ability { get; set; }
			public string Item { get; set; }
			public string Nature { get; set; }
			public string HiddenPower { get; set; }
			public string TeraType { get; set; }
			public List<string> Moves { get; set; } = new List<string>();
			public List<string> SourceTiers { get; set; } = new List<string>();
		}

		private class ZaMegaExpectationFile
		{
			public int SchemaVersion { get; set; }
			public Dictionary<string, string> ExpectedBases { get; set; } = new Dictionary<string, string>();
		}

		private static string PipelineResourceDirectory =>
			Path.Combine(
				GameDataHelpers.RootDirectory,
				"tools",
				"Pokabbie",
				"PokemonDataGenerator",
				"PokemonDataGenerator",
				"Resources",
				"PokemonProfiles",
				"Pipeline");
		private static string LockPath => Path.Combine(PipelineResourceDirectory, "profile_sources.lock.json");
		private static string CuratedSetsPath => Path.Combine(PipelineResourceDirectory, "divergence_competitive_sets.json");
		private static string ZaMegaExpectationsPath => Path.Combine(PipelineResourceDirectory, "za_mega_expected_bases.json");
		private static string HeaderPath => Path.Combine(GameDataHelpers.RootDirectory, "src", "data", "rogue_pokemon_profiles.h");
		private static string WorkDirectory => Path.Combine(GameDataHelpers.RootDirectory, "build", "profile_pipeline");
		private static string ActiveCachePath => Path.Combine(WorkDirectory, "upstream_bundle.cache.json");
		private static string CandidateCachePath => Path.Combine(WorkDirectory, "upstream_bundle.candidate.json");
		private static string CandidateHeaderPath => Path.Combine(WorkDirectory, "rogue_pokemon_profiles.candidate.h");
		private static string ReportPath => Path.Combine(WorkDirectory, "refresh_report.md");

		public static void Run(string command, bool acceptRemovals)
		{
			GameDataHelpers.IsVanillaVersion = false;
			switch (command)
			{
				case "refresh":
					Refresh(acceptRemovals);
					break;
				case "generate":
					Generate(HeaderPath);
					break;
				case "verify":
					Verify();
					break;
				default:
					throw new ArgumentException(
						$"Unknown profile pipeline command '{command}'. Expected refresh, generate, or verify.");
			}
		}

		private static void Refresh(bool acceptRemovals)
		{
			Directory.CreateDirectory(WorkDirectory);
			Console.WriteLine("Refreshing EX profiles from live PokeAPI and Showdown data...");

			CachedSourceBundle candidate = null;
			try
			{
				ContentCache.ResetFetchedContentTracking();
				PokemonProfileGenerator.ProfileBundle source =
					PokemonProfileGenerator.GatherSourceProfileBundle(true);
				candidate = new CachedSourceBundle
				{
					SchemaVersion = SchemaVersion,
					RefreshedAtUtc = DateTime.UtcNow.ToString("o"),
					SourceAggregateSha256 = ContentCache.GetFetchedContentAggregateHash(),
					Bundle = source,
				};
				WriteJsonAtomic(CandidateCachePath, candidate, Formatting.None);

				ValidateZaMegaResolutions(source, true);
				PokemonProfileGenerator.ProfileBundle finalBundle = BuildFinalBundle(source);
				ValidateFinalBundle(finalBundle);
				ExportBundleAtomic(finalBundle, CandidateHeaderPath);

				SourceLock previousLock = File.Exists(LockPath) ? LoadLock() : null;
				PokemonProfileGenerator.ProfileBundle previousFinal = null;
				List<string> destructiveChanges = new List<string>();
				if (previousLock != null && File.Exists(ActiveCachePath))
				{
					CachedSourceBundle previousCache = LoadActiveCache(previousLock);
					previousFinal = BuildFinalBundle(previousCache.Bundle);
					destructiveChanges.AddRange(
						FindDestructiveChanges(previousFinal, finalBundle));
					destructiveChanges.AddRange(
						FindTransformationMatchDrift(
							previousLock.TransformationMatches,
							finalBundle.TransformationMatches));
				}
				destructiveChanges = destructiveChanges.Distinct().OrderBy(change => change).ToList();

				File.WriteAllText(
					ReportPath,
					BuildRefreshReport(candidate, finalBundle, previousFinal, destructiveChanges));
				if (destructiveChanges.Any() && !acceptRemovals)
				{
					throw new InvalidDataException(
						"Profile refresh contains destructive changes. "
						+ "The candidate cache, header, and report are under build/profile_pipeline. "
						+ "Review them and rerun refresh --accept-removals to promote them.");
				}

				SourceLock sourceLock = new SourceLock
				{
					SchemaVersion = SchemaVersion,
					RefreshedAtUtc = candidate.RefreshedAtUtc,
					SourceAggregateSha256 = candidate.SourceAggregateSha256,
					CachedBundleSha256 = ComputeFileSha256(CandidateCachePath),
					Sources = s_SourceUrls,
					TransformationMatches = new Dictionary<string, int>(
						finalBundle.TransformationMatches),
				};
				string candidateLockPath = Path.Combine(WorkDirectory, "profile_sources.lock.candidate.json");
				WriteJsonAtomic(candidateLockPath, sourceLock, Formatting.Indented);

				ReplaceAtomically(CandidateCachePath, ActiveCachePath);
				ReplaceAtomically(CandidateHeaderPath, HeaderPath);
				ReplaceAtomically(candidateLockPath, LockPath);
				Console.WriteLine($"Updated source lock: {LockPath}");
				Console.WriteLine($"Updated generated header: {HeaderPath}");
				Console.WriteLine($"Refresh report: {ReportPath}");
			}
			catch (Exception exception)
			{
				File.WriteAllText(
					Path.Combine(WorkDirectory, "refresh_failure.md"),
					"# Pokémon Profile Refresh Failure\n\n"
					+ (candidate == null
						? "The live sources could not be normalized. No tracked profile data was changed.\n\n"
						: "The candidate remains under `build/profile_pipeline/` for inspection.\n\n")
					+ "```\n" + exception + "\n```\n");
				throw;
			}
		}

		private static void Generate(string outputPath)
		{
			SourceLock sourceLock = LoadLock();
			CachedSourceBundle cache = LoadActiveCache(sourceLock);
			ValidateZaMegaResolutions(cache.Bundle, false);

			PokemonProfileGenerator.ProfileBundle bundle = BuildFinalBundle(cache.Bundle);
			ValidateFinalBundle(bundle);
			List<string> transformationDrift =
				FindTransformationMatchDrift(
					sourceLock.TransformationMatches,
					bundle.TransformationMatches).ToList();
			if (transformationDrift.Any())
			{
				throw new InvalidDataException(
					"The cached profile sources no longer produce the locked transformations:\n"
					+ string.Join("\n", transformationDrift));
			}

			ExportBundleAtomic(bundle, outputPath);
			Console.WriteLine($"Generated profiles from the locked local cache: {outputPath}");
		}

		private static PokemonProfileGenerator.ProfileBundle BuildFinalBundle(
			PokemonProfileGenerator.ProfileBundle source)
		{
			PokemonProfileGenerator.ProfileBundle bundle = Clone(source);
			PokemonProfileGenerator.PrepareProfilesForGame(bundle);
			AppendCuratedSets(bundle);
			return bundle;
		}

		private static void Verify()
		{
			Directory.CreateDirectory(WorkDirectory);
			string firstPath = Path.Combine(WorkDirectory, "rogue_pokemon_profiles.verify.first.h");
			string secondPath = Path.Combine(WorkDirectory, "rogue_pokemon_profiles.verify.second.h");
			Generate(firstPath);
			Generate(secondPath);

			byte[] first = File.ReadAllBytes(firstPath);
			if (!first.SequenceEqual(File.ReadAllBytes(secondPath)))
				throw new InvalidDataException("Two consecutive profile generations were not byte-identical.");
			if (!first.SequenceEqual(File.ReadAllBytes(HeaderPath)))
				throw new InvalidDataException(
					"Tracked Pokémon profile header is stale. Run generate and review the diff.");

			File.Delete(firstPath);
			File.Delete(secondPath);
			Console.WriteLine("Tracked Pokémon profile header is deterministic and up to date.");
		}

		private static SourceLock LoadLock()
		{
			if (!File.Exists(LockPath))
				throw new FileNotFoundException("No profile source lock exists. Run refresh first.", LockPath);
			SourceLock sourceLock = JsonConvert.DeserializeObject<SourceLock>(
				File.ReadAllText(LockPath),
				s_JsonSettings);
			if (sourceLock == null
				|| sourceLock.SchemaVersion != SchemaVersion
				|| string.IsNullOrEmpty(sourceLock.SourceAggregateSha256)
				|| string.IsNullOrEmpty(sourceLock.CachedBundleSha256))
				throw new InvalidDataException("The profile source lock is malformed or unsupported.");
			return sourceLock;
		}

		private static CachedSourceBundle LoadActiveCache(SourceLock sourceLock)
		{
			if (!File.Exists(ActiveCachePath))
			{
				throw new FileNotFoundException(
					"The ignored local profile cache is missing. Run refresh before generate or verify.",
					ActiveCachePath);
			}
			if (ComputeFileSha256(ActiveCachePath) != sourceLock.CachedBundleSha256)
			{
				throw new InvalidDataException(
					"The local profile cache does not match the tracked source lock. Run refresh.");
			}

			CachedSourceBundle cache = JsonConvert.DeserializeObject<CachedSourceBundle>(
				File.ReadAllText(ActiveCachePath),
				s_JsonSettings);
			if (cache == null
				|| cache.SchemaVersion != SchemaVersion
				|| cache.Bundle == null
				|| cache.SourceAggregateSha256 != sourceLock.SourceAggregateSha256)
				throw new InvalidDataException("The local profile cache is malformed or does not match its lock.");
			return cache;
		}

		private static void AppendCuratedSets(PokemonProfileGenerator.ProfileBundle bundle)
		{
			CuratedSetRuleFile rules = JsonConvert.DeserializeObject<CuratedSetRuleFile>(
				File.ReadAllText(CuratedSetsPath),
				s_JsonSettings);
			AppendCuratedSets(bundle, rules);
		}

		private static void AppendCuratedSets(
			PokemonProfileGenerator.ProfileBundle bundle,
			CuratedSetRuleFile ruleFile)
		{
			if (ruleFile == null || ruleFile.SchemaVersion != 1)
				throw new InvalidDataException("The curated-set rule file is malformed or unsupported.");
			if (ruleFile.Sets == null
				|| ruleFile.Sets.Any(rule => rule == null || string.IsNullOrWhiteSpace(rule.StableId))
				|| ruleFile.Sets.GroupBy(rule => rule.StableId).Any(group => group.Count() != 1))
				throw new InvalidDataException("Curated competitive-set IDs must be unique.");

			Dictionary<string, PokemonProfileGenerator.PokemonProfile> profiles =
				bundle.Profiles.ToDictionary(profile => profile.Species);
			foreach (CuratedSetRule rule in ruleFile.Sets)
			{
				if (!profiles.TryGetValue(rule.Species, out PokemonProfileGenerator.PokemonProfile profile))
					throw new InvalidDataException($"Curated set {rule.StableId} targets missing profile {rule.Species}.");
				if (!new[] { "standard", "singles-strong", "doubles-strong", "strong-wild" }.Contains(rule.RogueRole))
					throw new InvalidDataException($"Curated set {rule.StableId} has invalid Rogue role {rule.RogueRole}.");
				if (rule.Moves == null || rule.Moves.Count != 4 || rule.Moves.Distinct().Count() != 4)
					throw new InvalidDataException($"Curated set {rule.StableId} must contain four distinct moves.");
				if (rule.Moves.Any(move => !GameDataHelpers.MoveDefines.ContainsKey(move)))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid move constant.");
				if (rule.Ability != null && !GameDataHelpers.AbilityDefines.ContainsKey(rule.Ability))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid Ability constant.");
				if (rule.Item != null && !GameDataHelpers.ItemDefines.ContainsKey(rule.Item))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid item constant.");
				if (rule.Nature != null && !GameDataHelpers.NatureDefines.ContainsKey(rule.Nature))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid nature constant.");
				if (rule.HiddenPower != null && !GameDataHelpers.TypesDefines.ContainsKey(rule.HiddenPower))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid Hidden Power type.");
				if (rule.TeraType != null && !GameDataHelpers.TypesDefines.ContainsKey(rule.TeraType))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid Tera type.");

				List<string> sourceTiers = (rule.SourceTiers ?? new List<string>())
					.Select(GameDataHelpers.FormatKeyword)
					.ToList();
				if (sourceTiers.Distinct().Count() != sourceTiers.Count
					|| sourceTiers.Any(tier => !GetValidMonFlagTiers().Contains(tier)))
					throw new InvalidDataException($"Curated set {rule.StableId} contains an invalid source tier.");

				PokemonProfileGenerator.PokemonCompetitiveSet curated =
					new PokemonProfileGenerator.PokemonCompetitiveSet
					{
						StableId = rule.StableId,
						Provenance = "divergence-curated",
						RogueRole = rule.RogueRole,
						Ability = rule.Ability,
						Item = rule.Item,
						Nature = rule.Nature,
						HiddenPower = rule.HiddenPower,
						TeraType = rule.TeraType,
						Moves = new List<string>(rule.Moves),
						SourceTiers = sourceTiers,
					};
				if (profile.CompetitiveSets.Any(set => SetFingerprint(set) == SetFingerprint(curated)))
					throw new InvalidDataException($"Curated set {rule.StableId} duplicates an existing set.");
				foreach (string move in curated.Moves)
				{
					if (!profile.CanLearnMove(move))
						throw new InvalidDataException($"Curated set {rule.StableId} contains illegal move {move}.");
				}
				profile.CompetitiveSets.Add(curated);
			}
		}

		private static HashSet<string> GetValidMonFlagTiers()
		{
			if (s_ValidMonFlagTiers == null)
			{
				string constantsPath = Path.Combine(
					GameDataHelpers.RootDirectory,
					"include",
					"constants",
					"rogue.h");
				s_ValidMonFlagTiers = new HashSet<string>(
					Regex.Matches(
							File.ReadAllText(constantsPath),
							@"^\s*#define\s+MON_FLAGS_([A-Z0-9_]+)\b",
							RegexOptions.Multiline)
						.Cast<Match>()
						.Select(match => match.Groups[1].Value));
			}
			return s_ValidMonFlagTiers;
		}

		private static void ValidateZaMegaResolutions(
			PokemonProfileGenerator.ProfileBundle bundle,
			bool validateLivePokeApiForms)
		{
			ZaMegaExpectationFile expectations = JsonConvert.DeserializeObject<ZaMegaExpectationFile>(
				File.ReadAllText(ZaMegaExpectationsPath),
				s_JsonSettings);
			if (expectations == null
				|| expectations.SchemaVersion != 1
				|| expectations.ExpectedBases.Count != 49)
				throw new InvalidDataException("The Z-A Mega expectations must contain exactly 49 forms.");
			if (bundle.ZaMegaResolutions.Count != expectations.ExpectedBases.Count)
				throw new InvalidDataException(
					$"Expected {expectations.ExpectedBases.Count} Z-A Mega resolutions, found {bundle.ZaMegaResolutions.Count}.");

			foreach (var resolution in expectations.ExpectedBases)
			{
				if (!bundle.ZaMegaResolutions.TryGetValue(resolution.Key, out string actualBase))
					throw new InvalidDataException($"Expected Z-A Mega form disappeared: {resolution.Key}");
				if (actualBase != resolution.Value)
					throw new InvalidDataException(
						$"Z-A Mega base changed for {resolution.Key}: {resolution.Value} -> {actualBase}");
				if (!GameDataHelpers.SpeciesDefines.ContainsKey(resolution.Key)
					|| !GameDataHelpers.SpeciesDefines.ContainsKey(actualBase))
					throw new InvalidDataException($"Invalid Z-A Mega resolution: {resolution.Key} -> {actualBase}");
				if (validateLivePokeApiForms && !PokeAPI.HasPokemonForm(resolution.Key))
					throw new InvalidDataException($"Expected Z-A Mega form disappeared from PokeAPI: {resolution.Key}");
			}

			if (bundle.SpeciesMetadata["SPECIES_MEOWSTIC_M_MEGA"].PokeApiForm != "meowstic-male-mega"
				|| bundle.SpeciesMetadata["SPECIES_MEOWSTIC_F_MEGA"].PokeApiForm != "meowstic-female-mega")
				throw new InvalidDataException("PokeAPI Meowstic Mega naming adapters no longer resolve.");
		}

		private static void ValidateFinalBundle(PokemonProfileGenerator.ProfileBundle bundle)
		{
			if (bundle.Profiles.GroupBy(profile => profile.Species).Any(group => group.Count() != 1))
				throw new InvalidDataException("Final profile table contains duplicate species entries.");
			Dictionary<string, PokemonProfileGenerator.PokemonProfile> profiles =
				bundle.Profiles.ToDictionary(profile => profile.Species);

			foreach (string species in profiles.Keys)
			{
				if (!GameDataHelpers.SpeciesDefines.ContainsKey(species)
					|| !GameDataHelpers.IsUniqueSpeciesDefine(species))
					throw new InvalidDataException($"Final profile table contains illegal species constant: {species}");
			}
			foreach (var redirect in bundle.RedirectedSpecies)
			{
				if (!GameDataHelpers.SpeciesDefines.ContainsKey(redirect.Key)
					|| !profiles.ContainsKey(redirect.Value))
					throw new InvalidDataException($"Profile redirect cannot be resolved: {redirect.Key} -> {redirect.Value}");
				if (bundle.SpeciesMetadata.TryGetValue(
						redirect.Key,
						out PokemonProfileGenerator.SpeciesSourceMetadata metadata)
					&& metadata.HasDedicatedCompetitiveSets)
				{
					throw new InvalidDataException(
						$"{redirect.Key} has dedicated competitive sets but redirects to {redirect.Value}.");
				}
			}
			foreach (string species in GameDataHelpers.SpeciesDefines.Keys.Where(GameDataHelpers.IsUniqueSpeciesDefine))
			{
				if (!profiles.ContainsKey(species) && !bundle.RedirectedSpecies.ContainsKey(species))
					throw new InvalidDataException($"Project species is missing from the profile table: {species}");
			}

			foreach (PokemonProfileGenerator.PokemonProfile profile in bundle.Profiles)
			{
				profile.ValidateContents();
				if (profile.CompetitiveSets.GroupBy(SetFingerprint).Any(group => group.Count() != 1))
					throw new InvalidDataException($"{profile.Species} contains duplicate competitive sets.");
				foreach (PokemonProfileGenerator.PokemonCompetitiveSet set in profile.CompetitiveSets)
				{
					if (set.Moves.Count == 0
						|| set.Moves.Count > 4
						|| set.Moves.Distinct().Count() != set.Moves.Count)
						throw new InvalidDataException($"{profile.Species} contains a malformed competitive set.");
					foreach (string move in set.Moves)
					{
						if (!profile.CanLearnMove(move))
							throw new InvalidDataException(
								$"{profile.Species} competitive set contains illegal move {move}.");
					}
				}
			}
		}

		private static IEnumerable<string> FindDestructiveChanges(
			PokemonProfileGenerator.ProfileBundle previous,
			PokemonProfileGenerator.ProfileBundle candidate)
		{
			Dictionary<string, PokemonProfileGenerator.PokemonProfile> candidateProfiles =
				candidate.Profiles.ToDictionary(profile => profile.Species);
			foreach (PokemonProfileGenerator.PokemonProfile oldProfile in previous.Profiles)
			{
				if (!candidateProfiles.TryGetValue(
						oldProfile.Species,
						out PokemonProfileGenerator.PokemonProfile newProfile))
				{
					yield return $"{oldProfile.Species}: profile removed";
					continue;
				}
				foreach (var move in oldProfile.LevelUpMoves.Where(
					move => !newProfile.LevelUpMoves.Any(
						other => other.Move == move.Move && other.Level == move.Level)))
					yield return $"{oldProfile.Species}: level-up entry removed: {move.Move}@{move.Level}";
				foreach (string move in oldProfile.TutorMoves.Except(newProfile.TutorMoves))
					yield return $"{oldProfile.Species}: tutor move removed: {move}";
				foreach (var set in oldProfile.CompetitiveSets.Where(
					set => !newProfile.CompetitiveSets.Any(
						other => SetFingerprint(other) == SetFingerprint(set))))
					yield return $"{oldProfile.Species}: competitive set removed: {SetDisplayId(set)}";
			}
			foreach (var redirect in previous.RedirectedSpecies)
			{
				if (!candidate.RedirectedSpecies.TryGetValue(redirect.Key, out string target))
					yield return $"{redirect.Key}: redirect removed ({redirect.Value})";
				else if (target != redirect.Value)
					yield return $"{redirect.Key}: redirect changed ({redirect.Value} -> {target})";
			}
		}

		private static IEnumerable<string> FindTransformationMatchDrift(
			Dictionary<string, int> previous,
			Dictionary<string, int> candidate)
		{
			foreach (string transformation in previous.Keys.Union(candidate.Keys).OrderBy(id => id))
			{
				previous.TryGetValue(transformation, out int oldCount);
				candidate.TryGetValue(transformation, out int newCount);
				if (oldCount != newCount)
					yield return $"transformation {transformation}: match count changed ({oldCount} -> {newCount})";
			}
		}

		private static string BuildRefreshReport(
			CachedSourceBundle candidate,
			PokemonProfileGenerator.ProfileBundle candidateFinal,
			PokemonProfileGenerator.ProfileBundle current,
			List<string> destructiveChanges)
		{
			StringBuilder report = new StringBuilder();
			report.AppendLine("# Pokémon Profile Refresh Report");
			report.AppendLine();
			report.AppendLine($"- Refreshed: {candidate.RefreshedAtUtc}");
			report.AppendLine($"- Source aggregate SHA-256: `{candidate.SourceAggregateSha256}`");
			report.AppendLine($"- Source profiles: {candidate.Bundle.Profiles.Count}");
			report.AppendLine($"- Final profiles: {candidateFinal.Profiles.Count}");
			report.AppendLine($"- Final competitive sets: {candidateFinal.Profiles.Sum(profile => profile.CompetitiveSets.Count)}");
			report.AppendLine($"- Redirects: {candidateFinal.RedirectedSpecies.Count}");
			report.AppendLine($"- Z-A Mega resolutions: {candidate.Bundle.ZaMegaResolutions.Count}");
			report.AppendLine($"- Destructive changes: {destructiveChanges.Count}");
			if (destructiveChanges.Any())
			{
				report.AppendLine();
				report.AppendLine("## Destructive changes");
				foreach (string change in destructiveChanges)
					report.AppendLine($"- {change}");
			}

			report.AppendLine();
			report.AppendLine("## Per-species changes");
			if (current == null)
			{
				report.AppendLine("- No previous local cache was available for comparison.");
			}
			else
			{
				List<string> profileChanges = DescribeProfileChanges(current, candidateFinal).ToList();
				if (profileChanges.Any())
				{
					foreach (string change in profileChanges)
						report.AppendLine(change);
				}
				else
				{
					report.AppendLine("- No generated-profile changes.");
				}
			}

			report.AppendLine();
			report.AppendLine("## Divergence transformation matches");
			foreach (var match in candidateFinal.TransformationMatches.OrderBy(entry => entry.Key))
				report.AppendLine($"- `{match.Key}`: {match.Value}");

			report.AppendLine();
			report.AppendLine("## Z-A Mega resolutions");
			foreach (var resolution in candidate.Bundle.ZaMegaResolutions.OrderBy(entry => entry.Key))
				report.AppendLine($"- {resolution.Key} -> {resolution.Value}");
			return report.ToString();
		}

		private static IEnumerable<string> DescribeProfileChanges(
			PokemonProfileGenerator.ProfileBundle before,
			PokemonProfileGenerator.ProfileBundle after)
		{
			Dictionary<string, PokemonProfileGenerator.PokemonProfile> oldProfiles =
				before.Profiles.ToDictionary(profile => profile.Species);
			Dictionary<string, PokemonProfileGenerator.PokemonProfile> newProfiles =
				after.Profiles.ToDictionary(profile => profile.Species);
			foreach (string species in oldProfiles.Keys.Union(newProfiles.Keys).OrderBy(species => species))
			{
				if (!oldProfiles.TryGetValue(species, out PokemonProfileGenerator.PokemonProfile oldProfile))
				{
					yield return $"- **{species}**: profile added.";
					continue;
				}
				if (!newProfiles.TryGetValue(species, out PokemonProfileGenerator.PokemonProfile newProfile))
				{
					yield return $"- **{species}**: profile removed.";
					continue;
				}

				List<string> changes = new List<string>();
				Dictionary<string, int> oldLevels =
					oldProfile.LevelUpMoves.ToDictionary(move => move.Move, move => move.Level);
				Dictionary<string, int> newLevels =
					newProfile.LevelUpMoves.ToDictionary(move => move.Move, move => move.Level);
				foreach (string move in newLevels.Keys.Except(oldLevels.Keys).OrderBy(move => move))
					changes.Add($"level-up +{move}@{newLevels[move]}");
				foreach (string move in oldLevels.Keys.Except(newLevels.Keys).OrderBy(move => move))
					changes.Add($"level-up -{move}@{oldLevels[move]}");
				foreach (string move in oldLevels.Keys.Intersect(newLevels.Keys)
					.Where(move => oldLevels[move] != newLevels[move]).OrderBy(move => move))
					changes.Add($"level-up {move} {oldLevels[move]}->{newLevels[move]}");
				foreach (string move in newProfile.TutorMoves.Except(oldProfile.TutorMoves).OrderBy(move => move))
					changes.Add($"tutor +{move}");
				foreach (string move in oldProfile.TutorMoves.Except(newProfile.TutorMoves).OrderBy(move => move))
					changes.Add($"tutor -{move}");

				List<PokemonProfileGenerator.PokemonCompetitiveSet> addedSets =
					newProfile.CompetitiveSets.Where(
						set => !oldProfile.CompetitiveSets.Any(
							oldSet => SetFingerprint(oldSet) == SetFingerprint(set))).ToList();
				List<PokemonProfileGenerator.PokemonCompetitiveSet> removedSets =
					oldProfile.CompetitiveSets.Where(
						set => !newProfile.CompetitiveSets.Any(
							newSet => SetFingerprint(newSet) == SetFingerprint(set))).ToList();
				if (addedSets.Any())
					changes.Add($"sets +{addedSets.Count} ({string.Join(", ", addedSets.Select(SetDisplayId))})");
				if (removedSets.Any())
					changes.Add($"sets -{removedSets.Count} ({string.Join(", ", removedSets.Select(SetDisplayId))})");
				if (changes.Any())
					yield return $"- **{species}**: {string.Join("; ", changes)}.";
			}

			foreach (string species in before.RedirectedSpecies.Keys
				.Union(after.RedirectedSpecies.Keys).OrderBy(species => species))
			{
				before.RedirectedSpecies.TryGetValue(species, out string oldTarget);
				after.RedirectedSpecies.TryGetValue(species, out string newTarget);
				if (oldTarget != newTarget)
					yield return $"- **{species}**: redirect `{oldTarget ?? "none"}` -> `{newTarget ?? "none"}`.";
			}
		}

		private static string SetFingerprint(
			PokemonProfileGenerator.PokemonCompetitiveSet set)
		{
			return string.Join(
				"|",
				new[]
				{
					set.Ability ?? "",
					set.Item ?? "",
					set.Nature ?? "",
					set.HiddenPower ?? "",
					set.TeraType ?? "",
					NormalizeRogueRole(set.RogueRole),
					string.Join(",", set.Moves),
					string.Join(",", set.SourceTiers.Distinct().OrderBy(tier => tier)),
				});
		}

		private static string SetDisplayId(
			PokemonProfileGenerator.PokemonCompetitiveSet set)
		{
			if (!string.IsNullOrEmpty(set.StableId))
				return set.StableId;
			if (set.SourceIds.Any())
				return set.SourceIds.OrderBy(id => id).First();
			return "fingerprint-" + ShortHash(SetFingerprint(set));
		}

		private static string NormalizeRogueRole(string role)
		{
			return string.IsNullOrEmpty(role) || role == "standard" ? "" : role;
		}

		private static T Clone<T>(T value)
		{
			return JsonConvert.DeserializeObject<T>(
				JsonConvert.SerializeObject(value, s_JsonSettings),
				s_JsonSettings);
		}

		private static string ShortHash(string value)
		{
			using (SHA256 sha256 = SHA256.Create())
			{
				return string.Concat(
					sha256.ComputeHash(Encoding.UTF8.GetBytes(value))
						.Take(6)
						.Select(valueByte => valueByte.ToString("x2")));
			}
		}

		private static string ComputeFileSha256(string path)
		{
			using (SHA256 sha256 = SHA256.Create())
			using (FileStream stream = File.OpenRead(path))
				return string.Concat(sha256.ComputeHash(stream).Select(value => value.ToString("x2")));
		}

		private static void ExportBundleAtomic(
			PokemonProfileGenerator.ProfileBundle bundle,
			string outputPath)
		{
			string temporaryPath = outputPath + ".tmp";
			PokemonProfileGenerator.ExportProfileBundle(bundle, temporaryPath);
			ReplaceAtomically(temporaryPath, outputPath);
		}

		private static void WriteJsonAtomic(
			string path,
			object value,
			Formatting formatting)
		{
			Directory.CreateDirectory(Path.GetDirectoryName(path));
			string temporaryPath = path + ".tmp";
			JsonSerializerSettings settings = new JsonSerializerSettings
			{
				Formatting = formatting,
				NullValueHandling = NullValueHandling.Ignore,
			};
			File.WriteAllText(
				temporaryPath,
				JsonConvert.SerializeObject(value, settings) + Environment.NewLine);
			ReplaceAtomically(temporaryPath, path);
		}

		private static void ReplaceAtomically(string temporaryPath, string destinationPath)
		{
			Directory.CreateDirectory(Path.GetDirectoryName(destinationPath));
			if (File.Exists(destinationPath))
			{
				string backupPath = destinationPath + ".bak";
				if (File.Exists(backupPath))
					File.Delete(backupPath);
				File.Replace(temporaryPath, destinationPath, backupPath);
				File.Delete(backupPath);
			}
			else
			{
				File.Move(temporaryPath, destinationPath);
			}
		}
	}
}
