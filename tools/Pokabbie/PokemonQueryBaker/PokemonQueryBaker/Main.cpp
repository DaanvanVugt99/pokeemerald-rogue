#include "BakeHelpers.h"

#include <string>
#include <fstream>
#include <array>
#include <vector>
#include <set>

extern "C"
{
	#include "rogue_baked.h"

#ifdef ROGUE_EXPANSION
	extern const struct SpeciesInfo gSpeciesInfo[];
#else
	extern const struct BaseStats gBaseStats[];
#endif

	extern const struct RoguePokedexVariant gPokedexVariants[POKEDEX_VARIANT_COUNT];
	extern const struct RoguePokedexRegion gPokedexRegions[POKEDEX_REGION_COUNT];
}

u16 eggLookup[NUM_SPECIES]{ SPECIES_NONE };
u8 evolutionCountLookup[NUM_SPECIES]{ 0 };

std::set<u16> eggEvolutionTypes[NUM_SPECIES];

static bool HasEvolutionConnectionOfType(u16 species, u16 type)
{
	u16 eggSpecies = eggLookup[species];
	auto types = eggEvolutionTypes[eggSpecies];

	return types.find(type) != types.end();
}

static void SetBitFlag(u16 elem, bool state, u8* arr)
{
	u16 idx = elem / 8;
	u16 bit = elem % 8;

	u8 bitMask = 1 << bit;

	if (state)
	{
		arr[idx] |= bitMask;
	}
	else
	{
		arr[idx] &= ~bitMask;
	}
}

static bool GetBitFlag(u16 elem, u8* arr)
{
	u16 idx = elem / 8;
	u16 bit = elem % 8;

	u8 bitMask = 1 << bit;

	return (arr[idx] & bitMask) != 0;
}

#ifdef ROGUE_EXPANSION
static u16 sRegionalEvolutionPathMasks[NUM_SPECIES]{ 0 };

enum
{
	REGIONAL_FORM_NONE = 0,
	REGIONAL_FORM_ALOLAN = 1 << 0,
	REGIONAL_FORM_GALARIAN = 1 << 1,
	REGIONAL_FORM_HISUIAN = 1 << 2,
	REGIONAL_FORM_PALDEAN = 1 << 3,
	REGIONAL_FORM_ALL = REGIONAL_FORM_ALOLAN
		| REGIONAL_FORM_GALARIAN
		| REGIONAL_FORM_HISUIAN
		| REGIONAL_FORM_PALDEAN,
};

static bool VariantContainsSpecies(u8 variant, u16 species)
{
	for (int i = 0; i < gPokedexVariants[variant].speciesCount; ++i)
	{
		if (gPokedexVariants[variant].speciesList[i] == species)
			return true;
	}

	return false;
}

static u8 GetVariantRegionalFormFlags(u8 variant)
{
	switch (variant)
	{
	case POKEDEX_VARIANT_ROGUE_MODERN:
	case POKEDEX_VARIANT_ROGUE_CLASSICPLUS:
	case POKEDEX_VARIANT_NATIONAL_GEN9:
		return REGIONAL_FORM_ALL;

	case POKEDEX_VARIANT_KANTO_LETSGO:
	case POKEDEX_VARIANT_ALOLA_SM:
	case POKEDEX_VARIANT_ALOLA_USUM:
	case POKEDEX_VARIANT_NATIONAL_GEN7:
		return REGIONAL_FORM_ALOLAN;

	case POKEDEX_VARIANT_GALAR_SWSH:
	case POKEDEX_VARIANT_GALAR_ISLEOFARMOR:
	case POKEDEX_VARIANT_GALAR_CROWNTUNDRA:
	case POKEDEX_VARIANT_GALAR_FULLDLC:
		return REGIONAL_FORM_GALARIAN;

	case POKEDEX_VARIANT_PALDEA_SCVI:
	case POKEDEX_VARIANT_PALDEA_KITAKAMI:
		return REGIONAL_FORM_PALDEAN;

	case POKEDEX_VARIANT_PALDEA_BLUEBERRY:
	case POKEDEX_VARIANT_PALDEA_FULLDLC:
		return REGIONAL_FORM_ALOLAN | REGIONAL_FORM_PALDEAN;

	case POKEDEX_VARIANT_EXTRAS_LEGENDSARCEUS:
		return REGIONAL_FORM_HISUIAN;

	case POKEDEX_VARIANT_NATIONAL_GEN8:
		return REGIONAL_FORM_ALOLAN | REGIONAL_FORM_GALARIAN | REGIONAL_FORM_HISUIAN;
	}

	return REGIONAL_FORM_NONE;
}

static u8 GetSpeciesRegionalFormFlag(u16 species)
{
	// White-Striped Basculin is a Hisui-specific form, but expansion does not
	// tag it with isHisuianForm.
	if (species == SPECIES_BASCULIN_WHITE_STRIPED)
		return REGIONAL_FORM_HISUIAN;

	if (gRogueSpeciesInfo[species].isAlolanForm)
		return REGIONAL_FORM_ALOLAN;
	if (gRogueSpeciesInfo[species].isGalarianForm)
		return REGIONAL_FORM_GALARIAN;
	if (gRogueSpeciesInfo[species].isHisuianForm)
		return REGIONAL_FORM_HISUIAN;
	if (gRogueSpeciesInfo[species].isPaldeanForm)
		return REGIONAL_FORM_PALDEAN;

	return REGIONAL_FORM_NONE;
}

static void BuildRegionalEvolutionPathMasks()
{
	bool changed;

	for (int species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
	{
		if (eggLookup[species] == species
			&& GET_BASE_SPECIES_ID(species) == species)
		{
			u8 regionalFlags = GetSpeciesRegionalFormFlag(species);
			sRegionalEvolutionPathMasks[species] |= 1 << regionalFlags;
		}
	}

	do
	{
		changed = false;

		for (int species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
		{
			u16 sourcePathMasks = sRegionalEvolutionPathMasks[species];

			if (sourcePathMasks == 0)
				continue;

			for (int evoIdx = 0; evoIdx < Rogue_GetMaxEvolutionCount(species); ++evoIdx)
			{
				struct Evolution evo;
				u16 targetPathMasks = 0;

				Rogue_ModifyEvolution(species, evoIdx, &evo);
				if (evo.method == 0 || evo.targetSpecies == SPECIES_NONE)
					continue;

				for (int pathMask = 0; pathMask <= REGIONAL_FORM_ALL; ++pathMask)
				{
					if (sourcePathMasks & (1 << pathMask))
					{
						u8 targetRegionalFlags = GetSpeciesRegionalFormFlag(evo.targetSpecies);
						targetPathMasks |= 1 << (pathMask | targetRegionalFlags);
					}
				}

				if ((sRegionalEvolutionPathMasks[evo.targetSpecies] | targetPathMasks)
					!= sRegionalEvolutionPathMasks[evo.targetSpecies])
				{
					sRegionalEvolutionPathMasks[evo.targetSpecies] |= targetPathMasks;
					changed = true;
				}
			}
		}
	}
	while (changed);
}

static bool IsRegionalFamilyApprovedForVariant(u16 species, u16 eggSpecies, u8 variant)
{
	u8 directFlags = GetSpeciesRegionalFormFlag(species) | GetSpeciesRegionalFormFlag(eggSpecies);
	u8 allowedFlags = GetVariantRegionalFormFlags(variant);
	u16 pathMasks = sRegionalEvolutionPathMasks[species];

	// Forms that are not represented in the evolution graph still retain their
	// direct form requirement.
	if (pathMasks == 0)
		pathMasks = 1 << directFlags;

	for (int pathMask = 0; pathMask <= REGIONAL_FORM_ALL; ++pathMask)
	{
		u8 requiredFlags;

		if (!(pathMasks & (1 << pathMask)))
			continue;

		requiredFlags = pathMask | directFlags;

		// The combined Paldea Pokédex contains families from all three component
		// Pokédexes. Only Blueberry's families should gain their Alolan branches.
		if (variant == POKEDEX_VARIANT_PALDEA_FULLDLC
			&& (requiredFlags & REGIONAL_FORM_ALOLAN)
			&& !VariantContainsSpecies(
				POKEDEX_VARIANT_PALDEA_BLUEBERRY,
				GET_BASE_SPECIES_ID(eggSpecies)))
			continue;

		if ((requiredFlags & ~allowedFlags) == 0)
			return true;
	}

	return false;
}
#endif

static bool IsEvoListSpeciesValid(u16 species)
{
#ifdef ROGUE_EXPANSION
	if (species == SPECIES_DARMANITAN_GALARIAN_ZEN_MODE || species == SPECIES_POLTCHAGEIST_ARTISAN || species == SPECIES_SINISTCHA_MASTERPIECE)
		return false;

	if (gRogueSpeciesInfo[species].baseHP == 0)
		return false;

	// Base species
	if (gRogueSpeciesInfo[species].formSpeciesIdTable == nullptr || gRogueSpeciesInfo[species].formSpeciesIdTable[0] == species)
		return true;

	// Or regional form
	if (gRogueSpeciesInfo[species].isAlolanForm || gRogueSpeciesInfo[species].isGalarianForm || gRogueSpeciesInfo[species].isHisuianForm || gRogueSpeciesInfo[species].isPaldeanForm)
		return true;

	return false;
#else
	if (species >= SPECIES_OLD_UNOWN_B && species <= SPECIES_OLD_UNOWN_Z)
		return false;

	return true;
#endif
}

int main()
{
	for (int s = SPECIES_NONE; s < NUM_SPECIES; ++s)
	{
		if (s == SPECIES_NONE)
		{
			eggLookup[s] = s;
			evolutionCountLookup[s] = 0;
		}
		else
		{
			u16 eggSpecies = Rogue_GetEggSpecies(s);
			eggLookup[s] = eggSpecies;
			evolutionCountLookup[s] = Rogue_GetMaxEvolutionCount(s);

#ifdef ROGUE_EXPANSION
			if (gSpeciesInfo[s].types[0] != TYPE_NONE)
				eggEvolutionTypes[eggSpecies].insert(gSpeciesInfo[s].types[0]);

			if (gSpeciesInfo[s].types[1] != TYPE_NONE)
				eggEvolutionTypes[eggSpecies].insert(gSpeciesInfo[s].types[1]);
#else
			if (gRogueSpeciesInfo[s].type1 != TYPE_NONE)
				eggEvolutionTypes[eggSpecies].insert(gRogueSpeciesInfo[s].type1);

			if (gRogueSpeciesInfo[s].type2 != TYPE_NONE)
				eggEvolutionTypes[eggSpecies].insert(gRogueSpeciesInfo[s].type2);
#endif
		}
	}

#ifdef ROGUE_EXPANSION
	BuildRegionalEvolutionPathMasks();
#endif

	std::string const c_OutputPath = "../../../../src/data/rogue_bake_data.h";

	std::ofstream file;
	file.open(c_OutputPath, std::ios::out);

	file << "// == WARNING ==\n";
	file << "// DO NOT EDIT THIS FILE\n";
	file << "// This file was automatically generated by PokemonQueryBaker\n";
	file << "\n";

	// Species info (Bundle into one u32 per species)
	//
	{
		file << "const struct RogueSpeciesBakedData gRogueBake_SpeciesData[NUM_SPECIES] =\n{\n";
		for (int s = SPECIES_NONE; s < NUM_SPECIES; ++s)
		{
			u32 typeFlags = 0;

			for (int t = 0; t < NUMBER_OF_MON_TYPES; ++t)
			{
				if (HasEvolutionConnectionOfType(s, t))
					typeFlags |= MON_TYPE_VAL_TO_FLAGS(t);
			}

			file << "\t[" << s << "] =\n\t{\n";
			file << "\t\t.eggSpecies = " << (int)eggLookup[s] << ",\n";
			file << "\t\t.evolutionCount = " << (int)evolutionCountLookup[s] << ",\n";
			file << "\t\t.evolutionChainTypeFlags = " << (int)typeFlags << ",\n";
			file << "\t},\n";
		}
		file << "};\n";
		file << "\n";
	}

	// Unique lists of final and egg evo species
	//
	{
		std::vector<u16> reorderedSpeciesList;
		std::set<u16> alreadyAddedSpecies;

#ifdef ROGUE_EXPANSION
		for (int j = SPECIES_NONE + 1; j < NUM_SPECIES; ++j)
		{
			int species = j;

			if (alreadyAddedSpecies.find(species) == alreadyAddedSpecies.end())
			{
				reorderedSpeciesList.push_back(species);
				alreadyAddedSpecies.insert(species);

				if (gRogueSpeciesInfo[species].formSpeciesIdTable != nullptr)
				{
					for (int i = 0; gRogueSpeciesInfo[species].formSpeciesIdTable[i] != FORM_SPECIES_END; ++i)
					{
						int s = gRogueSpeciesInfo[species].formSpeciesIdTable[i];
						if (alreadyAddedSpecies.find(s) == alreadyAddedSpecies.end())
						{
							reorderedSpeciesList.push_back(s);
							alreadyAddedSpecies.insert(s);
						}
					}
				}
			}
		}
#else
		for (int s = SPECIES_NONE + 1; s < NUM_SPECIES; ++s)
		{
			reorderedSpeciesList.push_back(s);
		}
#endif


		file << "const u16 gRogueBake_FinalEvoSpecies[] =\n{\n";
		for (u16 s : reorderedSpeciesList)
		{
			if (IsEvoListSpeciesValid(s) && Rogue_GetMaxEvolutionCount(s) == 0)
			{
				file << "\t" << s << ",\n";
			}
		}
		file << "};\n";
		file << "const u16 gRogueBake_FinalEvoSpecies_Count = ARRAY_COUNT(gRogueBake_FinalEvoSpecies);\n";
		file << "\n";

		file << "const u16 gRogueBake_EggSpecies[] =\n{\n";
		alreadyAddedSpecies.clear();

		for (u16 s : reorderedSpeciesList)
		{
			int species = Rogue_GetEggSpecies(s);

			if (alreadyAddedSpecies.find(species) == alreadyAddedSpecies.end())
			{
				alreadyAddedSpecies.insert(species);

				if (IsEvoListSpeciesValid(species) && Rogue_GetEggSpecies(species) == species)
				{
					file << "\t" << species << ",\n";
				}
			}
		}
		file << "};\n";
		file << "const u16 gRogueBake_EggSpecies_Count = ARRAY_COUNT(gRogueBake_EggSpecies);\n";
		file << "\n";
	}

	// Dex variant lists
	//
	{
		std::array<u8, SPECIES_FLAGS_BYTE_COUNT> bitFlags;
		std::array<u8, SPECIES_FLAGS_BYTE_COUNT> familyRootFlags;
		bitFlags.fill(0);
		familyRootFlags.fill(0);

		file << "\n";
		file << "const u8 gRogueBake_PokedexVariantBitFlags[POKEDEX_VARIANT_COUNT][SPECIES_FLAGS_BYTE_COUNT] =\n{\n";

		for (int i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
		{
			bitFlags.fill(0);

			for (int j = 0; j < gPokedexVariants[i].speciesCount; ++j)
				SetBitFlag(gPokedexVariants[i].speciesList[j], TRUE, &bitFlags[0]);

			file << "\t[" << i << "] =\n\t{" << "\n";

			for (int j = 0; j < SPECIES_FLAGS_BYTE_COUNT; ++j)
			{
				file << "\t\t" << (int)bitFlags[j] << ",\n";
			}

			file << "\t},\n";
		}

		//extern const struct RoguePokedexVariant gPokedexVariants[POKEDEX_VARIANT_COUNT];
		//extern const struct RoguePokedexRegion gPokedexRegions[POKEDEX_REGION_COUNT];

		file << "};\n";

		file << "\n";
		file << "const u8 gRogueBake_PokedexVariantEligibilityBitFlags[POKEDEX_VARIANT_COUNT][SPECIES_FLAGS_BYTE_COUNT] =\n{\n";

		for (int i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
		{
			bitFlags.fill(0);
			familyRootFlags.fill(0);

			for (int j = 0; j < gPokedexVariants[i].speciesCount; ++j)
			{
				u16 species = gPokedexVariants[i].speciesList[j];
				u16 familyRoot = eggLookup[species];

#ifdef ROGUE_EXPANSION
				familyRoot = GET_BASE_SPECIES_ID(familyRoot);
#endif

				SetBitFlag(species, TRUE, &bitFlags[0]);
				SetBitFlag(familyRoot, TRUE, &familyRootFlags[0]);
			}

			for (int species = SPECIES_NONE + 1; species < NUM_SPECIES; ++species)
			{
				u16 eggSpecies;
				u16 familyRoot;

				if (gRogueSpeciesInfo[species].baseHP == 0)
					continue;

				eggSpecies = eggLookup[species];
				familyRoot = eggSpecies;

#ifdef ROGUE_EXPANSION
				familyRoot = GET_BASE_SPECIES_ID(familyRoot);

				if (!IsRegionalFamilyApprovedForVariant(species, eggSpecies, i))
					continue;
#endif

				if (GetBitFlag(familyRoot, &familyRootFlags[0]))
					SetBitFlag(species, TRUE, &bitFlags[0]);
			}

			file << "\t[" << i << "] =\n\t{" << "\n";

			for (int j = 0; j < SPECIES_FLAGS_BYTE_COUNT; ++j)
				file << "\t\t" << (int)bitFlags[j] << ",\n";

			file << "\t},\n";
		}

		file << "};\n";
	}

	// Possible Evo + Form items
	//
	{
		file << "\n";
		file << "const u16 gRogueBake_EvoItems[] =\n{\n";

		for (int i = ITEM_NONE + 1; i < ITEMS_COUNT; ++i)
		{
			if (Rogue_IsEvolutionItem(i))
				file << "\t" << i << ",\n";
		}
		file << "};\n";
		file << "const u16 gRogueBake_EvoItems_Count = ARRAY_COUNT(gRogueBake_EvoItems);\n";
		file << "\n";

		file << "\n";
		file << "const u16 gRogueBake_FormItems[] =\n{\n";

		for (int i = ITEM_NONE + 1; i < ITEMS_COUNT; ++i)
		{
			if (Rogue_IsFormItem(i))
				file << "\t" << i << ",\n";
		}

		file << "};\n";
		file << "const u16 gRogueBake_FormItems_Count = ARRAY_COUNT(gRogueBake_FormItems);\n";
		file << "\n";
	}

	file.close();
	return 0;
}
