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


		private class SourceMoveInfo
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

		private class SourcePokemonProfile
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

		private class LevelUpMove
		{
			public string Move;
			public int Level;

			public override string ToString()
			{
				return Move + " @ Lv" + Level;
			}
		}

		private class PokemonProfile
		{
			// Profiles are cached after move filtering. Keep old caches usable and restore
			// deliberately selected compatibility for moves omitted from modern learnsets.
			private static readonly Dictionary<string, HashSet<string>> s_RestoredMoveLearnsets = new Dictionary<string, HashSet<string>>
			{
				{
					"MOVE_BARRAGE",
					new HashSet<string>
					{
						"SPECIES_EXEGGCUTE", "SPECIES_EXEGGUTOR", "SPECIES_EXEGGUTOR_ALOLAN",
					}
				},
				{
					"MOVE_BONE_CLUB",
					new HashSet<string>
					{
						"SPECIES_CUBONE", "SPECIES_MAROWAK", "SPECIES_MAROWAK_ALOLAN",
					}
				},
				{
					"MOVE_EGG_BOMB",
					new HashSet<string>
					{
						"SPECIES_EXEGGCUTE", "SPECIES_EXEGGUTOR", "SPECIES_CHANSEY", "SPECIES_MEW",
						"SPECIES_BLISSEY", "SPECIES_EXEGGUTOR_ALOLAN",
					}
				},
				{
					"MOVE_ION_DELUGE",
					new HashSet<string>
					{
						"SPECIES_CHINCHOU", "SPECIES_LANTURN", "SPECIES_AMPHAROS", "SPECIES_PACHIRISU",
						"SPECIES_ELECTIVIRE", "SPECIES_ZEBSTRIKA", "SPECIES_EMOLGA", "SPECIES_EELEKTROSS",
						"SPECIES_XURKITREE",
					}
				},
				{
					"MOVE_MAGNITUDE",
					new HashSet<string>
					{
						"SPECIES_SANDSHREW", "SPECIES_SANDSLASH", "SPECIES_DIGLETT", "SPECIES_DUGTRIO",
						"SPECIES_GEODUDE", "SPECIES_GRAVELER", "SPECIES_GOLEM", "SPECIES_LICKITUNG",
						"SPECIES_RHYHORN", "SPECIES_DONPHAN", "SPECIES_NOSEPASS", "SPECIES_NUMEL",
						"SPECIES_CAMERUPT", "SPECIES_BARBOACH", "SPECIES_WHISCASH", "SPECIES_RELICANTH",
						"SPECIES_TEPIG", "SPECIES_ROGGENROLA", "SPECIES_GOLETT", "SPECIES_GOLURK",
						"SPECIES_MUDBRAY", "SPECIES_DIGLETT_ALOLAN", "SPECIES_DUGTRIO_ALOLAN",
					}
				},
				{
					"MOVE_NEEDLE_ARM",
					new HashSet<string>
					{
						"SPECIES_CACNEA", "SPECIES_CACTURNE", "SPECIES_MARACTUS", "SPECIES_QUILLADIN",
						"SPECIES_CHESNAUGHT",
					}
				},
				{
					"MOVE_POWDER",
					new HashSet<string>
					{
						"SPECIES_VIVILLON", "SPECIES_CUTIEFLY",
					}
				},
				{
					"MOVE_POWER_SHIFT",
					new HashSet<string>
					{
						"SPECIES_GENGAR", "SPECIES_ONIX", "SPECIES_MR_MIME", "SPECIES_FLAREON",
						"SPECIES_UMBREON", "SPECIES_STEELIX", "SPECIES_MANTINE", "SPECIES_BLISSEY",
						"SPECIES_NOSEPASS", "SPECIES_DUSCLOPS", "SPECIES_RAMPARDOS", "SPECIES_BASTIODON",
						"SPECIES_VESPIQUEN", "SPECIES_DRIFBLIM", "SPECIES_CHATOT", "SPECIES_GLISCOR",
						"SPECIES_PORYGON_Z", "SPECIES_PROBOPASS", "SPECIES_DUSKNOIR", "SPECIES_UXIE",
						"SPECIES_AZELF", "SPECIES_REGIGIGAS", "SPECIES_CRESSELIA", "SPECIES_BRAVIARY_HISUIAN",
						"SPECIES_THUNDURUS", "SPECIES_THUNDURUS_THERIAN", "SPECIES_AVALUGG_HISUIAN",
						"SPECIES_SNEASLER", "SPECIES_ENAMORUS", "SPECIES_ENAMORUS_THERIAN",
					}
				},
				{
					"MOVE_SPIDER_WEB",
					new HashSet<string>
					{
						"SPECIES_SPINARAK", "SPECIES_ARIADOS", "SPECIES_JOLTIK", "SPECIES_GALVANTULA",
						"SPECIES_DEWPIDER", "SPECIES_ARAQUANID",
					}
				},
				{
					"MOVE_SPIKE_CANNON",
					new HashSet<string>
					{
						"SPECIES_CLOYSTER", "SPECIES_OMANYTE", "SPECIES_OMASTAR", "SPECIES_CORSOLA",
						"SPECIES_MAREANIE", "SPECIES_TOXAPEX",
					}
				},
				{
					"MOVE_TRUMP_CARD",
					new HashSet<string>
					{
						"SPECIES_FARFETCHD", "SPECIES_KANGASKHAN", "SPECIES_EEVEE", "SPECIES_SLOWKING",
						"SPECIES_DUNSPARCE", "SPECIES_MINUN", "SPECIES_CORPHISH", "SPECIES_SHELLOS",
						"SPECIES_OSHAWOTT", "SPECIES_MAGEARNA",
					}
				},
				{
					"MOVE_ALLURING_VOICE",
					new HashSet<string>
					{
						"SPECIES_PIKACHU", "SPECIES_RAICHU", "SPECIES_CLEFAIRY", "SPECIES_CLEFABLE",
						"SPECIES_JIGGLYPUFF", "SPECIES_WIGGLYTUFF", "SPECIES_DEWGONG", "SPECIES_LAPRAS",
						"SPECIES_EEVEE", "SPECIES_VAPOREON", "SPECIES_JOLTEON", "SPECIES_FLAREON",
						"SPECIES_MEW", "SPECIES_CLEFFA", "SPECIES_IGGLYBUFF", "SPECIES_MARILL",
						"SPECIES_AZUMARILL", "SPECIES_ESPEON", "SPECIES_UMBREON", "SPECIES_BLISSEY",
						"SPECIES_RALTS", "SPECIES_KIRLIA", "SPECIES_GARDEVOIR", "SPECIES_AZURILL",
						"SPECIES_PLUSLE", "SPECIES_MINUN", "SPECIES_FLYGON", "SPECIES_ALTARIA",
						"SPECIES_MILOTIC", "SPECIES_LATIAS", "SPECIES_PACHIRISU", "SPECIES_FINNEON",
						"SPECIES_LUMINEON", "SPECIES_LEAFEON", "SPECIES_GLACEON", "SPECIES_GALLADE",
						"SPECIES_PHIONE", "SPECIES_MANAPHY", "SPECIES_LILLIGANT", "SPECIES_MINCCINO",
						"SPECIES_CINCCINO", "SPECIES_SWANNA", "SPECIES_ALOMOMOLA", "SPECIES_MELOETTA",
						"SPECIES_FLABEBE", "SPECIES_FLOETTE", "SPECIES_FLORGES", "SPECIES_MEOWSTIC",
						"SPECIES_SYLVEON", "SPECIES_PRIMARINA", "SPECIES_ORICORIO", "SPECIES_RIBOMBEE",
						"SPECIES_COMFEY", "SPECIES_ALCREMIE", "SPECIES_ENAMORUS", "SPECIES_SKELEDIRGE",
						"SPECIES_FIDOUGH", "SPECIES_DACHSBUN", "SPECIES_ARBOLIVA", "SPECIES_FEZANDIPITI",
						"SPECIES_MEOWSTIC_FEMALE", "SPECIES_RAICHU_ALOLAN", "SPECIES_ORICORIO_POM_POM",
						"SPECIES_ORICORIO_PAU", "SPECIES_ORICORIO_SENSU", "SPECIES_ENAMORUS_THERIAN",
					}
				},
				{
					"MOVE_PSYCHIC_NOISE",
					new HashSet<string>
					{
						"SPECIES_JIGGLYPUFF", "SPECIES_WIGGLYTUFF", "SPECIES_VENONAT", "SPECIES_VENOMOTH",
						"SPECIES_PSYDUCK", "SPECIES_GOLDUCK", "SPECIES_SLOWBRO", "SPECIES_GENGAR",
						"SPECIES_DROWZEE", "SPECIES_HYPNO", "SPECIES_EXEGGCUTE", "SPECIES_EXEGGUTOR",
						"SPECIES_LAPRAS", "SPECIES_MEWTWO", "SPECIES_MEW", "SPECIES_NOCTOWL",
						"SPECIES_YANMA", "SPECIES_ESPEON", "SPECIES_MURKROW", "SPECIES_SLOWKING",
						"SPECIES_MISDREAVUS", "SPECIES_GIRAFARIG", "SPECIES_LUGIA", "SPECIES_GARDEVOIR",
						"SPECIES_GRUMPIG", "SPECIES_FLYGON", "SPECIES_CHIMECHO", "SPECIES_METANG",
						"SPECIES_METAGROSS", "SPECIES_LATIOS", "SPECIES_JIRACHI", "SPECIES_DEOXYS",
						"SPECIES_VESPIQUEN", "SPECIES_MISMAGIUS", "SPECIES_HONCHKROW", "SPECIES_CHINGLING",
						"SPECIES_BRONZONG", "SPECIES_YANMEGA", "SPECIES_UXIE", "SPECIES_MESPRIT",
						"SPECIES_GOTHITA", "SPECIES_GOTHORITA", "SPECIES_GOTHITELLE", "SPECIES_REUNICLUS",
						"SPECIES_DELPHOX", "SPECIES_FLORGES", "SPECIES_ESPURR", "SPECIES_MEOWSTIC",
						"SPECIES_MALAMAR", "SPECIES_TREVENANT", "SPECIES_NOIVERN", "SPECIES_HOOPA",
						"SPECIES_PRIMARINA", "SPECIES_RIBOMBEE", "SPECIES_ORANGURU", "SPECIES_BRUXISH",
						"SPECIES_TOXTRICITY", "SPECIES_HATTERENE", "SPECIES_INDEEDEE", "SPECIES_WYRDEER",
						"SPECIES_RABSCA", "SPECIES_FARIGIRAF", "SPECIES_SCREAM_TAIL", "SPECIES_MUNKIDORI",
						"SPECIES_IRON_CROWN", "SPECIES_DEOXYS_ATTACK", "SPECIES_DEOXYS_DEFENSE",
						"SPECIES_DEOXYS_SPEED", "SPECIES_MEOWSTIC_FEMALE", "SPECIES_HOOPA_UNBOUND",
						"SPECIES_RAICHU_ALOLAN", "SPECIES_EXEGGUTOR_ALOLAN", "SPECIES_ARTICUNO_GALARIAN",
						"SPECIES_SLOWKING_GALARIAN", "SPECIES_TOXTRICITY_LOW_KEY", "SPECIES_INDEEDEE_FEMALE",
						"SPECIES_BRAVIARY_HISUIAN",
					}
				},
				{
					"MOVE_UPPER_HAND",
					new HashSet<string>
					{
						"SPECIES_PIKACHU", "SPECIES_RAICHU", "SPECIES_POLIWRATH", "SPECIES_HITMONLEE",
						"SPECIES_HITMONCHAN", "SPECIES_MEW", "SPECIES_AIPOM", "SPECIES_HERACROSS",
						"SPECIES_SNEASEL", "SPECIES_TYROGUE", "SPECIES_HITMONTOP", "SPECIES_TREECKO",
						"SPECIES_GROVYLE", "SPECIES_SCEPTILE", "SPECIES_BLAZIKEN", "SPECIES_SHIFTRY",
						"SPECIES_MAKUHITA", "SPECIES_HARIYAMA", "SPECIES_MEDITITE", "SPECIES_MEDICHAM",
						"SPECIES_ZANGOOSE", "SPECIES_MONFERNO", "SPECIES_INFERNAPE", "SPECIES_AMBIPOM",
						"SPECIES_RIOLU", "SPECIES_LUCARIO", "SPECIES_CROAGUNK", "SPECIES_TOXICROAK",
						"SPECIES_WEAVILE", "SPECIES_GALLADE", "SPECIES_SAMUROTT", "SPECIES_CONKELDURR",
						"SPECIES_SCRAGGY", "SPECIES_SCRAFTY", "SPECIES_MIENFOO", "SPECIES_MIENSHAO",
						"SPECIES_COBALION", "SPECIES_TERRAKION", "SPECIES_VIRIZION", "SPECIES_KELDEO",
						"SPECIES_GRENINJA", "SPECIES_TALONFLAME", "SPECIES_HAWLUCHA", "SPECIES_DECIDUEYE",
						"SPECIES_CRABRAWLER", "SPECIES_CRABOMINABLE", "SPECIES_PASSIMIAN", "SPECIES_HAKAMO_O",
						"SPECIES_KOMMO_O", "SPECIES_FALINKS", "SPECIES_SNEASLER", "SPECIES_QUAQUAVAL",
						"SPECIES_SPIDOPS", "SPECIES_PAWMO", "SPECIES_PAWMOT", "SPECIES_FLAMIGO",
						"SPECIES_OKIDOGI", "SPECIES_RAICHU_ALOLAN", "SPECIES_LYCANROC_MIDNIGHT",
						"SPECIES_SAMUROTT_HISUIAN", "SPECIES_LILLIGANT_HISUIAN", "SPECIES_DECIDUEYE_HISUIAN",
					}
				},
			};

			private static readonly Dictionary<string, LevelUpMove[]> s_DivergenceLevelUpMoveLearnsets = new Dictionary<string, LevelUpMove[]>
			{
				{
					"SPECIES_UNOWN",
					new[]
					{
						new LevelUpMove { Move = "MOVE_SECRET_POWER", Level = 12 },
						new LevelUpMove { Move = "MOVE_ANCIENT_POWER", Level = 20 },
						new LevelUpMove { Move = "MOVE_COSMIC_POWER", Level = 28 },
						new LevelUpMove { Move = "MOVE_POWER_GEM", Level = 36 },
						new LevelUpMove { Move = "MOVE_STORED_POWER", Level = 40 },
						new LevelUpMove { Move = "MOVE_EARTH_POWER", Level = 48 },
					}
				},
				{ "SPECIES_HUNTAIL", new[] { new LevelUpMove { Move = "MOVE_DRAGON_TAIL", Level = 42 } } },
				{ "SPECIES_PARASECT", new[] { new LevelUpMove { Move = "MOVE_SHADOW_CLAW", Level = 0 } } },
				{ "SPECIES_GRIMMSNARL", new[] { new LevelUpMove { Move = "MOVE_BADDY_BAD", Level = 48 } } },
				{ "SPECIES_JELLICENT", new[] { new LevelUpMove { Move = "MOVE_BOUNCY_BUBBLE", Level = 48 } } },
				{ "SPECIES_VIKAVOLT", new[] { new LevelUpMove { Move = "MOVE_BUZZY_BUZZ", Level = 48 } } },
				{ "SPECIES_DRIFLOON", new[] { new LevelUpMove { Move = "MOVE_FLOATY_FALL", Level = 40 } } },
				{ "SPECIES_DRIFBLIM", new[] { new LevelUpMove { Move = "MOVE_FLOATY_FALL", Level = 40 } } },
				{ "SPECIES_CRYOGONAL", new[] { new LevelUpMove { Move = "MOVE_FREEZY_FROST", Level = 48 } } },
				{ "SPECIES_GARDEVOIR", new[] { new LevelUpMove { Move = "MOVE_GLITZY_GLOW", Level = 48 } } },
				{ "SPECIES_PICHU", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_RAICHU", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_RAICHU_ALOLAN", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_COSPLAY", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_ROCK_STAR", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_BELLE", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_POP_STAR", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_PH_D", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_LIBRE", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_ORIGINAL_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_HOENN_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_SINNOH_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_UNOVA_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_KALOS_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_ALOLA_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_PARTNER_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_PIKACHU_WORLD_CAP", new[] { new LevelUpMove { Move = "MOVE_PIKA_PAPOW", Level = 50 } } },
				{ "SPECIES_SUNFLORA", new[] { new LevelUpMove { Move = "MOVE_SAPPY_SEED", Level = 48 } } },
				{ "SPECIES_SIZZLIPEDE", new[] { new LevelUpMove { Move = "MOVE_SIZZLY_SLIDE", Level = 40 } } },
				{ "SPECIES_CENTISKORCH", new[] { new LevelUpMove { Move = "MOVE_SIZZLY_SLIDE", Level = 40 } } },
				{ "SPECIES_PONYTA_GALARIAN", new[] { new LevelUpMove { Move = "MOVE_SPARKLY_SWIRL", Level = 48 } } },
				{ "SPECIES_RAPIDASH_GALARIAN", new[] { new LevelUpMove { Move = "MOVE_SPARKLY_SWIRL", Level = 48 } } },
				{ "SPECIES_CHINCHOU", new[] { new LevelUpMove { Move = "MOVE_SPLISHY_SPLASH", Level = 44 } } },
				{ "SPECIES_LANTURN", new[] { new LevelUpMove { Move = "MOVE_SPLISHY_SPLASH", Level = 44 } } },
				{ "SPECIES_EEVEE", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_VAPOREON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_JOLTEON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_FLAREON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_ESPEON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_UMBREON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_LEAFEON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_GLACEON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_SYLVEON", new[] { new LevelUpMove { Move = "MOVE_VEEVEE_VOLLEY", Level = 50 } } },
				{ "SPECIES_BLITZLE", new[] { new LevelUpMove { Move = "MOVE_ZIPPY_ZAP", Level = 44 } } },
				{ "SPECIES_ZEBSTRIKA", new[] { new LevelUpMove { Move = "MOVE_ZIPPY_ZAP", Level = 44 } } },
			};

			private static readonly Dictionary<string, HashSet<string>> s_DivergenceTutorMoveLearnsets = new Dictionary<string, HashSet<string>>
			{
				{ "MOVE_ALLURING_VOICE", new HashSet<string> { "SPECIES_GOREBYSS" } },
				{ "MOVE_BADDY_BAD", new HashSet<string> { "SPECIES_ABSOL" } },
				{ "MOVE_BOUNCY_BUBBLE", new HashSet<string> { "SPECIES_POPPLIO", "SPECIES_BRIONNE", "SPECIES_PRIMARINA" } },
				{ "MOVE_BUZZY_BUZZ", new HashSet<string> { "SPECIES_MAREEP", "SPECIES_FLAAFFY", "SPECIES_AMPHAROS" } },
				{ "MOVE_FLOATY_FALL", new HashSet<string> { "SPECIES_HOPPIP", "SPECIES_SKIPLOOM", "SPECIES_JUMPLUFF" } },
				{ "MOVE_FREEZY_FROST", new HashSet<string> { "SPECIES_SNORUNT", "SPECIES_FROSLASS" } },
				{ "MOVE_GLITZY_GLOW", new HashSet<string> { "SPECIES_STARYU", "SPECIES_STARMIE" } },
				{ "MOVE_MEGA_PUNCH", new HashSet<string> { "SPECIES_ELECTIVIRE" } },
				{ "MOVE_MISTY_TERRAIN", new HashSet<string> { "SPECIES_GOREBYSS" } },
				{ "MOVE_POLTERGEIST", new HashSet<string> { "SPECIES_PARASECT" } },
				{ "MOVE_SAPPY_SEED", new HashSet<string> { "SPECIES_PHANTUMP", "SPECIES_TREVENANT" } },
				{ "MOVE_SIZZLY_SLIDE", new HashSet<string> { "SPECIES_SALANDIT", "SPECIES_SALAZZLE" } },
				{ "MOVE_SPARKLY_SWIRL", new HashSet<string> { "SPECIES_COMFEY" } },
				{ "MOVE_SPLISHY_SPLASH", new HashSet<string> { "SPECIES_TADBULB", "SPECIES_BELLIBOLT" } },
				{ "MOVE_ZIPPY_ZAP", new HashSet<string> { "SPECIES_VOLTORB", "SPECIES_ELECTRODE" } },
			};

			public string Species;
			public List<LevelUpMove> LevelUpMoves;
			public List<string> TutorMoves;
			public List<PokemonCompetitiveSet> CompetitiveSets;

			public static PokemonProfile FromSource(SourcePokemonProfile sourceProfile)
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
				foreach (var set in CompetitiveSets)
				{
					if (set.Ability == fromAbility)
						set.Ability = toAbility;
				}
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
			}

			private void UpdateLedianCompetitiveSets()
			{
				if (CompetitiveSets.Count < 2)
					throw new InvalidDataException("Ledian is missing the competitive sets required for its Divergence profiles");

				PokemonCompetitiveSet utilitySet = CompetitiveSets[0];
				utilitySet.Item = "ITEM_HEAVY_DUTY_BOOTS";
				utilitySet.Nature = "NATURE_JOLLY";
				utilitySet.Moves = new List<string>
				{
					"MOVE_ROOST",
					"MOVE_U_TURN",
					"MOVE_DRAIN_PUNCH",
					"MOVE_KNOCK_OFF",
				};

				PokemonCompetitiveSet ironFistSet = CompetitiveSets[1];
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
				if (CompetitiveSets.Count < 6)
					throw new InvalidDataException("Huntail is missing the competitive sets required for its Divergence profiles");

				CompetitiveSets[0].Nature = "NATURE_ADAMANT";
				CompetitiveSets[0].Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
				};

				CompetitiveSets[1].Item = "ITEM_LEFTOVERS";
				CompetitiveSets[1].Nature = "NATURE_ADAMANT";
				CompetitiveSets[1].Moves = new List<string>
				{
					"MOVE_COIL",
					"MOVE_AQUA_TAIL",
					"MOVE_CRUNCH",
					"MOVE_DRAGON_TAIL",
				};

				CompetitiveSets[2].Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_SUCKER_PUNCH",
				};

				CompetitiveSets[3].Item = "ITEM_CHOICE_BAND";
				CompetitiveSets[3].Nature = "NATURE_ADAMANT";
				CompetitiveSets[3].HiddenPower = null;
				CompetitiveSets[3].Moves = new List<string>
				{
					"MOVE_AQUA_TAIL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
					"MOVE_DRAGON_TAIL",
				};

				CompetitiveSets[4].Nature = "NATURE_ADAMANT";
				CompetitiveSets[4].Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
				};

				CompetitiveSets[5].Nature = "NATURE_ADAMANT";
				CompetitiveSets[5].Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_WATERFALL",
					"MOVE_CRUNCH",
					"MOVE_ICE_FANG",
				};
			}

			private void UpdateGorebyssCompetitiveSets()
			{
				if (CompetitiveSets.Count < 4)
					throw new InvalidDataException("Gorebyss is missing the competitive sets required for its Divergence profiles");

				CompetitiveSets[0].HiddenPower = null;
				CompetitiveSets[0].Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_HYDRO_PUMP",
					"MOVE_ALLURING_VOICE",
					"MOVE_ICE_BEAM",
				};

				CompetitiveSets[1].Moves = new List<string>
				{
					"MOVE_MISTY_TERRAIN",
					"MOVE_SHELL_SMASH",
					"MOVE_HYDRO_PUMP",
					"MOVE_ALLURING_VOICE",
				};

				CompetitiveSets[2].Moves = new List<string>
				{
					"MOVE_SHELL_SMASH",
					"MOVE_SURF",
					"MOVE_DRAINING_KISS",
					"MOVE_PSYCHIC",
				};

				CompetitiveSets[3].HiddenPower = null;
				CompetitiveSets[3].Moves = new List<string>
				{
					"MOVE_MISTY_TERRAIN",
					"MOVE_SHELL_SMASH",
					"MOVE_HYDRO_PUMP",
					"MOVE_ALLURING_VOICE",
				};
			}

			private void UpdateParasectCompetitiveSets()
			{
				if (CompetitiveSets.Count < 2)
					throw new InvalidDataException("Parasect is missing the competitive sets required for its Divergence profiles");

				CompetitiveSets[0].Moves = new List<string>
				{
					"MOVE_SPORE",
					"MOVE_SYNTHESIS",
					"MOVE_SEED_BOMB",
					"MOVE_POLTERGEIST",
				};

				CompetitiveSets[1].Moves = new List<string>
				{
					"MOVE_SWORDS_DANCE",
					"MOVE_SHADOW_CLAW",
					"MOVE_SEED_BOMB",
					"MOVE_SYNTHESIS",
				};
			}

			private void UpdateVolbeatCompetitiveSets()
			{
				if (CompetitiveSets.Count < 5)
					throw new InvalidDataException("Volbeat is missing the competitive sets required for its Divergence profiles");

				CompetitiveSets[1].Item = "ITEM_LIFE_ORB";
				CompetitiveSets[1].Ability = "ABILITY_SWARM";
				CompetitiveSets[1].Nature = "NATURE_TIMID";
				CompetitiveSets[1].Moves = new List<string>
				{
					"MOVE_TAIL_GLOW",
					"MOVE_THUNDER",
					"MOVE_BUG_BUZZ",
					"MOVE_ROOST",
				};

				CompetitiveSets[4].Item = "ITEM_HEAVY_DUTY_BOOTS";
			}

			private void UpdateFlorgesCompetitiveSets()
			{
				if (CompetitiveSets.Count < 11)
					throw new InvalidDataException("Florges is missing the competitive sets required for its Divergence profiles");

				CompetitiveSets[1].Moves = new List<string>
				{
					"MOVE_CALM_MIND",
					"MOVE_MOONBLAST",
					"MOVE_GIGA_DRAIN",
					"MOVE_SYNTHESIS",
				};

				CompetitiveSets[10].Moves = new List<string>
				{
					"MOVE_CALM_MIND",
					"MOVE_MOONBLAST",
					"MOVE_ENERGY_BALL",
					"MOVE_SYNTHESIS",
				};
			}

			private void UpdateElectivireCompetitiveSets()
			{
				if (CompetitiveSets.Count < 3)
					throw new InvalidDataException("Electivire is missing the competitive sets required for its Divergence profiles");

				CompetitiveSets[2].Moves = new List<string>
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
				foreach (var moveLearnset in s_RestoredMoveLearnsets)
				{
					if (moveLearnset.Value.Contains(Species) && !CanLearnMove(moveLearnset.Key))
						TutorMoves.Add(moveLearnset.Key);
				}

				if (s_DivergenceLevelUpMoveLearnsets.TryGetValue(Species, out LevelUpMove[] levelUpMoves))
				{
					foreach (var move in levelUpMoves)
					{
						if (!HasLevelUpMove(move.Move))
							LevelUpMoves.Add(new LevelUpMove { Move = move.Move, Level = move.Level });
					}
				}

				foreach (var moveLearnset in s_DivergenceTutorMoveLearnsets)
				{
					if (moveLearnset.Value.Contains(Species) && !CanLearnMove(moveLearnset.Key))
						TutorMoves.Add(moveLearnset.Key);
				}

				if (Species == "SPECIES_SCOVILLAIN" && !HasLevelUpMove("MOVE_SPICY_EXTRACT"))
					LevelUpMoves.Add(new LevelUpMove { Move = "MOVE_SPICY_EXTRACT", Level = 0 });

				if ((Species == "SPECIES_PAWMOT" || Species == "SPECIES_RABSCA")
					&& !HasLevelUpMove("MOVE_REVIVAL_BLESSING"))
					LevelUpMoves.Add(new LevelUpMove { Move = "MOVE_REVIVAL_BLESSING", Level = 0 });

				if (Species == "SPECIES_PAWMOT" || Species == "SPECIES_RABSCA")
					// These sets were built to recycle Revival Blessing's PP, which is
					// incompatible with Divergence's once-per-team battle limit.
					CompetitiveSets.RemoveAll(set => set.Item == "ITEM_LEPPA_BERRY");

				if (Species == "SPECIES_DONDOZO" && !HasLevelUpMove("MOVE_ORDER_UP"))
					LevelUpMoves.Add(new LevelUpMove { Move = "MOVE_ORDER_UP", Level = 50 });
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

		private class PokemonCompetitiveSet
		{
			public List<string> Moves = new List<string>();
			public string Ability;
			public string Item;
			public string Nature;
			public string HiddenPower;
			public string TeraType;
			public List<string> SourceTiers = new List<string>();

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

		public static void GatherProfiles()
		{
			List<PokemonProfile> profiles = new List<PokemonProfile>();
			Dictionary<string, string> redirectedSpecies = new Dictionary<string, string>();
			int exceptionHitCount = 0;

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
							// Only redirect species which are functionally identical for rogue spawning
							if (speciesName.EndsWith("_MEGA"))
							{
								redirectSpecies = speciesName.Substring(0, speciesName.Length - "_MEGA".Length);

								switch (speciesName)
								{
									case "SPECIES_PIKIN_MEGA":
										redirectSpecies = "SPECIES_MAREEP";
										break;
								}
							}
							else if (speciesName.EndsWith("_MEGA_X") || speciesName.EndsWith("_MEGA_Y"))
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
							else if (speciesName.StartsWith("SPECIES_FLOETTE_"))
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
						if (!GameDataHelpers.SpeciesDefines.ContainsKey(redirectSpecies))
							throw new InvalidDataException();

						redirectedSpecies[speciesName] = redirectSpecies;
						continue;
					}

					PokemonProfile profile = GatherProfileFor(speciesName);
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
					{
						// Gone over threshold
						throw e;
					}
				}
			}

			ExportProfiles(profiles, redirectedSpecies, Path.Combine(GameDataHelpers.RootDirectory, "src\\data\\rogue_pokemon_profiles.h"));
		}

		private static PokemonProfile GatherProfileFor(string speciesName)
		{
			string manualPath = ContentCache.GetWriteableCachePath($"res://PokemonProfiles//{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
			string cachePath = ContentCache.GetWriteableCachePath($"pokemon_profiles/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json");
			PokemonProfile outputProfile;

			if (File.Exists(manualPath))
			{
				Console.WriteLine($"Found '{speciesName}' profile manual override");

				string jsonProfile = File.ReadAllText(manualPath);
				outputProfile = JsonConvert.DeserializeObject<PokemonProfile>(jsonProfile, c_JsonSettings);
			}
			else if (File.Exists(cachePath))
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
				outputProfile.FormatDataForGame(); // collapse initially so we can easily inspect the cache file

				string cacheDir = Path.GetDirectoryName(cachePath);
				Directory.CreateDirectory(cacheDir);

				string profileJson = JsonConvert.SerializeObject(outputProfile, c_JsonSettings);
				File.WriteAllText(cachePath, profileJson);
			}

			outputProfile.FormatDataForGame();
			outputProfile.ValidateContents();

			return outputProfile;
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

			upperBlock.AppendLine("u16 const gRoguePokemonMoveUsages[MOVES_COUNT] = \n{");

			foreach (var kvp in moveCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			upperBlock.AppendLine("u16 const gRoguePokemonSpecialMoveUsages[MOVES_COUNT] = \n{");

			foreach (var kvp in specialMoveCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			upperBlock.AppendLine("u16 const gRoguePokemonHeldItemUsages[ITEMS_COUNT] = \n{");

			foreach (var kvp in heldItemCount)
				upperBlock.AppendLine($"\t[{kvp.Key}] = {kvp.Value},");

			upperBlock.AppendLine("};\n");

			// Pokemon Profiles
			//
			lowerBlock.AppendLine("struct RoguePokemonProfile const gRoguePokemonProfiles[NUM_SPECIES] = \n{");

			foreach(var profile in profiles)
			{
				// Mon flags
				HashSet<string> sourceTiers = new HashSet<string>();

				foreach (var compSet in profile.CompetitiveSets)
				{
					foreach (var tier in compSet.SourceTiers)
						sourceTiers.Add(GameDataHelpers.FormatKeyword(tier));
				}

				upperBlock.AppendLine($"#ifdef APPEND_MON_FLAGS_{profile.Species}");

				upperBlock.Append($"#define MON_FLAGS_{profile.Species} (APPEND_MON_FLAGS_{profile.Species}"); // allow easily appending flags
				foreach (var tier in sourceTiers)
					upperBlock.Append($" | MON_FLAGS_{tier}");
				upperBlock.AppendLine(")");

				upperBlock.AppendLine("#else");

				upperBlock.Append($"#define MON_FLAGS_{profile.Species} (0");
				foreach (var tier in sourceTiers)
					upperBlock.Append($" | MON_FLAGS_{tier}");
				upperBlock.AppendLine(")");

				upperBlock.AppendLine("#endif");
				upperBlock.AppendLine("");

				// Level moves
				upperBlock.AppendLine($"static struct LevelUpMove const sLevelUpMoves_{profile.Species}[] = \n{{");
				foreach(var move in profile.LevelUpMoves)
				{
					upperBlock.AppendLine($"\t{{ .move={move.Move}, .level={move.Level} }},");
				}
				upperBlock.AppendLine($"\t{{ .move=MOVE_NONE, .level=0 }},");
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();

				// Tutor moves
				upperBlock.AppendLine($"static u16 const sTutorMoves_{profile.Species}[] = \n{{");
				foreach (var move in profile.TutorMoves)
				{
					upperBlock.AppendLine($"\t{move},");
				}
				upperBlock.AppendLine($"\tMOVE_NONE,");
				upperBlock.AppendLine($"}};");
				upperBlock.AppendLine();

				// Comp sets
				upperBlock.AppendLine($"static struct RoguePokemonCompetitiveSet const sCompetitiveSets_{profile.Species}[] = \n{{");
				foreach(var compSet in profile.CompetitiveSets)
				{
					upperBlock.AppendLine($"\t{{");

					upperBlock.Append($"\t\t.flags= (0");
					foreach (var tier in compSet.SourceTiers)
						upperBlock.Append($" | MON_FLAGS_{tier}");
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
				lowerBlock.AppendLine($"\t[{profile.Species}] = \n\t{{");
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
				lowerBlock.AppendLine($"\t[{kvp.Key}] = \n\t{{");
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
