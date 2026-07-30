using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PokemonDataGenerator.Utils
{
	public static class PokeAPI
	{
		private static readonly string s_PokeApiAddress = "https://pokeapi.co/api/v2/";
		private static readonly string s_ShowdownApiAddress = "https://play.pokemonshowdown.com/data/sets/";
		private static readonly string s_ShowdownPokedexAddress = "https://play.pokemonshowdown.com/data/pokedex.json";
		private static Dictionary<string, string> s_PokeApiBaseAddresses = null;

		private static Dictionary<string, string> s_CachedPokedexURIs = null;
		private static Dictionary<string, string> s_CachedPokemonURIs = null;
		private static Dictionary<string, string> s_CachedPokemonSpeciesURIs = null;
		private static readonly Dictionary<string, JObject> s_CachedShowdownSets = new Dictionary<string, JObject>();
		private static JObject s_CachedShowdownPokedex;

		static PokeAPI()
		{
			s_PokeApiBaseAddresses = new Dictionary<string, string>();
			JObject result = ContentCache.GetJsonContent(s_PokeApiAddress);

			foreach(var kvp in result)
			{
				s_PokeApiBaseAddresses[kvp.Key] = kvp.Value.Value<string>();
			}
		}

		private static JArray GetArrayInternal(string uri)
		{
			JObject result = ContentCache.GetJsonContent(uri);
			JArray output = new JArray();

			while(true)
			{
				foreach (var token in result["results"])
					output.Add(token);

				if(result.ContainsKey("next") && result["next"].Value<string>() != null)
					result = ContentCache.GetJsonContent(result["next"].Value<string>());
				else
					break;
			}

			return output;
		}

		private static Dictionary<string, string> GetResultsAsTableInternal(string uri)
		{
			Dictionary<string, string> output = new Dictionary<string, string>();
			JArray result = GetArrayInternal(uri);

			foreach(var entry in result)
			{
				output[entry["name"].Value<string>()] = entry["url"].Value<string>();
			}

			return output;
		}

		public static Dictionary<string, string> GetPokedexURIs()
		{
			if(s_CachedPokedexURIs == null)
			{
				s_CachedPokedexURIs = GetResultsAsTableInternal(s_PokeApiBaseAddresses["pokedex"]);
			}

			return s_CachedPokedexURIs;
		}

		public static Dictionary<string, string> GetPokemonURIs()
		{
			if (s_CachedPokemonURIs == null)
			{
				Dictionary<string, string> unformatedTable = GetResultsAsTableInternal(s_PokeApiBaseAddresses["pokemon"]);
				Dictionary<string, string> outputTable = new Dictionary<string, string>();

				foreach (var kvp in unformatedTable)
				{
					string speciesName = ApiNameToSpeciesName(kvp.Key);
					outputTable[speciesName] = kvp.Value;
				}

				s_CachedPokemonURIs = outputTable;
			}

			return s_CachedPokemonURIs;
		}

		public static Dictionary<string, string> GetPokemonSpeciesURIs()
		{
			if (s_CachedPokemonSpeciesURIs == null)
			{
				Dictionary<string, string> unformatedTable = GetResultsAsTableInternal(s_PokeApiBaseAddresses["pokemon-species"]);
				Dictionary<string, string> outputTable = new Dictionary<string, string>();

				foreach (var kvp in unformatedTable)
				{
					string speciesName = ApiNameToSpeciesName(kvp.Key);
					outputTable[speciesName] = kvp.Value;
				}

				s_CachedPokemonSpeciesURIs = outputTable;
			}

			return s_CachedPokemonSpeciesURIs;
		}

		public static JObject GetPokemonEntry(string speciesName)
		{
			string apiName = SpeciesNameToApiName(speciesName);
			return ContentCache.GetJsonContent(s_PokeApiAddress + "/pokemon/" + apiName);
		}

		public static JObject GetPokemonSpeciesEntry(string speciesName)
		{
			string overridePath = $"res://ManualPokeAPI/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json";

			if(ContentCache.ExistsInCache(overridePath))
			{
				return ContentCache.GetJsonContent(overridePath);
			}

			var uriLookup = GetPokemonSpeciesURIs();
			RedirectSpeciesLookupName(speciesName, out string lookupSpecies, out string lookupVariant);

			JObject speciesEntry = ContentCache.GetJsonContent(uriLookup[lookupSpecies]);

			foreach(var variant in speciesEntry["varieties"])
			{
				var variantEntry = variant["pokemon"];
				if(variantEntry["name"].Value<string>() == lookupVariant)
				{
					return ContentCache.GetJsonContent(variantEntry["url"].Value<string>());
				}
			}

			if (speciesName == lookupSpecies)
			{
				// Failed to find the form so just assume we mean the default
				foreach (var variant in speciesEntry["varieties"])
				{
					var variantEntry = variant["pokemon"];
					if (variant["is_default"].Value<bool>())
					{
						return ContentCache.GetJsonContent(variantEntry["url"].Value<string>());
					}
				}
			}

			Console.WriteLine($"Unable to find matching variety for '{speciesName}' -> '{lookupSpecies}':'{lookupVariant}'\nAvailable:");
			foreach (var variant in speciesEntry["varieties"])
			{
				var variantEntry = variant["pokemon"];
				Console.WriteLine("\t" + variantEntry["name"].Value<string>());
			}

			throw new InvalidOperationException("API species error");
		}

		public static void PrefetchPokemonProfiles(IEnumerable<string> speciesNames)
		{
			Dictionary<string, string> speciesUris = GetPokemonSpeciesURIs();
			List<Tuple<string, string, string>> lookups = new List<Tuple<string, string, string>>();
			foreach (string speciesName in speciesNames.Distinct())
			{
				string overridePath = $"res://ManualPokeAPI/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json";
				if (ContentCache.ExistsInCache(overridePath))
					continue;
				RedirectSpeciesLookupName(speciesName, out string apiSpecies, out string variantName);
				if (speciesUris.ContainsKey(apiSpecies))
					lookups.Add(Tuple.Create(apiSpecies, variantName, speciesUris[apiSpecies]));
			}

			ContentCache.PrefetchHttpContent(lookups.Select(lookup => lookup.Item3));
			List<string> formUris = new List<string>();
			foreach (var lookup in lookups)
			{
				JObject speciesEntry = ContentCache.GetJsonContent(lookup.Item3);
				foreach (var variant in speciesEntry["varieties"])
				{
					if (variant["pokemon"]["name"].Value<string>() == lookup.Item2)
					{
						formUris.Add(variant["pokemon"]["url"].Value<string>());
						break;
					}
				}
			}
			ContentCache.PrefetchHttpContent(formUris);
		}

		public static bool HasPokemonForm(string speciesName)
		{
			string overridePath = $"res://ManualPokeAPI/{(GameDataHelpers.IsVanillaVersion ? "Vanilla" : "EX")}/{speciesName}.json";
			if (ContentCache.ExistsInCache(overridePath))
				return true;
			Dictionary<string, string> speciesUris = GetPokemonSpeciesURIs();
			RedirectSpeciesLookupName(speciesName, out string apiSpecies, out string variantName);
			if (!speciesUris.TryGetValue(apiSpecies, out string speciesUri))
				return false;
			JObject speciesEntry = ContentCache.GetJsonContent(speciesUri);
			return speciesEntry["varieties"].Any(variant => variant["pokemon"]["name"].Value<string>() == variantName);
		}

		private static string SpeciesNameToApiName(string speciesName)
		{
			if (speciesName.StartsWith("SPECIES_"))
				speciesName = speciesName.Substring("SPECIES_".Length);

			return speciesName.ToLower().Replace("_", "-");
		}

		private static string ApiNameToSpeciesName(string apiName)
		{
			return "SPECIES_" + GameDataHelpers.FormatKeyword(apiName);
		}

		private static string NormalizeSpeciesIdentifier(string name)
		{
			if (name.StartsWith("SPECIES_", StringComparison.CurrentCultureIgnoreCase))
				name = name.Substring("SPECIES_".Length);
			return new string(name.Where(char.IsLetterOrDigit).Select(char.ToLowerInvariant).ToArray());
		}

		public static string NormalizeIdentifier(string name)
		{
			return NormalizeSpeciesIdentifier(name);
		}

		private static JObject GetShowdownSetDocument(string generation)
		{
			if (!s_CachedShowdownSets.TryGetValue(generation, out JObject document))
			{
				document = ContentCache.GetJsonContent(s_ShowdownApiAddress + generation + ".json");
				s_CachedShowdownSets[generation] = document;
			}
			return document;
		}

		public static bool TryGetShowdownGen9MegaBaseSpecies(string speciesName, out string baseSpecies)
		{
			if (s_CachedShowdownPokedex == null)
				s_CachedShowdownPokedex = ContentCache.GetJsonContent(s_ShowdownPokedexAddress);

			string normalizedSpecies = NormalizeSpeciesIdentifier(speciesName);
			foreach (var entry in s_CachedShowdownPokedex)
			{
				if (NormalizeSpeciesIdentifier(entry.Key) != normalizedSpecies)
					continue;

				JObject data = entry.Value.Value<JObject>();
				string forme = data["forme"]?.Value<string>();
				string rawBaseSpecies = data["baseSpecies"]?.Value<string>();
				string nonstandard = data["isNonstandard"]?.Value<string>();
				if (nonstandard != "Future"
					|| string.IsNullOrEmpty(forme)
					|| !forme.Contains("Mega")
					|| string.IsNullOrEmpty(rawBaseSpecies))
					break;

				baseSpecies = ApiNameToSpeciesName(rawBaseSpecies);
				return true;
			}

			baseSpecies = null;
			return false;
		}

		public static bool HasShowdownCompetitiveSets(string speciesName)
		{
			if (s_CompetitiveSetsBySpecies == null
				|| s_CompetitiveSetIndexIsVanilla != GameDataHelpers.IsVanillaVersion)
				BuildCompetitiveSetIndex();
			return s_CompetitiveSetsBySpecies.TryGetValue(speciesName, out JObject sets)
				&& sets.Properties().Any(property => property.Value is JArray tierSets && tierSets.Any());
		}

		public static void RedirectSpeciesLookupName(string speciesName, out string apiSpecies, out string variantName)
		{
			if (GameDataHelpers.IsVanillaVersion)
			{
				if (speciesName.StartsWith("SPECIES_UNOWN"))
					speciesName = "SPECIES_UNOWN";
			}

			apiSpecies = speciesName;
			variantName = SpeciesNameToApiName(speciesName);

			if (!GameDataHelpers.IsVanillaVersion
				&& TryGetShowdownGen9MegaBaseSpecies(speciesName, out string gen9MegaBaseSpecies))
			{
				apiSpecies = gen9MegaBaseSpecies;
				if (speciesName == "SPECIES_MEOWSTIC_M_MEGA")
					variantName = "meowstic-male-mega";
				else if (speciesName == "SPECIES_MEOWSTIC_F_MEGA")
					variantName = "meowstic-female-mega";
				return;
			}

			if(speciesName.StartsWith("SPECIES_DEOXYS"))
			{
				apiSpecies = "SPECIES_DEOXYS";

				if (speciesName == "SPECIES_DEOXYS")
					variantName = "deoxys-normal";
			}


			if (!GameDataHelpers.IsVanillaVersion)
			{
				if (speciesName.EndsWith("_ALOLAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_ALOLAN".Length);
					variantName = variantName.Replace("-alolan", "-alola");
				}
				else if (speciesName.EndsWith("_GALARIAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_GALARIAN".Length);
					variantName = variantName.Replace("-galarian", "-galar");
				}
				else if (speciesName.EndsWith("_HISUIAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_HISUIAN".Length);
					variantName = variantName.Replace("-hisuian", "-hisui");
				}
				else if (speciesName.EndsWith("_PALDEAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_HISUIAN".Length);
					variantName = variantName.Replace("-paldean", "-paldea");
				}
				else if (speciesName.EndsWith("_ORIGIN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_ORIGIN".Length);
				}
				else if (speciesName.EndsWith("_THERIAN"))
				{
					apiSpecies = apiSpecies.Substring(0, apiSpecies.Length - "_THERIAN".Length);
				}
				else if (speciesName.StartsWith("SPECIES_PIKACHU_"))
				{
					apiSpecies = "SPECIES_PIKACHU";
				}
				else if (speciesName.StartsWith("SPECIES_ROTOM_"))
				{
					apiSpecies = "SPECIES_ROTOM";
				}
				else if (speciesName.StartsWith("SPECIES_SHAYMIN_"))
				{
					apiSpecies = "SPECIES_SHAYMIN";
				}
				else if (speciesName.StartsWith("SPECIES_BASCULIN_"))
				{
					apiSpecies = "SPECIES_BASCULIN";
				}
				else if (speciesName.StartsWith("SPECIES_GRENINJA_"))
				{
					apiSpecies = "SPECIES_GRENINJA";
				}
				else if (speciesName.StartsWith("SPECIES_ZYGARDE_"))
				{
					apiSpecies = "SPECIES_ZYGARDE";
				}
				else if (speciesName.StartsWith("SPECIES_ORICORIO_"))
				{
					apiSpecies = "SPECIES_ORICORIO";
				}
				else if (speciesName.StartsWith("SPECIES_LYCANROC_"))
				{
					apiSpecies = "SPECIES_LYCANROC";
				}
				else if (speciesName.StartsWith("SPECIES_CALYREX_"))
				{
					apiSpecies = "SPECIES_CALYREX";
					variantName = variantName.Replace("-rider", "");
				}

				switch (speciesName)
				{
					case "SPECIES_DARMANITAN_GALARIAN":
						apiSpecies = "SPECIES_DARMANITAN";
						variantName = "darmanitan-galar-standard";
						break;

					case "SPECIES_DARMANITAN_GALARIAN_ZEN_MODE":
						apiSpecies = "SPECIES_DARMANITAN";
						variantName = "darmanitan-galar-zen";
						break;

					case "SPECIES_PIKACHU_PH_D":
						apiSpecies = "SPECIES_PIKACHU";
						variantName = "pikachu-phd";
						break;

					case "SPECIES_PICHU_SPIKY_EARED":
						apiSpecies = "SPECIES_PICHU";
						variantName = "pichu";
						break;

						//case "SPECIES_WORMADAM":
						//	apiSpecies = "SPECIES_WORMADAM";
						//	variantName = "wormadam-plant";
						//	break;
						//
					case "SPECIES_WORMADAM_SANDY_CLOAK":
						apiSpecies = "SPECIES_WORMADAM";
						variantName = "wormadam-sandy";
						break;
					
					case "SPECIES_WORMADAM_TRASH_CLOAK":
						apiSpecies = "SPECIES_WORMADAM";
						variantName = "wormadam-trash";
						break;

					case "SPECIES_KYUREM_WHITE":
					case "SPECIES_KYUREM_BLACK":
						apiSpecies = "SPECIES_KYUREM";
						break;

					case "SPECIES_MEOWSTIC_FEMALE":
						apiSpecies = "SPECIES_MEOWSTIC";
						variantName = "meowstic-female";
						break;

					case "SPECIES_HOOPA_UNBOUND":
						apiSpecies = "SPECIES_HOOPA";
						break;

					case "SPECIES_ROCKRUFF_OWN_TEMPO":
						apiSpecies = "SPECIES_ROCKRUFF";
						break;

					case "SPECIES_NECROZMA_DUSK_MANE":
						apiSpecies = "SPECIES_NECROZMA";
						variantName = "necrozma-dusk";
						break;
					case "SPECIES_NECROZMA_DAWN_WINGS":
						apiSpecies = "SPECIES_NECROZMA";
						variantName = "necrozma-dawn";
						break;
					case "SPECIES_NECROZMA_ULTRA":
						apiSpecies = "SPECIES_NECROZMA";
						variantName = "necrozma-ultra";
						break;


					case "SPECIES_TOXTRICITY_LOW_KEY":
						apiSpecies = "SPECIES_TOXTRICITY";
						break;

					case "SPECIES_INDEEDEE_FEMALE":
						apiSpecies = "SPECIES_INDEEDEE";
						variantName = "indeedee-female";
						break;

					case "SPECIES_FLOETTE_ETERNAL_FLOWER":
						apiSpecies = "SPECIES_FLOETTE";
						variantName = "floette-eternal";
						break;

					case "SPECIES_ZACIAN_CROWNED_SWORD":
						apiSpecies = "SPECIES_ZACIAN";
						variantName = "zacian-crowned";
						break;
					case "SPECIES_ZAMAZENTA_CROWNED_SHIELD":
						apiSpecies = "SPECIES_ZAMAZENTA";
						variantName = "zamazenta-crowned";
						break;

					case "SPECIES_URSHIFU_RAPID_STRIKE_STYLE":
						apiSpecies = "SPECIES_URSHIFU";
						variantName = "urshifu-rapid-strike";
						break;

					case "SPECIES_BASCULEGION_FEMALE":
						apiSpecies = "SPECIES_BASCULEGION";
						break;

					case "SPECIES_OINKOLOGNE_FEMALE":
						apiSpecies = "SPECIES_OINKOLOGNE";
						variantName = "oinkologne-female";
						break;

					case "SPECIES_PALAFIN_HERO":
						apiSpecies = "SPECIES_PALAFIN";
						variantName = "palafin-hero";
						break;

					case "SPECIES_MAUSHOLD":
						apiSpecies = "SPECIES_MAUSHOLD";
						variantName = "maushold-family-of-three";
						break;

					case "SPECIES_MAUSHOLD_FAMILY_OF_FOUR":
						apiSpecies = "SPECIES_MAUSHOLD";
						variantName = "maushold-family-of-four";
						break;

					case "SPECIES_TATSUGIRI_DROOPY":
						apiSpecies = "SPECIES_TATSUGIRI";
						variantName = "tatsugiri-droopy";
						break;

					case "SPECIES_TATSUGIRI_STRETCHY":
						apiSpecies = "SPECIES_TATSUGIRI";
						variantName = "tatsugiri-stretchy";
						break;

					case "SPECIES_DUDUNSPARCE_THREE_SEGMENT":
						apiSpecies = "SPECIES_DUDUNSPARCE";
						variantName = "dudunsparce-three-segment";
						break;

					case "SPECIES_GIMMIGHOUL_ROAMING":
						apiSpecies = "SPECIES_GIMMIGHOUL";
						variantName = "gimmighoul-roaming";
						break;

					case "SPECIES_TAUROS_PALDEAN_COMBAT_BREED":
						apiSpecies = "SPECIES_TAUROS";
						variantName = "tauros-paldea-combat-breed";
						break;

					case "SPECIES_TAUROS_PALDEAN_BLAZE_BREED":
						apiSpecies = "SPECIES_TAUROS";
						variantName = "tauros-paldea-blaze-breed";
						break;

					case "SPECIES_TAUROS_PALDEAN_AQUA_BREED":
						apiSpecies = "SPECIES_TAUROS";
						variantName = "tauros-paldea-aqua-breed";
						break;

					case "SPECIES_POLTCHAGEIST_ARTISAN":
						apiSpecies = "SPECIES_POLTCHAGEIST";
						variantName = "poltchageist";
						break;

					case "SPECIES_SINISTCHA_MASTERPIECE":
						apiSpecies = "SPECIES_SINISTCHA";
						variantName = "sinistcha";
						break;

					case "SPECIES_OGERPON_WELLSPRING_MASK":
						apiSpecies = "SPECIES_OGERPON";
						variantName = "ogerpon-wellspring-mask";
						break;

					case "SPECIES_OGERPON_HEARTHFLAME_MASK":
						apiSpecies = "SPECIES_OGERPON";
						variantName = "ogerpon-hearthflame-mask";
						break;

					case "SPECIES_OGERPON_CORNERSTONE_MASK":
						apiSpecies = "SPECIES_OGERPON";
						variantName = "ogerpon-cornerstone-mask";
						break;

					case "SPECIES_URSALUNA_BLOODMOON":
						apiSpecies = "SPECIES_URSALUNA";
						variantName = "ursaluna-bloodmoon";
						break;

						//
						//case "SPECIES_GIRATINA":
						//	apiSpecies = "SPECIES_GIRATINA";
						//	variantName = "giratina-altered";
						//	break;
						//
						//case "SPECIES_SHAYMIN":
						//	apiSpecies = "SPECIES_SHAYMIN";
						//	variantName = "shaymin-land";
						//	break;
						//
						//case "SPECIES_BASCULIN":
						//	apiSpecies = "SPECIES_BASCULIN";
						//	variantName = "basculin-red-striped";
						//	break;
						//
						//case "SPECIES_DARMANITAN":
						//	apiSpecies = "SPECIES_DARMANITAN";
						//	variantName = "darmanitan-standard";
						//	break;
						//
						//case "SPECIES_TORNADUS":
						//	apiSpecies = "SPECIES_TORNADUS";
						//	variantName = "tornadus-incarnate";
						//	break;
						//case "SPECIES_THUNDURUS":
						//	apiSpecies = "SPECIES_THUNDURUS";
						//	variantName = "thundurus-incarnate";
						//	break;
						//case "SPECIES_LANDORUS":
						//	apiSpecies = "SPECIES_LANDORUS";
						//	variantName = "landorus-incarnate";
						//	break;
				}
			}
		}

		private static Dictionary<string, JObject> s_CompetitiveSetsBySpecies;
		private static bool s_CompetitiveSetIndexIsVanilla;

		public static JObject GetPokemonSpeciesCompetitiveSets(string speciesName)
		{
			if (s_CompetitiveSetsBySpecies == null || s_CompetitiveSetIndexIsVanilla != GameDataHelpers.IsVanillaVersion)
				BuildCompetitiveSetIndex();
			return s_CompetitiveSetsBySpecies.TryGetValue(speciesName, out JObject sets)
				? sets
				: new JObject();
		}

		private static void BuildCompetitiveSetIndex()
		{
			s_CompetitiveSetIndexIsVanilla = GameDataHelpers.IsVanillaVersion;
			s_CompetitiveSetsBySpecies = new Dictionary<string, JObject>();
			string[] lookupGens = GameDataHelpers.IsVanillaVersion
				? new[] { "gen3" }
				: new[] { "gen6", "gen7", "gen8", "gen9" };

			foreach (string lookupGen in lookupGens)
			{
				foreach (var compTierKvp in GetShowdownSetDocument(lookupGen))
				{
					if (compTierKvp.Key.EndsWith("hackmons", StringComparison.CurrentCultureIgnoreCase)
						|| compTierKvp.Key.EndsWith("cap", StringComparison.CurrentCultureIgnoreCase)
						|| compTierKvp.Key.EndsWith("anyability", StringComparison.CurrentCultureIgnoreCase))
						continue;

					foreach (var upperGroupingKvp in compTierKvp.Value.Value<JObject>())
					{
						foreach (var speciesSetGroupKvp in upperGroupingKvp.Value.Value<JObject>())
						{
							if (IsSpeciesIgnored(speciesSetGroupKvp.Key))
								continue;
							VerifyCompetitiveSpecies(speciesSetGroupKvp.Key);
							string projectSpecies = CompetitiveApiNameToSpeciesName(speciesSetGroupKvp.Key);
							if (!s_CompetitiveSetsBySpecies.TryGetValue(projectSpecies, out JObject speciesSets))
							{
								speciesSets = new JObject();
								s_CompetitiveSetsBySpecies[projectSpecies] = speciesSets;
							}
							if (!(speciesSets[compTierKvp.Key] is JArray tierOutput))
							{
								tierOutput = new JArray();
								speciesSets[compTierKvp.Key] = tierOutput;
							}

							foreach (var currentSetKvp in speciesSetGroupKvp.Value.Value<JObject>())
							{
								var currentSet = (JObject)currentSetKvp.Value.DeepClone();
								currentSet["_profileSourceId"] = string.Join(
									"|",
									lookupGen,
									compTierKvp.Key,
									speciesSetGroupKvp.Key,
									currentSetKvp.Key);
								tierOutput.Add(currentSet);
							}
						}
					}
				}
			}
		}

		private static void VerifyCompetitiveSpecies(string apiName)
		{
			// Dud method to verify that we're properly supporting all of the cases we need to be
			string speciesName = CompetitiveApiNameToSpeciesName(apiName);

			if (GameDataHelpers.IsVanillaVersion)
			{
				switch (apiName)
				{
					case "Deoxys-Speed":
					case "Deoxys-Defense":
					case "Deoxys-Attack":
						return;
				}
			}
			else
			{
				switch (apiName)
				{
					// Ignored
					case "Eevee-Starter":
						return;
				}
			}

			if (!GameDataHelpers.SpeciesDefines.ContainsKey(speciesName))
				throw new NotImplementedException($"Showdown species '{apiName}' maps to unsupported project species '{speciesName}'.");
		}

		private static string CompetitiveApiNameToSpeciesName(string apiName)
		{
			if (apiName.EndsWith("-Gmax", StringComparison.CurrentCultureIgnoreCase))
				apiName = apiName.Substring(0, apiName.Length - "-Gmax".Length) + "-Gigantamax";
			if (apiName == "Urshifu-Gigantamax")
				apiName = "Urshifu-Single-Strike-Style-Gigantamax";

			if (!GameDataHelpers.IsVanillaVersion)
			{
				string directSpeciesName = "SPECIES_" + GameDataHelpers.FormatKeyword(apiName);
				if (GameDataHelpers.SpeciesDefines.ContainsKey(directSpeciesName))
					return ResolveCompetitiveProfileOwner(directSpeciesName);
			}

			if (apiName.EndsWith("-mega", StringComparison.CurrentCultureIgnoreCase))
			{
				apiName = apiName.Substring(0, apiName.Length - "-mega".Length);
			}
			else if (apiName.EndsWith("-mega-x", StringComparison.CurrentCultureIgnoreCase) || apiName.EndsWith("-mega-y", StringComparison.CurrentCultureIgnoreCase))
			{
				apiName = apiName.Substring(0, apiName.Length - "-mega-x".Length);
			}
			else if (apiName.EndsWith("-primal", StringComparison.CurrentCultureIgnoreCase))
			{
				apiName = apiName.Substring(0, apiName.Length - "-primal".Length);
			}

			if (!GameDataHelpers.IsVanillaVersion)
			{
				if(apiName.EndsWith("-Alola"))
					apiName = apiName.Replace("-Alola", "-Alolan");

				if (apiName.EndsWith("-Galar"))
					apiName = apiName.Replace("-Galar", "-Galarian");

				if (apiName.EndsWith("-Hisui"))
					apiName = apiName.Replace("-Hisui", "-Hisuian");

				if (apiName.EndsWith("-Paldea"))
					apiName = apiName.Replace("-Paldea", "-Paldean");

				switch (apiName)
				{
					case "Darmanitan-Zen":
						apiName = "Darmanitan-Zen-mode";
						break;
					case "Darmanitan-Galar-Zen":
						apiName = "Darmanitan-Galarian-Zen-mode";
						break;

					case "Wormadam-Sandy":
						apiName = "Wormadam-Sandy-Cloak";
						break;
					case "Wormadam-Trash":
						apiName = "Wormadam-Trash-Cloak";
						break;

					case "Meowstic-F":
						apiName = "Meowstic-Female";
						break;

					case "Indeedee-F":
						apiName = "Indeedee-Female";
						break;

					case "Calyrex-Ice":
						apiName = "Calyrex-Ice-Rider";
						break;

					case "Calyrex-Shadow":
						apiName = "Calyrex-Shadow-Rider";
						break;

					case "Urshifu-Rapid-Strike":
						apiName = "Urshifu-Rapid-Strike-Style";
						break;

					case "Basculegion-F":
						apiName = "Basculegion-Female";
						break;

					case "Oinkologne-F":
						apiName = "Oinkologne-Female";
						break;

					case "Ogerpon-Wellspring":
						apiName = "Ogerpon-Wellspring-Mask";
						break;

					case "Ogerpon-Hearthflame":
						apiName = "Ogerpon-Hearthflame-Mask";
						break;

					case "Ogerpon-Cornerstone":
						apiName = "Ogerpon-Cornerstone-Mask";
						break;

					case "Tauros-Paldea-Combat":
						apiName = "Tauros-Paldean-Combat-Breed";
						break;

					case "Tauros-Paldea-Blaze":
						apiName = "Tauros-Paldean-Blaze-Breed";
						break;

					case "Tauros-Paldea-Aqua":
						apiName = "Tauros-Paldean-Aqua-Breed";
						break;

					case "Maushold-Four":
						apiName = "Maushold-Family-Of-Four";
						break;

					case "Squawkabilly-Green":
					case "Squawkabilly-Blue":
					case "Squawkabilly-Yellow":
					case "Squawkabilly-White":
						apiName += "-Plumage";
						break;
				}
			}


			return ResolveCompetitiveProfileOwner("SPECIES_" + GameDataHelpers.FormatKeyword(apiName));
		}

		private static string ResolveCompetitiveProfileOwner(string speciesName)
		{
			if (GameDataHelpers.IsVanillaVersion)
				return speciesName;

			// Z-A Mega forms are upstream-owned forms. Keep their sets attached to
			// the form so a future dedicated set promotes it to an independent
			// profile instead of folding it into the base species.
			if (TryGetShowdownGen9MegaBaseSpecies(speciesName, out string ignoredBaseSpecies))
				return speciesName;

			if (speciesName.EndsWith("_MEGA_X") || speciesName.EndsWith("_MEGA_Y"))
				return speciesName.Substring(0, speciesName.Length - "_MEGA_X".Length);
			if (speciesName.EndsWith("_MEGA"))
				return speciesName.Substring(0, speciesName.Length - "_MEGA".Length);
			if (speciesName.EndsWith("_PRIMAL"))
				return speciesName.Substring(0, speciesName.Length - "_PRIMAL".Length);
			if (speciesName.EndsWith("_GIGANTAMAX"))
			{
				switch (speciesName)
				{
					case "SPECIES_TOXTRICITY_AMPED_GIGANTAMAX":
						return "SPECIES_TOXTRICITY";
					case "SPECIES_URSHIFU_SINGLE_STRIKE_STYLE_GIGANTAMAX":
						return "SPECIES_URSHIFU";
					default:
						return speciesName.Substring(0, speciesName.Length - "_GIGANTAMAX".Length);
				}
			}

			if (speciesName.StartsWith("SPECIES_ARCEUS_"))
				return "SPECIES_ARCEUS";
			if (speciesName.StartsWith("SPECIES_SILVALLY_"))
				return "SPECIES_SILVALLY";
			if (speciesName.StartsWith("SPECIES_PUMPKABOO_"))
				return "SPECIES_PUMPKABOO";
			if (speciesName.StartsWith("SPECIES_GOURGEIST_"))
				return "SPECIES_GOURGEIST";

			switch (speciesName)
			{
				case "SPECIES_DARMANITAN_ZEN_MODE":
					return "SPECIES_DARMANITAN";
				case "SPECIES_DARMANITAN_ZEN_MODE_GALARIAN":
					return "SPECIES_DARMANITAN_GALARIAN";
				case "SPECIES_MELOETTA_PIROUETTE":
					return "SPECIES_MELOETTA";
				case "SPECIES_ZARUDE_DADA":
					return "SPECIES_ZARUDE";
				default:
					return speciesName;
			}
		}

		private static bool IsSpeciesIgnored(string apiName)
		{
			//string speciesName = CompetitiveApiNameToSpeciesName(apiName);
			//
			//switch(speciesName)
			//{
			//	// Currently not implemented species
			//	case "SPECIES_ARCHALUDON":
			//	case "SPECIES_HYDRAPPLE":
			//	case "SPECIES_GOUGING_FIRE":
			//	case "SPECIES_RAGING_BOLT":
			//	case "SPECIES_IRON_BOULDER":
			//	case "SPECIES_IRON_CROWN":
			//		return true;
			//}

			return false;
		}
	}
}
