using Newtonsoft.Json;
using Newtonsoft.Json.Converters;
using Newtonsoft.Json.Linq;
using PokemonDataGenerator.Utils;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Pokedex
{
	public static class PokemonProfileGenerator
	{
		private const int ProfileCacheSchemaVersion = 2;
		private static readonly Dictionary<string, int> s_TransformationMatchCounts = new Dictionary<string, int>();

		private static void RecordTransformationMatch(string id, int count = 1)
		{
			if (!s_TransformationMatchCounts.ContainsKey(id))
				s_TransformationMatchCounts[id] = 0;
			s_TransformationMatchCounts[id] += count;
		}

		private static readonly JsonSerializerSettings c_JsonSettings = new JsonSerializerSettings
		{
			Formatting = Formatting.Indented,
			Converters = new List<JsonConverter>(new[] 
			{
				new StringEnumConverter()
			})
		};

		private class MovesetSettings
		{
			private static MovesetSettings s_VanillaSettings;
			private static MovesetSettings s_ExSettings;

			public List<string> levelUpPreference = new List<string>();
			public Dictionary<string, string[]> allowedTutorMoves = new Dictionary<string, string[]>();

			public static MovesetSettings VanillaSettings
			{
				get
				{
					if(s_VanillaSettings == null)
					{
						s_VanillaSettings = new MovesetSettings();

						// Priority order list of level up moves
						s_VanillaSettings.levelUpPreference.Add("rogue");
						s_VanillaSettings.levelUpPreference.Add("emerald");
						s_VanillaSettings.levelUpPreference.Add("firered-leafgreen");

						// Orderless versions we will base tutor moves off
						s_VanillaSettings.allowedTutorMoves.Add("rogue", new[] { "rogue", "emerald", "firered-leafgreen", "xd", "colosseum" });
						s_VanillaSettings.allowedTutorMoves.Add("emerald", new[] { "rogue", "emerald", "firered-leafgreen", "xd", "colosseum" });
						s_VanillaSettings.allowedTutorMoves.Add("firered-leafgreen", new[] { "rogue", "emerald", "firered-leafgreen", "xd", "colosseum" });
					}
					return s_VanillaSettings;
				}
			}

			public static MovesetSettings ExSettings
			{
				get
				{
					if (s_ExSettings == null)
					{
						s_ExSettings = new MovesetSettings();

						// Priority order list of level up moves
						s_ExSettings.levelUpPreference.Add("rogue");
						s_ExSettings.levelUpPreference.Add("scarlet-violet");
						s_ExSettings.levelUpPreference.Add("sword-shield");
						s_ExSettings.levelUpPreference.Add("ultra-sun-ultra-moon");

						// Orderless versions we will base tutor moves off
						s_ExSettings.allowedTutorMoves.Add("rogue", new[] { "rogue", "scarlet-violet", "sword-shield", "ultra-sun-ultra-moon" });
						s_ExSettings.allowedTutorMoves.Add("scarlet-violet", new[] { "rogue", "scarlet-violet" });
						s_ExSettings.allowedTutorMoves.Add("sword-shield", new[] { "rogue", "sword-shield" });
						s_ExSettings.allowedTutorMoves.Add("ultra-sun-ultra-moon", new[] { "rogue", "ultra-sun-ultra-moon" });

						// Just needed for cosplay pikachu
						s_ExSettings.levelUpPreference.Add("omega-ruby-alpha-sapphire");
						s_ExSettings.allowedTutorMoves.Add("omega-ruby-alpha-sapphire", new[] { "rogue", "omega-ruby-alpha-sapphire" });
					}
					return s_ExSettings;
				}
			}

			public void RemoveInvalidMoves(SourcePokemonProfile source)
			{
				// Find out which game we're going to base the level up moveset off
				int levelUpIndex = int.MaxValue;

				foreach(var move in source.Moves)
				{
					int versionIndex = levelUpPreference.IndexOf(move.versionName);
					if (versionIndex != -1 && versionIndex < levelUpIndex)
					{
						levelUpIndex = versionIndex;

						// Found ideal preference
						if (levelUpIndex == 0)
							break;
					}
				}

				if (levelUpIndex >= levelUpPreference.Count)
					throw new NotImplementedException("Unable to find valid level up moveset version");

				string moveGroupName = levelUpPreference[levelUpIndex];

				source.Moves.RemoveAll((move) =>
				{
					if (move.originMethod == SourceMoveInfo.LearnMethod.LevelUp)
					{
						return move.versionName != moveGroupName;
					}
					else
					{
						// Treat all other moves as tutor moves
						return !allowedTutorMoves[moveGroupName].Contains(move.versionName);
					}

				});

				// Carry forward toxic & scald nerf from gen8
				if(!GameDataHelpers.IsVanillaVersion)
				{
					if (moveGroupName != "ultra-sun-ultra-moon")
					{
						source.Moves.RemoveAll((move) =>
						{
							if (move.moveName == "toxic")
							{
								if (move.versionName == "scarlet-violet" || move.versionName == "sword-shield")
								{
									return false;
								}

								// Don't accept moveset from before gen8
								return true;
							}

							return false;
						});

						source.Moves.RemoveAll((move) =>
						{
							if (move.moveName == "scald")
							{
								if (move.versionName == "scarlet-violet" || move.versionName == "sword-shield")
								{
									return false;
								}

								// Don't accept moveset from before gen8
								return true;
							}

							return false;
						});
					}
					else
					{
						if (source.Moves.Where((move) => move.moveName == "toxic").Any())
						{
							if(source.Types.Where(str => str == "poison").Any())
							{
								// allow toxic
							}
							else
							{
								// remove toxic here (assuming this is what GF will eventually do?)
								source.Moves.RemoveAll((move) =>
								{
									if (move.moveName == "toxic")
									{
										return true;
									}

									return false;
								});
							}
						}

						if (source.Moves.Where((move) => move.moveName == "scald").Any())
						{
							if (source.Types.Where(str => str == "fire").Any())
							{
								// allow scald
							}
							else
							{
								// remove scald here (assuming this is what GF will eventually do?)
								source.Moves.RemoveAll((move) =>
								{
									if (move.moveName == "scald")
									{
										return true;
									}

									return false;
								});
							}
						}
					}
				}
			}
		}


		internal class SourceMoveInfo
		{
			public enum LearnMethod
			{
				Invalid,
				Egg,
				TM,
				LevelUp,
				Tutor,
			}

			public LearnMethod originMethod;
			public int learnLevel;
			public string moveName;
			public string versionName;

			public override string ToString()
			{
				if(originMethod == LearnMethod.LevelUp)
					return originMethod.ToString() + "@" + learnLevel + " : " + moveName + " (" + versionName + ")";
				else
					return originMethod.ToString() + " : " + moveName + " (" + versionName + ")";
			}
		}

		internal class SourcePokemonProfile
		{
			public string Species;
			public List<SourceMoveInfo> Moves;
			public List<PokemonCompetitiveSet> CompetitiveSets;
			public string[] Types = new string[2];
			public string[] Abilities = new string[3];

			public SourcePokemonProfile(string name)
			{
				Species = name;
				Moves = new List<SourceMoveInfo>();
				CompetitiveSets = new List<PokemonCompetitiveSet>();

				for (int i = 0; i < Types.Length; ++i)
					Types[i] = "none";

				for (int i = 0; i < Abilities.Length; ++i)
					Abilities[i] = "none";
			}

			public void CollapseMovesets()
			{
				//Moves.RemoveAll((m) => PokemonMoveHelpers.IsMoveUnsupported(m.moveName));

				var movesetPreferences = (GameDataHelpers.IsVanillaVersion ? MovesetSettings.VanillaSettings : MovesetSettings.ExSettings);
				movesetPreferences.RemoveInvalidMoves(this);

				// Simplify the definitions now and then we'll remove any duplicates
				List<SourceMoveInfo> newMoves = new List<SourceMoveInfo>();

				foreach(var oldMove in Moves)
				{
					SourceMoveInfo newMove = oldMove;
					newMove.moveName = FormatMoveName(newMove.moveName, true);
					newMove.versionName = "rogue";

					if (IsBannedMove(newMove.moveName))
						continue;

					if (!GameDataHelpers.MoveDefines.ContainsKey(newMove.moveName))
						throw new InvalidDataException();

					if (newMove.originMethod == SourceMoveInfo.LearnMethod.LevelUp)
					{
						var existingLevelUpMove = newMoves.Where(m => m.moveName == newMove.moveName && m.originMethod == SourceMoveInfo.LearnMethod.LevelUp).FirstOrDefault();

						// If we already have the same level up move only take it at the lowest learn level
						if (existingLevelUpMove != null)
						{
							existingLevelUpMove.learnLevel = Math.Min(existingLevelUpMove.learnLevel, newMove.learnLevel);
							continue;
						}
					}
					else
					{
						newMove.originMethod = SourceMoveInfo.LearnMethod.Tutor;

						//var anyLevelUpMove = Moves.Where(m => m.moveName == newMove.moveName && m.originMethod == SourceMoveInfo.LearnMethod.LevelUp).FirstOrDefault();
						//if (anyLevelUpMove != null)
						//{
						//	// If we learn this as a level up move, don't include an entry as a tutor move
						//	continue;
						//}
					}

					if (!newMoves.Contains(newMove))
						newMoves.Add(newMove);
				}

				Moves = newMoves;
			}
		}

		internal class LevelUpMove
		{
			public string Move;
			public int Level;

			public override string ToString()
			{
				return Move + " @ Lv" + Level;
			}
		}

		private class LearnsetRule
		{
			public string Id { get; set; }
			public string Species { get; set; }
			public string Move { get; set; }
			public int Level { get; set; }
		}

		private class LearnsetRuleFile
		{
			public int SchemaVersion { get; set; }
			public List<LearnsetRule> CompatibilityTutor { get; set; } = new List<LearnsetRule>();
			public List<LearnsetRule> LevelUp { get; set; } = new List<LearnsetRule>();
			public List<LearnsetRule> Tutor { get; set; } = new List<LearnsetRule>();
		}

		internal class PokemonProfile
		{
			private static LearnsetRuleFile s_LearnsetRules;

			private static LearnsetRuleFile LearnsetRules
			{
				get
				{
					if (s_LearnsetRules == null)
					{
						string path = Path.Combine(
							GameDataHelpers.RootDirectory,
							"tools", "Pokabbie", "PokemonDataGenerator", "PokemonDataGenerator",
							"Resources", "PokemonProfiles", "Pipeline", "divergence_learnsets.json");
						s_LearnsetRules = JsonConvert.DeserializeObject<LearnsetRuleFile>(File.ReadAllText(path));
						if (s_LearnsetRules.SchemaVersion != 1)
							throw new InvalidDataException($"Unsupported Divergence learnset-rule schema {s_LearnsetRules.SchemaVersion}");
						List<LearnsetRule> allRules = s_LearnsetRules.CompatibilityTutor
							.Concat(s_LearnsetRules.LevelUp)
							.Concat(s_LearnsetRules.Tutor)
							.ToList();
						if (allRules.GroupBy(rule => rule.Id).Any(group => group.Count() != 1))
							throw new InvalidDataException("Divergence learnset-rule IDs must be unique.");
						foreach (LearnsetRule rule in allRules)
						{
							if (!GameDataHelpers.SpeciesDefines.ContainsKey(rule.Species)
								|| !GameDataHelpers.MoveDefines.ContainsKey(rule.Move))
								throw new InvalidDataException($"Invalid Divergence learnset rule {rule.Id}.");
						}
					}
					return s_LearnsetRules;
				}
			}

			// Divergence learnset additions and legacy compatibility are tracked in divergence_learnsets.json.
			public string Species;
			public List<LevelUpMove> LevelUpMoves;
			public List<string> TutorMoves;
			public List<PokemonCompetitiveSet> CompetitiveSets;

			internal static PokemonProfile FromSource(SourcePokemonProfile sourceProfile)
			{
				PokemonProfile profile = new PokemonProfile();
				profile.Species = sourceProfile.Species;
				profile.CompetitiveSets = new List<PokemonCompetitiveSet>(sourceProfile.CompetitiveSets);
				profile.LevelUpMoves = new List<LevelUpMove>();
				profile.TutorMoves = new List<string>();

				foreach (var move in sourceProfile.Moves)
				{
					if (move.originMethod == SourceMoveInfo.LearnMethod.LevelUp)
					{
						var existingLevelUpMove = profile.LevelUpMoves.Where(m => m.Move == move.moveName).FirstOrDefault();

						// Take the lowest level if we already had an entry for this
						if (existingLevelUpMove != null)
							existingLevelUpMove.Level = Math.Min(existingLevelUpMove.Level, move.learnLevel);
						else
							profile.LevelUpMoves.Add(new LevelUpMove { Move = move.moveName, Level = move.learnLevel });
					}
					else
					{
						if(!profile.TutorMoves.Contains(move.moveName))
							profile.TutorMoves.Add(move.moveName);
					}
				}

				return profile;
			}

			public bool HasLevelUpMove(string move)
			{
				return LevelUpMoves.Where(m => m.Move == move).Any();
			}

			public bool HasTutorMove(string move)
			{
				return TutorMoves.Where(m => m == move).Any();
			}

			public bool CanLearnMove(string move)
			{
				return HasLevelUpMove(move) || HasTutorMove(move);
			}

			private void EraseMove(string move)
			{
				LevelUpMoves.RemoveAll(m => m.Move == move);
				TutorMoves.RemoveAll(m => m == move);
			}

			private void ReplaceMove(string fromMove, string toMove, int levelOverride = -1)
			{
				for(int i = 0; i < LevelUpMoves.Count; ++i)
				{
					if (LevelUpMoves[i].Move == fromMove)
					{
						LevelUpMoves[i].Move = toMove;
						if(levelOverride >= 0)
							LevelUpMoves[i].Level = levelOverride;
					}
				}

				for (int i = 0; i < TutorMoves.Count; ++i)
				{
					if (TutorMoves[i] == fromMove)
						TutorMoves[i] = toMove;
				}
			}

			private void ReplaceCompetitiveAbility(string fromAbility, string toAbility)
			{
				int matchCount = 0;
				foreach (var set in CompetitiveSets)
				{
					if (set.Ability == fromAbility)
					{
						set.Ability = toAbility;
						++matchCount;
					}
				}
				RecordTransformationMatch(
					$"{Species.ToLowerInvariant()}-ability-{fromAbility.ToLowerInvariant()}-to-{toAbility.ToLowerInvariant()}",
					matchCount);
			}

			private PokemonCompetitiveSet SelectCompetitiveSet(
				string transformationId,
				Func<PokemonCompetitiveSet, bool> predicate)
			{
				List<PokemonCompetitiveSet> matches = CompetitiveSets.Where(predicate).ToList();
				if (matches.Count != 1)
				{
					throw new InvalidDataException(
						$"{transformationId} expected exactly one competitive set for {Species}, found {matches.Count}");
				}
				RecordTransformationMatch(transformationId);
				return matches[0];
			}

			private static bool HasSourceId(PokemonCompetitiveSet set, string sourceId)
			{
				return set.SourceIds.Contains(sourceId);
			}

			private void UpdatePolitoedCompetitiveSets()
			{
				foreach (var set in CompetitiveSets)
				{
					if (set.Ability != "ABILITY_DRIZZLE" && set.Ability != "ABILITY_LIQUID_VOICE")
						continue;

					set.Ability = "ABILITY_LIQUID_VOICE";

					if (set.Moves.Contains("MOVE_HYPER_VOICE"))
						continue;

					string moveToReplace = new[]
					{
						"MOVE_SURF",
						"MOVE_HYDRO_PUMP",
						"MOVE_MUDDY_WATER",
						"MOVE_WATER_PULSE",
						"MOVE_WHIRLPOOL",
					}.FirstOrDefault(move => set.Moves.Contains(move));

					if (moveToReplace == null)
						throw new InvalidDataException("Politoed's Liquid Voice set has no Water-type move to replace with Hyper Voice");

					set.Moves[set.Moves.IndexOf(moveToReplace)] = "MOVE_HYPER_VOICE";
				}
			}

			private static bool IsDoublesCompetitiveTier(string tier)
			{
				return tier.Contains("DOUBLES") || tier.Contains("VGC");
			}

			private void UpdateIndeedeeCompetitiveAbilities()
			{
				foreach (var set in CompetitiveSets)
				{
					if (set.Ability != "ABILITY_PSYCHIC_SURGE" && set.Ability != "ABILITY_TELEPATHY")
						continue;

					set.Ability = set.SourceTiers.Any() && set.SourceTiers.All(IsDoublesCompetitiveTier)
						? "ABILITY_TELEPATHY"
						: "ABILITY_SYNCHRONIZE";
				}
			}

			private void UpdateUnownCompetitiveSets()
			{
				List<string> sourceTiers = CompetitiveSets
					.SelectMany(set => set.SourceTiers)
					.Distinct()
					.ToList();

				if (!sourceTiers.Any())
				{
					sourceTiers.Add("GEN6PU");
					sourceTiers.Add("GEN7PU");
				}

				CompetitiveSets.Clear();
				CompetitiveSets.Add(new PokemonCompetitiveSet
				{
					StableId = "unown-divergence-choice-specs",
					Provenance = "divergence-redesign",
					RogueRole = "standard",
					Ability = "ABILITY_LEVITATE",
					Item = "ITEM_CHOICE_SPECS",
					Nature = "NATURE_MODEST",
					HiddenPower = "TYPE_FIGHTING",
					TeraType = "TYPE_PSYCHIC",
					Moves = new List<string>
					{
						"MOVE_HIDDEN_POWER",
						"MOVE_EARTH_POWER",
						"MOVE_POWER_GEM",
						"MOVE_TERA_BLAST",
					},
					SourceTiers = new List<string>(sourceTiers),
				});
				CompetitiveSets.Add(new PokemonCompetitiveSet
				{
					StableId = "unown-divergence-cosmic-power",
					Provenance = "divergence-redesign",
					RogueRole = "strong-wild",
					Ability = "ABILITY_LEVITATE",
					Item = "ITEM_LEFTOVERS",
					Nature = "NATURE_TIMID",
					TeraType = "TYPE_PSYCHIC",
					Moves = new List<string>
					{
						"MOVE_COSMIC_POWER",
						"MOVE_STORED_POWER",
						"MOVE_EARTH_POWER",
						"MOVE_POWER_GEM",
					},
					SourceTiers = new List<string>(sourceTiers),
				});
				RecordTransformationMatch("unown-full-profile-redesign");
			}

			private void UpdateLedianCompetitiveSets()
			{
				PokemonCompetitiveSet utilitySet = SelectCompetitiveSet(
					"ledian-battle-formation-utility",
					set => HasSourceId(set, "gen6|gen6pu|Ledian|Dual Screens"));
				utilitySet.Item = "ITEM_HEAVY_DUTY_BOOTS";
				utilitySet.Nature = "NATURE_JOLLY";
				utilitySet.Moves = new List<string>
				{
					"MOVE_ROOST",
					"MOVE_U_TURN",
					"MOVE_DRAIN_PUNCH",
					"MOVE_KNOCK_OFF",
				};

				PokemonCompetitiveSet ironFistSet = SelectCompetitiveSet(
					"ledian-battle-formation-iron-fist",
					set => HasSourceId(set, "gen7|gen7pu|Ledian|Falcon PUNCH (All-out Attacker)"));
				ironFistSet.Moves = new List<string>
				{
					"MOVE_ROOST",
					"MOVE_POWER_UP_PUNCH",
					"MOVE_DRAIN_PUNCH",
					"MOVE_ICE_PUNCH",
				};
			}

			private void UpdateHuntailCompetitiveSets()
			{
				PokemonCompetitiveSet mixedShellSmash = SelectCompetitiveSet(
					"huntail-abyssal-maw-mixed-shell-smash",
					set => HasSourceId(set, "gen6|gen6pu|Huntail|Shell Smash"));
				mixedShellSmash.Nature = "NATURE_ADAMANT";
				mixedShellSmash.Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
				};

				PokemonCompetitiveSet physicalShellSmash = SelectCompetitiveSet(
					"huntail-abyssal-maw-coil",
					set => HasSourceId(set, "gen6|gen6pu|Huntail|Showdown Usage"));
				physicalShellSmash.Item = "ITEM_LEFTOVERS";
				physicalShellSmash.Nature = "NATURE_ADAMANT";
				physicalShellSmash.Moves = new List<string>
				{
					"MOVE_COIL",
					"MOVE_AQUA_TAIL",
					"MOVE_CRUNCH",
					"MOVE_DRAGON_TAIL",
				};

				PokemonCompetitiveSet gen7WaterVeil = SelectCompetitiveSet(
					"huntail-abyssal-maw-gen7-water-veil",
					set => HasSourceId(set, "gen7|gen7zu|Huntail|Mixed Shell Smash"));
				gen7WaterVeil.Nature = "NATURE_ADAMANT";
				gen7WaterVeil.Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
				};

				PokemonCompetitiveSet gen7SwiftSwim = SelectCompetitiveSet(
					"huntail-abyssal-maw-gen7-swift-swim",
					set => HasSourceId(set, "gen7|gen7zu|Huntail|Showdown Usage"));
				gen7SwiftSwim.Nature = "NATURE_ADAMANT";
				gen7SwiftSwim.Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
				};
			}

			private void UpdateGorebyssCompetitiveSets()
			{
				PokemonCompetitiveSet gen6NuSet = SelectCompetitiveSet(
					"gorebyss-shell-game-gen6nu",
					set => HasSourceId(set, "gen6|gen6nu|Gorebyss|Shell Smash"));
				gen6NuSet.HiddenPower = null;
				gen6NuSet.Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_HYDRO_PUMP",
					"MOVE_ALLURING_VOICE",
					"MOVE_ICE_BEAM",
				};

				PokemonCompetitiveSet gen6PuSet = SelectCompetitiveSet(
					"gorebyss-shell-game-gen6pu-terrain",
					set => HasSourceId(set, "gen6|gen6pu|Gorebyss|Shell Smash"));
				gen6PuSet.Moves = new List<string>
				{
					"MOVE_MISTY_TERRAIN",
					"MOVE_SHELL_SMASH",
					"MOVE_HYDRO_PUMP",
					"MOVE_ALLURING_VOICE",
				};

				PokemonCompetitiveSet alternateGen6PuSet = SelectCompetitiveSet(
					"gorebyss-shell-game-gen6pu-draining-kiss",
					set => HasSourceId(set, "gen6|gen6pu|Gorebyss|Showdown Usage"));
				alternateGen6PuSet.Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_SURF",
					"MOVE_DRAINING_KISS",
					"MOVE_PSYCHIC",
				};

				PokemonCompetitiveSet gen7PuSet = SelectCompetitiveSet(
					"gorebyss-shell-game-gen7pu-terrain",
					set => HasSourceId(set, "gen7|gen7pu|Gorebyss|Shell Smash"));
				gen7PuSet.HiddenPower = null;
				gen7PuSet.Moves = new List<string>
				{
					"MOVE_MISTY_TERRAIN",
					"MOVE_SHELL_SMASH",
					"MOVE_HYDRO_PUMP",
					"MOVE_ALLURING_VOICE",
				};
			}

			private void UpdateParasectCompetitiveSets()
			{
				PokemonCompetitiveSet defensiveSet = SelectCompetitiveSet(
					"parasect-fungal-infection-poltergeist",
					set => HasSourceId(set, "gen6|gen6pu|Parasect|Spore"));
				defensiveSet.Moves = new List<string>
				{
					"MOVE_SPORE",
					"MOVE_SYNTHESIS",
					"MOVE_SEED_BOMB",
					"MOVE_POLTERGEIST",
				};

				PokemonCompetitiveSet physicalSet = SelectCompetitiveSet(
					"parasect-fungal-infection-shadow-claw",
					set => HasSourceId(set, "gen7|gen7pu|Parasect|Specially Defensive"));
				physicalSet.Moves = new List<string>
				{
					"MOVE_SWORDS_DANCE",
					"MOVE_SHADOW_CLAW",
					"MOVE_SEED_BOMB",
					"MOVE_SYNTHESIS",
				};
			}

			private void UpdateVolbeatCompetitiveSets()
			{
				PokemonCompetitiveSet tailGlowSet = SelectCompetitiveSet(
					"volbeat-electric-tail-glow",
					set => HasSourceId(set, "gen6|gen6pu|Volbeat|Showdown Usage"));
				tailGlowSet.Item = "ITEM_LIFE_ORB";
				tailGlowSet.Ability = "ABILITY_SWARM";
				tailGlowSet.Nature = "NATURE_TIMID";
				tailGlowSet.Moves = new List<string>
				{
					"MOVE_TAIL_GLOW",
					"MOVE_THUNDER",
					"MOVE_BUG_BUZZ",
					"MOVE_ROOST",
				};

				PokemonCompetitiveSet defogSet = SelectCompetitiveSet(
					"volbeat-remove-dead-damp-rock",
					set => HasSourceId(set, "gen7|gen7pu|Volbeat|Showdown Usage"));
				defogSet.Item = "ITEM_HEAVY_DUTY_BOOTS";
			}

			private void UpdateFlorgesCompetitiveSets()
			{
				PokemonCompetitiveSet gen6CalmMindSet = SelectCompetitiveSet(
					"florges-flower-rite-gen6-grass-stab",
					set => HasSourceId(set, "gen6|gen6uu|Florges|Defensive Calm Mind"));
				gen6CalmMindSet.Moves = new List<string>
				{
					"MOVE_CALM_MIND",
					"MOVE_MOONBLAST",
					"MOVE_GIGA_DRAIN",
					"MOVE_SYNTHESIS",
				};

				PokemonCompetitiveSet gen9CalmMindSet = SelectCompetitiveSet(
					"florges-flower-rite-gen9-grass-stab",
					set => HasSourceId(set, "gen9|gen9pu|Florges|Calm Mind"));
				gen9CalmMindSet.Moves = new List<string>
				{
					"MOVE_CALM_MIND",
					"MOVE_MOONBLAST",
					"MOVE_ENERGY_BALL",
					"MOVE_SYNTHESIS",
				};
			}

			private void UpdateElectivireCompetitiveSets()
			{
				PokemonCompetitiveSet dynamoFistsSet = SelectCompetitiveSet(
					"electivire-dynamo-fists",
					set => HasSourceId(set, "gen6|gen6nu|Electivire|Showdown Usage"));
				dynamoFistsSet.Moves = new List<string>
				{
					"MOVE_ELECTRIC_TERRAIN",
					"MOVE_PLASMA_FISTS",
					"MOVE_MEGA_PUNCH",
					"MOVE_ICE_PUNCH",
				};
			}

			private void UpdateSunfloraCompetitiveSets()
			{
				PokemonCompetitiveSet chlorophyllSet = CompetitiveSets
					.FirstOrDefault(set => set.Ability == "ABILITY_EARLY_BIRD");

				if (chlorophyllSet == null)
					throw new InvalidDataException("Sunflora is missing its Early Bird competitive set");

				chlorophyllSet.Ability = "ABILITY_CHLOROPHYLL";
				chlorophyllSet.Item = "ITEM_LIFE_ORB";
				chlorophyllSet.Nature = "NATURE_TIMID";
				chlorophyllSet.Moves = new List<string>
				{
					"MOVE_SOLAR_BEAM",
					"MOVE_EARTH_POWER",
					"MOVE_WEATHER_BALL",
					"MOVE_SLUDGE_BOMB",
				};
			}

			private void UpdateDodrioCompetitiveSets()
			{
				foreach (var set in CompetitiveSets)
				{
					if (set.Moves.Contains("MOVE_DRILL_RUN"))
						continue;

					string moveToReplace = new[]
					{
						"MOVE_RETURN",
						"MOVE_DOUBLE_EDGE",
						"MOVE_THRASH",
						"MOVE_LOW_KICK",
						"MOVE_QUICK_ATTACK",
						"MOVE_JUMP_KICK",
					}.FirstOrDefault(move => set.Moves.Contains(move));

					if (moveToReplace == null)
						throw new InvalidDataException("Dodrio competitive set has no move to replace with Ground STAB");

					set.Moves[set.Moves.IndexOf(moveToReplace)] = "MOVE_DRILL_RUN";
				}
			}

			private void UpdateNoctowlCompetitiveSets()
			{
				foreach (var set in CompetitiveSets)
				{
					if (set.Item == "ITEM_CHOICE_SPECS" && set.Moves.Contains("MOVE_HYPER_VOICE"))
						set.Moves[set.Moves.IndexOf("MOVE_HYPER_VOICE")] = "MOVE_PSYCHIC";
					else if (set.Moves.Contains("MOVE_NASTY_PLOT") && set.Moves.Contains("MOVE_DEFOG"))
						set.Moves[set.Moves.IndexOf("MOVE_DEFOG")] = "MOVE_PSYCHIC";
				}
			}

			private void ApplyDivergenceAbilityReplacements()
			{
				switch (Species)
				{
					case "SPECIES_UNOWN":
						UpdateUnownCompetitiveSets();
						break;
					case "SPECIES_LEDIAN":
						UpdateLedianCompetitiveSets();
						break;
					case "SPECIES_PARASECT":
						UpdateParasectCompetitiveSets();
						break;
					case "SPECIES_VOLBEAT":
						UpdateVolbeatCompetitiveSets();
						break;
					case "SPECIES_HUNTAIL":
						UpdateHuntailCompetitiveSets();
						break;
					case "SPECIES_GOREBYSS":
						UpdateGorebyssCompetitiveSets();
						break;
					case "SPECIES_ELECTIVIRE":
						UpdateElectivireCompetitiveSets();
						break;
					case "SPECIES_FLORGES":
						UpdateFlorgesCompetitiveSets();
						break;
					case "SPECIES_SUNFLORA":
						UpdateSunfloraCompetitiveSets();
						break;
					case "SPECIES_DODRIO":
						UpdateDodrioCompetitiveSets();
						break;
					case "SPECIES_NOCTOWL":
						UpdateNoctowlCompetitiveSets();
						break;
					case "SPECIES_GENGAR":
						ReplaceCompetitiveAbility("ABILITY_CURSED_BODY", "ABILITY_LEVITATE");
						break;
					case "SPECIES_POLITOED":
						UpdatePolitoedCompetitiveSets();
						break;
					case "SPECIES_PELIPPER":
						ReplaceCompetitiveAbility("ABILITY_DRIZZLE", "ABILITY_WIND_RIDER");
						break;
					case "SPECIES_TYRANITAR":
						ReplaceCompetitiveAbility("ABILITY_SAND_STREAM", "ABILITY_BATTLE_ARMOR");
						break;
					case "SPECIES_SNOVER":
					case "SPECIES_ABOMASNOW":
						ReplaceCompetitiveAbility("ABILITY_SNOW_WARNING", "ABILITY_GRASS_PELT");
						break;
					case "SPECIES_ROTOM":
						ReplaceCompetitiveAbility("ABILITY_LEVITATE", "ABILITY_CURSED_BODY");
						break;
					case "SPECIES_ROTOM_HEAT":
						ReplaceCompetitiveAbility("ABILITY_LEVITATE", "ABILITY_FLASH_FIRE");
						break;
					case "SPECIES_ROTOM_WASH":
						ReplaceCompetitiveAbility("ABILITY_LEVITATE", "ABILITY_WATER_ABSORB");
						break;
					case "SPECIES_ROTOM_FROST":
						ReplaceCompetitiveAbility("ABILITY_LEVITATE", "ABILITY_SNOW_WARNING");
						break;
					case "SPECIES_ROTOM_FAN":
						ReplaceCompetitiveAbility("ABILITY_LEVITATE", "ABILITY_WIND_POWER");
						break;
					case "SPECIES_ROTOM_MOW":
						ReplaceCompetitiveAbility("ABILITY_LEVITATE", "ABILITY_GRASSY_SURGE");
						break;
					case "SPECIES_VANILLUXE":
						ReplaceCompetitiveAbility("ABILITY_SNOW_WARNING", "ABILITY_SLUSH_RUSH");
						break;
					case "SPECIES_AMAURA":
					case "SPECIES_AURORUS":
						ReplaceCompetitiveAbility("ABILITY_SNOW_WARNING", "ABILITY_SOLID_ROCK");
						break;
					case "SPECIES_TORKOAL":
						ReplaceCompetitiveAbility("ABILITY_WHITE_SMOKE", "ABILITY_FLAME_BODY");
						ReplaceCompetitiveAbility("ABILITY_DROUGHT", "ABILITY_STEAM_ENGINE");
						break;
					case "SPECIES_GIGALITH":
						ReplaceCompetitiveAbility("ABILITY_SAND_STREAM", "ABILITY_STAMINA");
						break;
					case "SPECIES_PINCURCHIN":
						ReplaceCompetitiveAbility("ABILITY_ELECTRIC_SURGE", "ABILITY_INNARDS_OUT");
						break;
					case "SPECIES_INDEEDEE":
					case "SPECIES_INDEEDEE_MALE":
					case "SPECIES_INDEEDEE_FEMALE":
						UpdateIndeedeeCompetitiveAbilities();
						break;
					case "SPECIES_SMOLIV":
					case "SPECIES_DOLLIV":
					case "SPECIES_ARBOLIVA":
						ReplaceCompetitiveAbility("ABILITY_SEED_SOWER", "ABILITY_NATURAL_CURE");
						break;
				}
			}

			private void RestoreImplementedMoveData()
			{
				foreach (LearnsetRule rule in LearnsetRules.CompatibilityTutor.Where(rule => rule.Species == Species))
				{
					if (!CanLearnMove(rule.Move))
						TutorMoves.Add(rule.Move);
				}

				foreach (LearnsetRule rule in LearnsetRules.LevelUp.Where(rule => rule.Species == Species))
				{
					LevelUpMove existingMove = LevelUpMoves.FirstOrDefault(move => move.Move == rule.Move);
					if (existingMove == null)
						LevelUpMoves.Add(new LevelUpMove { Move = rule.Move, Level = rule.Level });
					else
						existingMove.Level = rule.Level;
				}

				foreach (LearnsetRule rule in LearnsetRules.Tutor.Where(rule => rule.Species == Species))
				{
					if (!TutorMoves.Contains(rule.Move))
						TutorMoves.Add(rule.Move);
				}

				if (Species == "SPECIES_WHISCASH")
					EraseMove("MOVE_SWIFT");

				if (Species == "SPECIES_PAWMOT" || Species == "SPECIES_RABSCA")
					// These sets were built to recycle Revival Blessing's PP, which is
					// incompatible with Divergence's once-per-team battle limit.
					CompetitiveSets.RemoveAll(set => set.Item == "ITEM_LEPPA_BERRY");

			}

			private bool AttemptReplaceMove(PokemonCompetitiveSet target, string move, params string[] orderedReplacements)
			{
				foreach(string testMove in orderedReplacements)
				{
					if(!target.Moves.Contains(testMove) && CanLearnMove(testMove))
					{
						int index = target.Moves.IndexOf(move);
						target.Moves[index] = testMove;
						return true;
					}
				}

				return false;
			}

			public void FormatDataForGame()
			{
				if (!GameDataHelpers.IsVanillaVersion)
					RestoreImplementedMoveData();

				// Now we've added the sets, add any moves that we can't currently learn as tutor moves
				foreach (var set in CompetitiveSets)
				{
					foreach (var move in set.Moves.ToArray())
					{
						bool canLearnMove = CanLearnMove(move);
						if (!canLearnMove)
						{
							if (!GameDataHelpers.IsVanillaVersion)
							{
								if (move == "MOVE_TOXIC")
								{
									if (AttemptReplaceMove(set, "MOVE_TOXIC", 
										"MOVE_THUNDER_WAVE", 
										"MOVE_WILL_O_WISP",
										"MOVE_YAWN",
										"MOVE_HYPNOSIS",
										"MOVE_STEALTH_ROCK",
										"MOVE_KNOCK_OFF",
										"MOVE_IRON_HEAD",
										"MOVE_SUBSTITUTE",
										"MOVE_U_TURN",
										"MOVE_ATTRACT",
										"MOVE_METRONOME",
										"MOVE_INFESTATION",
										"MOVE_SAND_ATTACK"
									))
									{
										// Prevent being added to tutor move list
										continue;
									}
									else
									{
										throw new Exception(Species + " no longer supports Toxic!");
									}
								}

								if (move == "MOVE_SCALD")
								{
									if (AttemptReplaceMove(set, "MOVE_SCALD",
										"MOVE_FREEZE_DRY",
										"MOVE_HYDRO_PUMP",
										"MOVE_SURF",
										"MOVE_ICE_BEAM",
										"MOVE_WATER_PULSE"
									))
									{
										// Prevent being added to tutor move list
										continue;
									}
									else
									{
										throw new Exception(Species + " no longer supports Scald!");
									}
								}
							}

							TutorMoves.Add(move);
						}
					}
				}

				// Now we have the complete moves so perform any post-processing
				{
					LevelUpMoves.RemoveAll((move) =>
					{
						return IsBannedMove(move.Move);
					});
					TutorMoves.RemoveAll((move) =>
					{
						return IsBannedMove(move);
					});

					if (!GameDataHelpers.IsVanillaVersion)
					{
						// if we can learn hidden power or tera blast, we actually can have both
						if (CanLearnMove("MOVE_TERA_BLAST") || CanLearnMove("MOVE_HIDDEN_POWER"))
						{
							if (!TutorMoves.Contains("MOVE_TERA_BLAST"))
								TutorMoves.Add("MOVE_TERA_BLAST");

							if (!TutorMoves.Contains("MOVE_HIDDEN_POWER"))
								TutorMoves.Add("MOVE_HIDDEN_POWER");
						}

						// Replace all instances of hail with snowscape
						ReplaceMove("MOVE_HAIL", "MOVE_SNOWSCAPE");

						if (Species == "SPECIES_RAYQUAZA")
						{
							// Make dragon ascent a late level learn
							ReplaceMove("MOVE_DRAGON_ASCENT", "MOVE_DRAGON_ASCENT", 90);
						}

						if (Species == "SPECIES_ETERNATUS")
						{
							// Restore Eternatus's signature move, which was removed from its Gen 9 learnset
							if (!HasLevelUpMove("MOVE_ETERNABEAM"))
								LevelUpMoves.Add(new LevelUpMove { Move = "MOVE_ETERNABEAM", Level = 88 });
						}

						ApplyDivergenceAbilityReplacements();
					}
				}

				// Now sort them before we export
				LevelUpMoves = LevelUpMoves.OrderBy(m => m.Level).ToList();
				TutorMoves = TutorMoves.OrderBy(m => m).ToList();


				if (!GameDataHelpers.IsVanillaVersion)
				{
					// Now apply same move rename/removal to competitive sets
					foreach (var compSet in CompetitiveSets)
					{
						for (int m = 0; m < compSet.Moves.Count; ++m)
						{
							string moveName = compSet.Moves[m];
							if (IsBannedMove(moveName))
							{
								compSet.Moves.RemoveAt(m--);
							}
							else
							{
								switch (moveName)
								{
									case "MOVE_HAIL":
										compSet.Moves[m] = "MOVE_SNOWSCAPE";
										break;
								}
							}

						}
					}
				}
			}

			public void ValidateContents()
			{
				if (!LevelUpMoves.Any())
					throw new InvalidDataException($"'{Species}' missing level up moves");

				// Verify the move is recognised in game here
				foreach (var move in LevelUpMoves.Select(m => m.Move).Union(TutorMoves))
				{
					if (!GameDataHelpers.MoveDefines.ContainsKey(move))
						throw new InvalidDataException($"'{Species}' has unsupported move '{move}'");
				}
			}

		}

		internal class PokemonCompetitiveSet
		{
			public List<string> Moves = new List<string>();
			public string Ability;
			public string Item;
			public string Nature;
			public string HiddenPower;
			public string TeraType;
			public List<string> SourceTiers = new List<string>();
			public List<string> SourceIds = new List<string>();
			public string StableId;
			public string Provenance;
			public string RogueRole;

			public bool IsCompatibleWith(PokemonCompetitiveSet other)
			{
				if (Ability != other.Ability)
					return false;
				if (Item != other.Item)
					return false;
				if (Nature != other.Nature)
					return false;
				if (HiddenPower != other.HiddenPower)
					return false;
				if (TeraType != other.TeraType)
					return false;

				if (Moves.Count != other.Moves.Count)
					return false;

				for(int i = 0; i < Moves.Count; ++i)
				{
					if (Moves[i] != other.Moves[i])
						return false;
				}

				return true;
			}

			public static PokemonCompetitiveSet ParseFrom(string sourceTier, JObject json)
			{
				PokemonCompetitiveSet output = new PokemonCompetitiveSet();
				output.SourceTiers.Add(sourceTier);
				if (json.ContainsKey("_profileSourceId"))
					output.SourceIds.Add(json["_profileSourceId"].Value<string>());
				output.Provenance = "showdown";

				string ability = json["ability"].Value<string>();
				if (ability != "No Ability")
				{
					output.Ability = "ABILITY_" + GameDataHelpers.FormatKeyword(ability);

					switch(output.Ability)
					{
						case "ABILITY_AS_ONE_(GLASTRIER)":
							output.Ability = "ABILITY_AS_ONE_ICE_RIDER";
							break;
						case "ABILITY_AS_ONE_(SPECTRIER)":
							output.Ability = "ABILITY_AS_ONE_SHADOW_RIDER";
							break;
					}

					if (!GameDataHelpers.AbilityDefines.ContainsKey(output.Ability))
						throw new InvalidDataException();
				}

				if (json.ContainsKey("item"))
				{
					output.Item = FormatItemName(json["item"].Value<string>());

					if (!GameDataHelpers.ItemDefines.ContainsKey(output.Item))
						throw new InvalidDataException();
				}

				if (json.ContainsKey("nature"))
				{
					output.Nature = "NATURE_" + GameDataHelpers.FormatKeyword(json["nature"].Value<string>());

					if (!GameDataHelpers.NatureDefines.ContainsKey(output.Nature))
						throw new InvalidDataException();
				}

				if (json.ContainsKey("teraType"))
				{
					output.TeraType = json["teraType"].Value<string>().Trim();
					output.TeraType = "TYPE_" + GameDataHelpers.FormatKeyword(output.TeraType);

					if (!GameDataHelpers.TypesDefines.ContainsKey(output.TeraType))
						throw new InvalidDataException();
				}

				foreach (var move in json["moves"])
				{
					string moveName = move.Value<string>();

					if(moveName.StartsWith("Hidden Power", StringComparison.CurrentCultureIgnoreCase))
					{
						output.HiddenPower = moveName.Substring("Hidden Power".Length).Trim();
						output.HiddenPower = "TYPE_" + GameDataHelpers.FormatKeyword(output.HiddenPower);
						moveName = "Hidden Power";

						if (!GameDataHelpers.TypesDefines.ContainsKey(output.HiddenPower))
							throw new InvalidDataException();
					}

					moveName = FormatMoveName(moveName, false);

					output.Moves.Add(moveName);
				}

				return output;
			}
		}

		private static string FormatMoveName(string moveName, bool fromLearnsets)
		{
			// Fix some spellings
			if (GameDataHelpers.IsVanillaVersion)
			{
				switch (moveName.ToLower().Replace("-", " "))
				{
					case "feint attack":
						moveName = "faint attack";
						break;

					case "high jump kick":
						moveName = "hi jump kick";
						break;

					case "smelling salts":
						moveName = "smelling salt";
						break;
				}
			}
			else
			{
				switch (moveName.ToLower().Replace("-", " "))
				{
					case "vice grip":
						moveName = "vise grip";
						break;
				}
			}

			string outputName = "MOVE_" + GameDataHelpers.FormatKeyword(moveName);

			if (!GameDataHelpers.MoveDefines.ContainsKey(outputName))
			{
				// Attempt to remove spaces and find a matching name, as that's pretty minor and it's easier to do this
				string testName = outputName.Replace("_", "");

				foreach(var kvp in GameDataHelpers.MoveDefines)
				{
					if (testName == kvp.Key.Replace("_", ""))
						return kvp.Key;
				}
			}

			return outputName;
		}

		private static string FormatItemName(string itemName)
		{
			if (!GameDataHelpers.IsVanillaVersion)
			{
				switch(itemName.ToLower().Replace("-", " "))
				{
					case "stick":
						itemName = "leek";
						break;
				}
			}

			string outputName = "ITEM_" + GameDataHelpers.FormatKeyword(itemName);

			if (!GameDataHelpers.ItemDefines.ContainsKey(outputName))
			{
				// Attempt to remove spaces and find a matching name, as that's pretty minor and it's easier to do this
				string testName = outputName.Replace("_", "");

				foreach (var kvp in GameDataHelpers.ItemDefines)
				{
					if (testName == kvp.Key.Replace("_", ""))
						return kvp.Key;
				}
			}

			return outputName;
		}

		private static bool IsBannedMove(string moveName)
		{
			// No moves are currently banned. Tera Blast is replaced by Hidden Power elsewhere.
			return false;
		}

		internal class ProfileBundle
		{
			public List<PokemonProfile> Profiles = new List<PokemonProfile>();
			public Dictionary<string, string> RedirectedSpecies = new Dictionary<string, string>();
			public Dictionary<string, string> ZaMegaResolutions = new Dictionary<string, string>();
			public Dictionary<string, int> TransformationMatches = new Dictionary<string, int>();
			public Dictionary<string, SpeciesSourceMetadata> SpeciesMetadata = new Dictionary<string, SpeciesSourceMetadata>();
		}

		internal class SpeciesSourceMetadata
		{
			public string ProjectSpecies;
			public string PokeApiSpecies;
			public string PokeApiForm;
			public string ShowdownId;
			public string UpstreamBaseSpecies;
			public bool HasDedicatedCompetitiveSets;
			public string ProfileSource;
		}

		public static void GatherProfiles()
		{
			ProfileBundle bundle = GatherSourceProfileBundle(false);
			PrepareProfilesForGame(bundle);
			ExportProfileBundle(bundle, Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\rogue_pokemon_profiles.h"));
		}

		internal static ProfileBundle GatherSourceProfileBundle(bool bypassHttpCache)
		{
			ContentCache.BypassHttpCache = bypassHttpCache;
			List<PokemonProfile> profiles = new List<PokemonProfile>();
			Dictionary<string, string> redirectedSpecies = new Dictionary<string, string>();
			Dictionary<string, string> zaMegaResolutions = new Dictionary<string, string>();
			Dictionary<string, SpeciesSourceMetadata> speciesMetadata = new Dictionary<string, SpeciesSourceMetadata>();
			int exceptionHitCount = 0;

			if (bypassHttpCache)
			{
				PokeAPI.PrefetchPokemonProfiles(
					GameDataHelpers.SpeciesDefines.Keys.Where(GameDataHelpers.IsUniqueSpeciesDefine));
			}

			Stack<string> speciesToProcess = new Stack<string>();
			foreach (var kvp in GameDataHelpers.SpeciesDefines.Reverse())
			{
				speciesToProcess.Push(kvp.Key);
			}

			while(speciesToProcess.Count != 0)
			{
				string speciesName = speciesToProcess.Pop();

				try
				{
					if (!GameDataHelpers.IsUniqueSpeciesDefine(speciesName))
						continue;

					PokeAPI.RedirectSpeciesLookupName(speciesName, out string apiSpeciesName, out string apiFormName);
					SpeciesSourceMetadata metadata = new SpeciesSourceMetadata
					{
						ProjectSpecies = speciesName,
						PokeApiSpecies = apiSpeciesName,
						PokeApiForm = apiFormName,
						ShowdownId = PokeAPI.NormalizeIdentifier(speciesName.Replace("SPECIES_", "")),
						HasDedicatedCompetitiveSets = !GameDataHelpers.IsVanillaVersion
							&& PokeAPI.HasShowdownCompetitiveSets(speciesName),
					};
					speciesMetadata[speciesName] = metadata;

					if (GameDataHelpers.IsVanillaVersion && speciesName.StartsWith("SPECIES_UNOWN_"))
					{
						// Skip these as they appear after NUM_SPECIES
						continue;
					}

					// Skipped species (Handle elsewhere)
					string redirectSpecies = null;

					if (speciesName.StartsWith("SPECIES_UNOWN_"))
					{
						redirectSpecies = "SPECIES_UNOWN";
					}
					else if (speciesName.StartsWith("SPECIES_CASTFORM_"))
					{
						redirectSpecies = "SPECIES_CASTFORM";
					}

					if (redirectSpecies == null)
					{
						if (!GameDataHelpers.IsVanillaVersion)
						{
							if (PokeAPI.TryGetShowdownGen9MegaBaseSpecies(speciesName, out string upstreamMegaBase))
							{
								switch (speciesName)
								{
									case "SPECIES_FLOETTE_MEGA":
										upstreamMegaBase = "SPECIES_FLOETTE_ETERNAL_FLOWER";
										break;
									case "SPECIES_MEOWSTIC_F_MEGA":
										upstreamMegaBase = "SPECIES_MEOWSTIC_FEMALE";
										break;
									case "SPECIES_TATSUGIRI_DROOPY_MEGA":
										upstreamMegaBase = "SPECIES_TATSUGIRI_DROOPY";
										break;
									case "SPECIES_TATSUGIRI_STRETCHY_MEGA":
										upstreamMegaBase = "SPECIES_TATSUGIRI_STRETCHY";
										break;
								}
								zaMegaResolutions[speciesName] = upstreamMegaBase;
								metadata.UpstreamBaseSpecies = upstreamMegaBase;
								if (!metadata.HasDedicatedCompetitiveSets)
									redirectSpecies = upstreamMegaBase;
							}

							// Only redirect species which are functionally identical for rogue spawning
							if (redirectSpecies == null
								&& !zaMegaResolutions.ContainsKey(speciesName)
								&& speciesName.EndsWith("_MEGA"))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_MEGA".Length);

								switch (speciesName)
								{
									case "SPECIES_PIKIN_MEGA":
										redirectSpecies = "SPECIES_MAREEP";
										break;
								}
							}
							else if (!zaMegaResolutions.ContainsKey(speciesName)
								&& (speciesName.EndsWith("_MEGA_X") || speciesName.EndsWith("_MEGA_Y")))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_MEGA_X".Length);
							}
							else if (speciesName.EndsWith("_PRIMAL"))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_PRIMAL".Length);
							}
							else if (speciesName.EndsWith("_GIGANTAMAX"))
							{
								switch (speciesName)
								{
									case "SPECIES_TOXTRICITY_AMPED_GIGANTAMAX":
										redirectSpecies = "SPECIES_TOXTRICITY";
										break;

									case "SPECIES_URSHIFU_SINGLE_STRIKE_STYLE_GIGANTAMAX":
										redirectSpecies = "SPECIES_URSHIFU";
										break;

									default:
										redirectSpecies = speciesName.Substring(0, speciesName.Length - "_GIGANTAMAX".Length);
										break;
								}
							}
							else if (speciesName.StartsWith("SPECIES_BURMY_"))
							{
								redirectSpecies = "SPECIES_BURMY";
							}
							else if (speciesName.StartsWith("SPECIES_ARCEUS_"))
							{
								redirectSpecies = "SPECIES_ARCEUS";
							}
							else if (speciesName.StartsWith("SPECIES_DEERLING_"))
							{
								redirectSpecies = "SPECIES_DEERLING";
							}
							else if (speciesName.StartsWith("SPECIES_SAWSBUCK_"))
							{
								redirectSpecies = "SPECIES_SAWSBUCK";
							}
							else if (speciesName.StartsWith("SPECIES_GENESECT_"))
							{
								redirectSpecies = "SPECIES_GENESECT";
							}
							else if (speciesName.StartsWith("SPECIES_VIVILLON_"))
							{
								redirectSpecies = "SPECIES_VIVILLON";
							}
							else if (speciesName.StartsWith("SPECIES_FLABEBE_"))
							{
								redirectSpecies = "SPECIES_FLABEBE";
							}
							else if (speciesName.StartsWith("SPECIES_FLOETTE_")
								&& speciesName != "SPECIES_FLOETTE_ETERNAL_FLOWER"
								&& speciesName != "SPECIES_FLOETTE_MEGA")
							{
								redirectSpecies = "SPECIES_FLOETTE";
							}
							else if (speciesName.StartsWith("SPECIES_FLORGES_"))
							{
								redirectSpecies = "SPECIES_FLORGES";
							}
							else if (speciesName.StartsWith("SPECIES_FURFROU_"))
							{
								redirectSpecies = "SPECIES_FURFROU";
							}
							else if (speciesName.StartsWith("SPECIES_PUMPKABOO_"))
							{
								redirectSpecies = "SPECIES_PUMPKABOO";
							}
							else if (speciesName.StartsWith("SPECIES_GOURGEIST_"))
							{
								redirectSpecies = "SPECIES_GOURGEIST";
							}
							else if (speciesName.StartsWith("SPECIES_SILVALLY_"))
							{
								redirectSpecies = "SPECIES_SILVALLY";
							}
							else if (speciesName.StartsWith("SPECIES_MINIOR_"))
							{
								redirectSpecies = "SPECIES_MINIOR";
							}
							else if (speciesName.StartsWith("SPECIES_CRAMORANT_"))
							{
								redirectSpecies = "SPECIES_CRAMORANT";
							}
							else if (speciesName.StartsWith("SPECIES_ALCREMIE_"))
							{
								redirectSpecies = "SPECIES_ALCREMIE";
							}
							else if (speciesName.StartsWith("SPECIES_SQUAWKABILLY_"))
							{
								redirectSpecies = "SPECIES_SQUAWKABILLY";
							}
							else
							{
								switch (speciesName)
								{
									case "SPECIES_CHERRIM_SUNSHINE":
										redirectSpecies = "SPECIES_CHERRIM";
										break;

									case "SPECIES_SHELLOS_EAST_SEA":
										redirectSpecies = "SPECIES_SHELLOS";
										break;

									case "SPECIES_GASTRODON_EAST_SEA":
										redirectSpecies = "SPECIES_GASTRODON";
										break;

									case "SPECIES_DARMANITAN_ZEN_MODE":
										redirectSpecies = "SPECIES_DARMANITAN";
										break;

									case "SPECIES_DARMANITAN_ZEN_MODE_GALARIAN":
										redirectSpecies = "SPECIES_DARMANITAN_GALARIAN";
										break;

									case "SPECIES_KELDEO_RESOLUTE":
										redirectSpecies = "SPECIES_KELDEO";
										break;

									case "SPECIES_MELOETTA_PIROUETTE":
										redirectSpecies = "SPECIES_MELOETTA";
										break;

									case "SPECIES_AEGISLASH_BLADE":
										redirectSpecies = "SPECIES_AEGISLASH";
										break;

									case "SPECIES_XERNEAS_ACTIVE":
										redirectSpecies = "SPECIES_XERNEAS";
										break;

									case "SPECIES_ZYGARDE_50_POWER_CONSTRUCT":
										redirectSpecies = "SPECIES_ZYGARDE";
										break;

									case "SPECIES_WISHIWASHI_SCHOOL":
										redirectSpecies = "SPECIES_WISHIWASHI";
										break;

									case "SPECIES_MIMIKYU_BUSTED":
										redirectSpecies = "SPECIES_MIMIKYU";
										break;

									case "SPECIES_MAGEARNA_ORIGINAL_COLOR":
										redirectSpecies = "SPECIES_MAGEARNA";
										break;

									case "SPECIES_SINISTEA_ANTIQUE":
										redirectSpecies = "SPECIES_SINISTEA";
										break;
									case "SPECIES_POLTEAGEIST_ANTIQUE":
										redirectSpecies = "SPECIES_POLTEAGEIST";
										break;

									case "SPECIES_EISCUE_NOICE_FACE":
										redirectSpecies = "SPECIES_EISCUE";
										break;

									case "SPECIES_MORPEKO_HANGRY":
										redirectSpecies = "SPECIES_MORPEKO";
										break;

									case "SPECIES_ETERNATUS_ETERNAMAX":
										redirectSpecies = "SPECIES_ETERNATUS";
										break;

									case "SPECIES_ZARUDE_DADA":
										redirectSpecies = "SPECIES_ZARUDE";
										break;

									case "SPECIES_OGERPON_TEAL_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON";
										break;

									case "SPECIES_OGERPON_WELLSPRING_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON_WELLSPRING_MASK";
										break;

									case "SPECIES_OGERPON_HEARTHFLAME_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON_HEARTHFLAME_MASK";
										break;

									case "SPECIES_OGERPON_CORNERSTONE_MASK_TERA":
										redirectSpecies = "SPECIES_OGERPON_CORNERSTONE_MASK";
										break;

									case "SPECIES_TERAPAGOS_TERASTAL":
									case "SPECIES_TERAPAGOS_STELLAR":
										redirectSpecies = "SPECIES_TERAPAGOS";
										break;

									case "SPECIES_WOBBUFFET_PUNCHING":
										redirectSpecies = "SPECIES_WOBBUFFET";
										break;
								}
							}
						}
					}

					if (redirectSpecies != null)
					{
						if (metadata.UpstreamBaseSpecies == null)
							metadata.UpstreamBaseSpecies = redirectSpecies;
						metadata.ProfileSource = zaMegaResolutions.ContainsKey(speciesName)
							? "showdown-form-redirect"
							: "legacy-redirect";
						if (!GameDataHelpers.SpeciesDefines.ContainsKey(redirectSpecies))
							throw new InvalidDataException();

						redirectedSpecies[speciesName] = redirectSpecies;
						continue;
					}

					string manualProfilePath = ContentCache.GetWriteableCachePath(
						$"res://PokemonProfiles//{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
					metadata.ProfileSource = File.Exists(manualProfilePath)
						? "manual-resource"
						: "pokeapi-showdown";
					PokemonProfile profile = GatherProfileFor(speciesName, bypassHttpCache);
					profiles.Add(profile);
				}
				catch (AggregateException e)
				{
					if (e.InnerException is HttpRequestException)
						Console.WriteLine($"\tCaught Http Exception '{e.InnerException.Message}'");

					if (++exceptionHitCount < 5)
					{
						speciesToProcess.Push(speciesName);

						Console.WriteLine($"\tSleeping (assuming just rapid access error)");
						Thread.Sleep(5000);
					}
					else
					{
						// Gone over threshold
						throw e;
					}
				}
			}

			ContentCache.BypassHttpCache = false;
			return new ProfileBundle
			{
				Profiles = profiles,
				RedirectedSpecies = redirectedSpecies,
				ZaMegaResolutions = zaMegaResolutions,
				SpeciesMetadata = speciesMetadata,
			};
		}

		private static PokemonProfile GatherProfileFor(string speciesName, bool bypassCache)
		{
			string manualPath = ContentCache.GetWriteableCachePath($"res://PokemonProfiles//{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
			string cachePath = ContentCache.GetWriteableCachePath(
				$"pokemon_profiles/v{ProfileCacheSchemaVersion}/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
			PokemonProfile outputProfile;

			if (File.Exists(manualPath))
			{
				Console.WriteLine($"Found '{speciesName}' profile manual override");

				string jsonProfile = File.ReadAllText(manualPath);
				outputProfile = JsonConvert.DeserializeObject<PokemonProfile>(jsonProfile, c_JsonSettings);
			}
			else if (!bypassCache && File.Exists(cachePath))
			{
				Console.WriteLine($"Found '{speciesName}' profile in cache");

				string jsonProfile = File.ReadAllText(cachePath);
				outputProfile = JsonConvert.DeserializeObject<PokemonProfile>(jsonProfile, c_JsonSettings);
			}
			else
			{
				Console.WriteLine($"Gathering '{speciesName}' profile from source");

				SourcePokemonProfile sourceProfile = new SourcePokemonProfile(speciesName);

				JObject monEntry = PokeAPI.GetPokemonSpeciesEntry(speciesName);

				foreach (var obj in monEntry["abilities"])
				{
					string abilityName = obj["ability"]["name"].ToString();
					string rawSlot = obj["slot"].ToString();

					sourceProfile.Abilities[int.Parse(rawSlot) - 1] = abilityName;
				}

				foreach (var obj in monEntry["types"])
				{
					string name = obj["type"]["name"].ToString();
					string rawSlot = obj["slot"].ToString();

					sourceProfile.Types[int.Parse(rawSlot) - 1] = name;
				}

				foreach (var moveObj in monEntry["moves"])
				{
					foreach (var versionObj in moveObj["version_group_details"])
					{
						SourceMoveInfo moveInfo = new SourceMoveInfo();
						moveInfo.moveName = moveObj["move"]["name"].ToString();
						moveInfo.versionName = versionObj["version_group"]["name"].ToString();

						string method = versionObj["move_learn_method"]["name"].ToString();
						switch (method)
						{
							case "egg":
								moveInfo.originMethod = SourceMoveInfo.LearnMethod.Egg;
								break;
							case "machine":
								moveInfo.originMethod = SourceMoveInfo.LearnMethod.TM;
								break;
							case "tutor":
								moveInfo.originMethod = SourceMoveInfo.LearnMethod.Tutor;
								break;
							case "level-up":
								moveInfo.originMethod = SourceMoveInfo.LearnMethod.LevelUp;
								moveInfo.learnLevel = int.Parse(versionObj["level_learned_at"].ToString());
								break;

							// Special cases
							//case "stadium-surfing-pikachu":
							//case "light-ball-egg":
							default:
								moveInfo.originMethod = SourceMoveInfo.LearnMethod.Tutor;
								break;

								//default:
								//	throw new NotImplementedException();
						}

						sourceProfile.Moves.Add(moveInfo);
					}
				}

				JObject competitiveSets = PokeAPI.GetPokemonSpeciesCompetitiveSets(speciesName);

				foreach (var tierKvp in competitiveSets)
				{
					foreach (var currentSet in tierKvp.Value.Value<JArray>())
					{
						string tierName = GameDataHelpers.FormatKeyword(tierKvp.Key);
						PokemonCompetitiveSet compSet = PokemonCompetitiveSet.ParseFrom(tierName, currentSet.Value<JObject>());

						bool hasMerged = false;

						foreach (var existingSet in sourceProfile.CompetitiveSets)
						{
							// No need to contain duplicate sets
							if (existingSet.IsCompatibleWith(compSet))
							{
								existingSet.SourceTiers.Add(tierName);
								existingSet.SourceIds.AddRange(compSet.SourceIds);
								hasMerged = true;
								break;
							}
						}

						if (!hasMerged)
							sourceProfile.CompetitiveSets.Add(compSet);
					}
				}

				sourceProfile.CollapseMovesets();

				outputProfile = PokemonProfile.FromSource(sourceProfile);

				if (!bypassCache)
				{
					string cacheDir = Path.GetDirectoryName(cachePath);
					Directory.CreateDirectory(cacheDir);

					string profileJson = JsonConvert.SerializeObject(outputProfile, c_JsonSettings);
					File.WriteAllText(cachePath, profileJson);
				}
			}

			return outputProfile;
		}

		internal static void PrepareProfilesForGame(ProfileBundle bundle)
		{
			s_TransformationMatchCounts.Clear();
			foreach (PokemonProfile profile in bundle.Profiles)
			{
				profile.FormatDataForGame();
				profile.ValidateContents();
			}
			bundle.TransformationMatches = new Dictionary<string, int>(s_TransformationMatchCounts);
		}

		internal static void ExportProfileBundle(ProfileBundle bundle, string filePath)
		{
			ExportProfiles(bundle.Profiles, bundle.RedirectedSpecies, filePath);
		}

		private static string GetRogueRoleFlag(PokemonCompetitiveSet set)
		{
			switch (set.RogueRole)
			{
				case null:
				case "standard":
					return null;
				case "singles-strong":
					return "MON_FLAG_SINGLES_STRONG";
				case "doubles-strong":
					return "MON_FLAG_DOUBLES_STRONG";
				case "strong-wild":
					return "MON_FLAG_STRONG_WILD";
				default:
					throw new InvalidDataException(
						$"{set.StableId ?? "competitive set"} has unsupported Rogue role {set.RogueRole}.");
			}
		}

		private static void ExportProfiles(List<PokemonProfile> profiles, Dictionary<string, string> redirectedSpecies, string filePath)
		{
			Console.WriteLine($"Exporting profiles to '{filePath}'");

			StringBuilder upperBlock = new StringBuilder();
			StringBuilder lowerBlock = new StringBuilder();

			upperBlock.AppendLine("// == WARNING ==");
			upperBlock.AppendLine("// DO NOT EDIT THIS FILE");
			upperBlock.AppendLine("// This file was automatically generated by PokemonDataGenerator");
			upperBlock.AppendLine("//");
			upperBlock.AppendLine();


			// Gather some usage info to slap at the top
			//
			{
				PokemonProfile mostLevelMoves = null;
				PokemonProfile mostTutorMoves = null;
				HashSet<string> competitiveFormats = new HashSet<string>();

				foreach (var profile in profiles)
				{
					if (mostLevelMoves == null || profile.LevelUpMoves.Count > mostLevelMoves.LevelUpMoves.Count)
						mostLevelMoves = profile;

					if (mostTutorMoves == null || profile.TutorMoves.Count > mostTutorMoves.TutorMoves.Count)
						mostTutorMoves = profile;

					foreach (var compSet in profile.CompetitiveSets)
					{
						foreach (var tier in compSet.SourceTiers)
							competitiveFormats.Add(tier);
					}
				}

				upperBlock.AppendLine("// == INFO ==");
				upperBlock.AppendLine("//");
				upperBlock.AppendLine("// Highest Move Count");
				upperBlock.AppendLine($"// Level Up: {mostLevelMoves.Species} ({mostLevelMoves.LevelUpMoves.Count})");
				upperBlock.AppendLine($"// Tutor: {mostTutorMoves.Species} ({mostTutorMoves.TutorMoves.Count})");

				upperBlock.AppendLine("//");
				upperBlock.AppendLine("// Source Tiers:");
				foreach (var tier in competitiveFormats.OrderBy(s => s.ToLower()))
				{
					upperBlock.AppendLine($"// {tier}");
				}
				upperBlock.AppendLine("//");
			}

			// Move/Item usages
			//
			Dictionary<string, int> moveCount = new Dictionary<string, int>();
			Dictionary<string, int> specialMoveCount = new Dictionary<string, int>(); // i.e. moves found not in the level up moveset
			Dictionary<string, int> heldItemCount = new Dictionary<string, int>();

			foreach (var profile in profiles)
			{
				HashSet<string> uniqueItems = new HashSet<string>();
				HashSet<string> uniqueMoves = new HashSet<string>();

				foreach (var compSet in profile.CompetitiveSets)
				{
					foreach (var move in compSet.Moves)
						uniqueMoves.Add(move);

					if (compSet.Item != null)
						uniqueItems.Add(compSet.Item);
				}

				foreach (var move in uniqueMoves)
				{
					if (moveCount.ContainsKey(move))
						++moveCount[move];
					else
						moveCount[move] = 1;

					if (!profile.HasLevelUpMove(move))
					{
						if (specialMoveCount.ContainsKey(move))
							++specialMoveCount[move];
						else
							specialMoveCount[move] = 1;
					}
				}

				foreach (var item in uniqueItems)
				{
					if (heldItemCount.ContainsKey(item))
						++heldItemCount[item];
					else
						heldItemCount[item] = 1;
				}
			}

			upperBlock.AppendLine("u16 const gRoguePokemonMoveUsages[MOVES_COUNT] =\n{");

			foreach (var kvp in moveCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			upperBlock.AppendLine("u16 const gRoguePokemonSpecialMoveUsages[MOVES_COUNT] =\n{");

			foreach (var kvp in specialMoveCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			upperBlock.AppendLine("u16 const gRoguePokemonHeldItemUsages[ITEMS_COUNT] =\n{");

			foreach (var kvp in heldItemCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			// Pokemon Profiles
			//
			lowerBlock.AppendLine("struct RoguePokemonProfile const gRoguePokemonProfiles[NUM_SPECIES] =\n{");

			foreach(var profile in profiles)
			{
				// Mon flags
				HashSet<string> sourceTiers = new HashSet<string>();
				HashSet<string> rogueRoleFlags = new HashSet<string>();

				foreach (var compSet in profile.CompetitiveSets)
				{
					foreach (var tier in compSet.SourceTiers)
						sourceTiers.Add(GameDataHelpers.FormatKeyword(tier));
					string roleFlag = GetRogueRoleFlag(compSet);
					if (roleFlag != null)
						rogueRoleFlags.Add(roleFlag);
				}

				upperBlock.AppendLine($"#ifdef APPEND_MON_FLAGS_{profile.Species}");

				upperBlock.Append($"#define MON_FLAGS_{profile.Species} (APPEND_MON_FLAGS_{profile.Species}"); // allow easily appending flags
				foreach (var tier in sourceTiers)
					upperBlock.Append($" | MON_FLAGS_{tier}");
				foreach (var roleFlag in rogueRoleFlags.OrderBy(flag => flag))
					upperBlock.Append($" | {roleFlag}");
				upperBlock.AppendLine(")");

				upperBlock.AppendLine("#else");

				upperBlock.Append($"#define MON_FLAGS_{profile.Species} (0");
				foreach (var tier in sourceTiers)
					upperBlock.Append($" | MON_FLAGS_{tier}");
				foreach (var roleFlag in rogueRoleFlags.OrderBy(flag => flag))
					upperBlock.Append($" | {roleFlag}");
				upperBlock.AppendLine(")");

				upperBlock.AppendLine("#endif");
				upperBlock.AppendLine("");

				// Level moves
				upperBlock.AppendLine($"static struct LevelUpMove const sLevelUpMoves_{profile.Species}[] =\n{{");
				foreach(var move in profile.LevelUpMoves)
				{
					upperBlock.AppendLine($"\t{{ .move={move.Move}, .level={move.Level} }},");
				}
				upperBlock.AppendLine($"\t{{ .move=MOVE_NONE, .level=0 }},");
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();

				// Tutor moves
				upperBlock.AppendLine($"static u16 const sTutorMoves_{profile.Species}[] =\n{{");
				foreach (var move in profile.TutorMoves)
				{
					upperBlock.AppendLine($"\t{move},");
				}
				upperBlock.AppendLine($"\tMOVE_NONE,");
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();

				// Comp sets
				upperBlock.AppendLine($"static struct RoguePokemonCompetitiveSet const sCompetitiveSets_{profile.Species}[] =\n{{");
				foreach(var compSet in profile.CompetitiveSets)
				{
					upperBlock.AppendLine($"\t{{");

					upperBlock.Append($"\t\t.flags= (0");
					foreach (var tier in compSet.SourceTiers)
						upperBlock.Append($" | MON_FLAGS_{tier}");
					string roleFlag = GetRogueRoleFlag(compSet);
					if (roleFlag != null)
						upperBlock.Append($" | {roleFlag}");
					upperBlock.AppendLine("),");

					if (compSet.Item != null)
						upperBlock.AppendLine($"\t\t.heldItem={compSet.Item},");

					if (compSet.Ability != null)
						upperBlock.AppendLine($"\t\t.ability={compSet.Ability},");

					if (compSet.HiddenPower != null)
						upperBlock.AppendLine($"\t\t.hiddenPowerType={compSet.HiddenPower},");
					else
						upperBlock.AppendLine($"\t\t.hiddenPowerType=TYPE_NONE,");

					if (compSet.TeraType != null)
						upperBlock.AppendLine($"\t\t.teraType={compSet.TeraType},");
					else
						upperBlock.AppendLine($"\t\t.teraType=TYPE_NONE,");

					if (compSet.Nature != null)
						upperBlock.AppendLine($"\t\t.nature={compSet.Nature},");

					upperBlock.AppendLine($"\t\t.moves=\n\t\t{{");
					foreach(var move in compSet.Moves)
					{
						upperBlock.AppendLine($"\t\t\t{move},");
					}
					upperBlock.AppendLine($"\t\t}},");

					upperBlock.AppendLine($"\t}},");
				}
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();


				// Add to species lookup below
				lowerBlock.AppendLine($"\t[{profile.Species}] =\n\t{{");
				lowerBlock.AppendLine($"\t\t.levelUpMoves = sLevelUpMoves_{profile.Species},");
				lowerBlock.AppendLine($"\t\t.tutorMoves = sTutorMoves_{profile.Species},");
				lowerBlock.AppendLine($"\t\t.competitiveSets = sCompetitiveSets_{profile.Species},");
				lowerBlock.AppendLine($"\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_{profile.Species}),");
				lowerBlock.AppendLine($"\t\t.monFlags = MON_FLAGS_{profile.Species},");
				lowerBlock.AppendLine($"\t}},");
			}

			// Attach redirected species info too
			foreach(var kvp in redirectedSpecies)
			{
				lowerBlock.AppendLine($"\t[{kvp.Key}] =\n\t{{");
				lowerBlock.AppendLine($"\t\t.levelUpMoves = sLevelUpMoves_{kvp.Value},");
				lowerBlock.AppendLine($"\t\t.tutorMoves = sTutorMoves_{kvp.Value},");
				lowerBlock.AppendLine($"\t\t.competitiveSets = sCompetitiveSets_{kvp.Value},");
				lowerBlock.AppendLine($"\t\t.competitiveSetCount = ARRAY_COUNT(sCompetitiveSets_{kvp.Value}),");
				lowerBlock.AppendLine($"\t\t.monFlags = MON_FLAGS_{kvp.Value},");
				lowerBlock.AppendLine($"\t}},");
			}

			lowerBlock.AppendLine("};");

			File.WriteAllText(filePath, upperBlock.ToString() + "\n"+ lowerBlock.ToString());
		}
	}
}
