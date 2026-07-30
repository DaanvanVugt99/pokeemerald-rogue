#include "global.h"
#include "constants/abilities.h"
#include "constants/moves.h"
#include "constants/rogue.h"
#include "constants/rgb.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_colour_utils.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_safari.h"
#include "test/test.h"

#define TEST_FORMAT_MON_TYPE                    1
#define TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY    2
#define TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY    3
#define TEST_DYNAMIC_MOVE_POOL_CAPACITY       255
#define TEST_DYNAMIC_MOVE_PAIR_CODE_START     (TEST_DYNAMIC_MOVE_POOL_CAPACITY + 1)

TEST("Unique Pokemon palette classification safely handles neutral palettes")
{
    u8 i;
    u16 palette[16] = {0};
    u16 layers[16];

    Rogue_GenerateLayerPaletteByHue(palette, palette, layers);

    for (i = 0; i < ARRAY_COUNT(layers); ++i)
        EXPECT_EQ(layers[i], RGB_BLACK);
}

TEST("Unique Pokemon palette classification treats a single hue as the primary layer")
{
    u16 palette[16] = {0};
    u16 layers[16];

    palette[1] = HSVToRGB((struct HSV){ .h = 20, .s = 255, .v = 255 });
    palette[2] = palette[1];

    Rogue_GenerateLayerPaletteByHue(palette, palette, layers);

    EXPECT_EQ(layers[0], RGB_BLACK);
    EXPECT_EQ(layers[1], RGB_RED);
    EXPECT_EQ(layers[2], RGB_RED);
}

TEST("Unique Pokemon palette classification measures hue across the color wheel boundary")
{
    u16 palette[16] = {0};
    u16 layers[16];

    palette[1] = HSVToRGB((struct HSV){ .h = 250, .s = 255, .v = 255 });
    palette[2] = HSVToRGB((struct HSV){ .h = 5, .s = 255, .v = 255 });
    palette[3] = HSVToRGB((struct HSV){ .h = 128, .s = 255, .v = 255 });

    Rogue_GenerateLayerPaletteByHue(palette, palette, layers);

    EXPECT_EQ(layers[1], RGB_RED);
    EXPECT_EQ(layers[2], RGB_RED);
    EXPECT_EQ(layers[3], RGB_GREEN);
}

static u32 EncodeTestMoveSelection(u32 move1, u32 move2)
{
    u32 first;
    u32 second;
    u32 pairRank;

    if(move1 == 0)
        return move2;
    if(move2 == 0)
        return move1;

    first = move1 - 1;
    second = move2 - 1;
    if(first > second)
    {
        u32 temp = first;
        first = second;
        second = temp;
    }

    pairRank = first * (2 * TEST_DYNAMIC_MOVE_POOL_CAPACITY - first - 1) / 2;
    pairRank += second - first - 1;
    return TEST_DYNAMIC_MOVE_PAIR_CODE_START + pairRank;
}

static u32 DynamicOriginalCustomMonId(u32 move1, u32 move2, u32 ability)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (ability << 15);
}

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot, u32 typeMoveChoice, u32 move1, u32 move2, u32 ability)
{
    u32 rarity;
    u32 payload;

    if(ability != 0)
    {
        rarity = UNIQUE_RARITY_EPIC;
        payload = move1 | (ability << 8);
    }
    else if(move1 != 0 || move2 != 0)
    {
        rarity = UNIQUE_RARITY_RARE;
        payload = EncodeTestMoveSelection(move1, move2);
    }
    else
    {
        rarity = UNIQUE_RARITY_COMMON;
        payload = 0;
    }

    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (typeSlot << 5)
        | (typeMoveChoice << 6)
        | (payload << 8)
        | (rarity << 26)
        | (TEST_FORMAT_MON_TYPE << 28);
}

static u32 DynamicOriginalUniqueAbilityCustomMonId(u32 move1, u32 move2, u32 abilitySeed, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | EncodeTestMoveSelection(move1, move2)
        | (uniqueAbility << 15)
        | (abilitySeed << 25)
        | (TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY << 28);
}

static u32 DynamicTypeUniqueAbilityCustomMonId(u32 type, u32 typeSlot, u32 typeMoveChoice, u32 move1, u32 abilitySeed, u32 uniqueAbility)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | type
        | (typeSlot << 5)
        | (typeMoveChoice << 6)
        | (move1 << 8)
        | (uniqueAbility << 16)
        | (abilitySeed << 26)
        | (TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY << 28);
}

static bool8 IsExpectedCreationSynergyMove(u8 type, u16 move)
{
    static const u16 sMoves[NUMBER_OF_MON_TYPES][3] =
    {
        [TYPE_NORMAL] = { MOVE_SUBSTITUTE, MOVE_RECOVER, MOVE_SWORDS_DANCE },
        [TYPE_FIGHTING] = { MOVE_BULK_UP },
        [TYPE_FLYING] = { MOVE_ROOST },
        [TYPE_FIRE] = { MOVE_WILL_O_WISP },
        [TYPE_WATER] = { MOVE_AQUA_RING },
        [TYPE_POISON] = { MOVE_TOXIC_SPIKES, MOVE_TOXIC },
        [TYPE_GROUND] = { MOVE_SPIKES },
        [TYPE_ROCK] = { MOVE_STEALTH_ROCK, MOVE_ROCK_POLISH },
        [TYPE_BUG] = { MOVE_STICKY_WEB, MOVE_QUIVER_DANCE },
        [TYPE_GHOST] = { MOVE_DESTINY_BOND, MOVE_CURSE, MOVE_CONFUSE_RAY },
        [TYPE_STEEL] = { MOVE_SHIFT_GEAR, MOVE_IRON_DEFENSE },
        [TYPE_GRASS] = { MOVE_LEECH_SEED, MOVE_SPORE, MOVE_STRENGTH_SAP },
        [TYPE_ELECTRIC] = { MOVE_CHARGE, MOVE_THUNDER_WAVE, MOVE_MAGNET_RISE },
        [TYPE_PSYCHIC] = { MOVE_TRICK_ROOM, MOVE_CALM_MIND, MOVE_GRAVITY },
        [TYPE_ICE] = { MOVE_HAZE },
        [TYPE_DRAGON] = { MOVE_DRAGON_DANCE },
        [TYPE_DARK] = { MOVE_TAUNT, MOVE_NASTY_PLOT, MOVE_TORMENT },
        [TYPE_FAIRY] = { MOVE_CHARM, MOVE_SWEET_KISS },
    };
    u8 i;

    if(move == MOVE_NONE)
        return FALSE;

    for(i = 0; i < ARRAY_COUNT(sMoves[type]); ++i)
    {
        if(sMoves[type][i] == move)
            return TRUE;
    }

    return FALSE;
}

TEST("Dynamic custom unique ability lookup falls back for ordinary species")
{
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, 0), ABILITY_SILVER_LINING);
}

TEST("Dynamic custom unique ability lookup overrides species mapping")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_IMPACT);
    struct Pokemon mon;

    CreateMon(&mon, SPECIES_SHUCKLE, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_CUSTOM_MON, customMonId);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, customMonId), ABILITY_IMPACT);
    EXPECT_EQ(GetMonUniqueAbility(&mon), ABILITY_IMPACT);
}

TEST("Dynamic custom unique ability lookup falls back when the encoded id has none")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_SHUCKLE, customMonId), ABILITY_SILVER_LINING);
}

TEST("Safari reconstruction preserves a unique Pokemon's native ability slot")
{
    static const u16 sExpectedAbilities[] =
    {
        ABILITY_LEVITATE,
        ABILITY_NEUTRALIZING_GAS,
    };
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0, 0, 0);
    u8 abilityNum;

    for(abilityNum = 0; abilityNum < ARRAY_COUNT(sExpectedAbilities); ++abilityNum)
    {
        struct Pokemon original;
        struct Pokemon restored;
        struct RogueSafariMon safariMon = {0};
        u16 eggSpecies = SPECIES_KOFFING;

        CreateMon(&original, SPECIES_WEEZING, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_CUSTOM_MON, customMonId);
        SetMonData(&original, MON_DATA_ABILITY_NUM, &abilityNum);
        RogueSafari_CopyToSafariMon(&original.box, &safariMon);

        ZeroMonData(&restored);
        CreateMon(&restored, SPECIES_KOFFING, 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
        RogueSafari_CopyFromSafariMon(&safariMon, &restored.box);
        SetMonData(&restored, MON_DATA_SPECIES, &eggSpecies);
        Rogue_ApplyCustomMonIdToMon(customMonId, &restored);

        EXPECT_EQ(GetMonData(&restored, MON_DATA_ABILITY_NUM), abilityNum);
        EXPECT_EQ(GetMonAbility(&restored), sExpectedAbilities[abilityNum]);
    }
}

TEST("Reapplying custom data keeps an explicit standard ability authoritative")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);
    struct Pokemon mon;
    u8 abilityNum = 1;

    CreateMon(&mon, SPECIES_WEEZING, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(&mon, MON_DATA_ABILITY_NUM, &abilityNum);
    Rogue_ApplyCustomMonIdToMon(customMonId, &mon);

    EXPECT_EQ(GetMonData(&mon, MON_DATA_ABILITY_NUM), 0);
    EXPECT_EQ(GetMonAbility(&mon), RogueGift_GetCustomMonAbility(customMonId, 0));
}

TEST("Reapplying custom data preserves nature, IVs, and calculated stats")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);
    struct Pokemon mon;
    u16 maxHp;
    u16 attack;
    u16 defense;
    u16 speed;
    u16 spAttack;
    u16 spDefense;

    CreateMon(&mon, SPECIES_WEEZING, 50, 31, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetNature(&mon, NATURE_ADAMANT);
    maxHp = GetMonData(&mon, MON_DATA_MAX_HP);
    attack = GetMonData(&mon, MON_DATA_ATK);
    defense = GetMonData(&mon, MON_DATA_DEF);
    speed = GetMonData(&mon, MON_DATA_SPEED);
    spAttack = GetMonData(&mon, MON_DATA_SPATK);
    spDefense = GetMonData(&mon, MON_DATA_SPDEF);

    Rogue_ApplyCustomMonIdToMon(customMonId, &mon);

    EXPECT_EQ(GetNature(&mon), NATURE_ADAMANT);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_HP_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPDEF_IV), 31);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_MAX_HP), maxHp);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_ATK), attack);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_DEF), defense);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPEED), speed);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPATK), spAttack);
    EXPECT_EQ(GetMonData(&mon, MON_DATA_SPDEF), spDefense);
}

#ifdef ROGUE_EXPANSION
TEST("Dynamic standard ability pool includes every audited regular ability once by mechanic")
{
    static const u16 sExpectedExclusions[] =
    {
        ABILITY_FORECAST,
        ABILITY_MULTITYPE,
        ABILITY_FLOWER_GIFT,
        ABILITY_ZEN_MODE,
        ABILITY_STANCE_CHANGE,
        ABILITY_SHIELDS_DOWN,
        ABILITY_SCHOOLING,
        ABILITY_DISGUISE,
        ABILITY_BATTLE_BOND,
        ABILITY_POWER_CONSTRUCT,
        ABILITY_RKS_SYSTEM,
        ABILITY_GULP_MISSILE,
        ABILITY_ICE_FACE,
        ABILITY_HUNGER_SWITCH,
        ABILITY_ZERO_TO_HERO,
        ABILITY_COMMANDER,
        ABILITY_TERA_SHIFT,
        ABILITY_TERA_SHELL,
        ABILITY_TERAFORM_ZERO,
        ABILITY_FORECAST_PRIORITY,
        ABILITY_EMBODY_ASPECT_TEAL,
        ABILITY_EMBODY_ASPECT_HEARTHFLAME,
        ABILITY_EMBODY_ASPECT_WELLSPRING,
        ABILITY_EMBODY_ASPECT_CORNERSTONE,
        ABILITY_TRUANT,
        ABILITY_SLOW_START,
        ABILITY_DEFEATIST,
        ABILITY_STALL,
        ABILITY_NEUTRALIZING_GAS,
        ABILITY_WONDER_GUARD,
    };
    u16 eligibleCount = 0;
    u16 ability;
    u16 i;

    for(ability = 1; ability <= ABILITY_FORECAST_PRIORITY; ++ability)
    {
        bool8 expected = TRUE;

        for(i = 0; i < ARRAY_COUNT(sExpectedExclusions); ++i)
        {
            if(sExpectedExclusions[i] == ability)
                expected = FALSE;
        }

        EXPECT_EQ(RogueGift_DebugIsStandardAbilityEligible(ability), expected);
        if(expected)
            ++eligibleCount;
    }

    EXPECT_EQ(eligibleCount, 281);
    EXPECT_EQ(RogueGift_DebugGetStandardAbilityGroupCount(), 267);
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_SHADOW_TAG));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_ARENA_TRAP));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_IMPOSTER));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_MOODY));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_PARENTAL_BOND));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_EMERGENCY_EXIT));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_WIMP_OUT));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_AS_ONE_ICE_RIDER));
    EXPECT(RogueGift_DebugIsStandardAbilityEligible(ABILITY_AS_ONE_SHADOW_RIDER));
}

TEST("Duplicate standard ability names share one weighted flavor group")
{
    static const u16 sGroups[][3] =
    {
        { ABILITY_HUGE_POWER,       ABILITY_PURE_POWER },
        { ABILITY_CLOUD_NINE,       ABILITY_AIR_LOCK },
        { ABILITY_MOLD_BREAKER,     ABILITY_TERAVOLT,          ABILITY_TURBOBLAZE },
        { ABILITY_DAZZLING,         ABILITY_QUEENLY_MAJESTY,   ABILITY_ARMOR_TAIL },
        { ABILITY_STALWART,         ABILITY_PROPELLER_TAIL },
        { ABILITY_GOOEY,            ABILITY_TANGLING_HAIR },
        { ABILITY_ROUGH_SKIN,       ABILITY_IRON_BARBS },
        { ABILITY_RECEIVER,         ABILITY_POWER_OF_ALCHEMY },
        { ABILITY_PROTEAN,          ABILITY_LIBERO },
        { ABILITY_CLEAR_BODY,       ABILITY_WHITE_SMOKE },
        { ABILITY_MOXIE,            ABILITY_CHILLING_NEIGH },
        { ABILITY_EMERGENCY_EXIT,   ABILITY_WIMP_OUT },
    };
    u8 group;

    for(group = 0; group < ARRAY_COUNT(sGroups); ++group)
    {
        u8 flavorCount = sGroups[group][2] == ABILITY_NONE ? 2 : 3;
        u8 flavor;

        EXPECT_EQ(RogueGift_DebugGetStandardAbilityFlavorCount(sGroups[group][0]), flavorCount);
        for(flavor = 0; flavor < flavorCount; ++flavor)
        {
            EXPECT_EQ(RogueGift_DebugGetStandardAbilityGroupRepresentative(sGroups[group][flavor]), sGroups[group][0]);
            EXPECT_EQ(RogueGift_DebugGetStandardAbilityFlavor(sGroups[group][0], flavor), sGroups[group][flavor]);
        }
    }
}

TEST("Native standard ability checks reject an entire flavor group")
{
    EXPECT(RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(SPECIES_MEDITITE, ABILITY_HUGE_POWER));
    EXPECT(RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(SPECIES_MEDITITE, ABILITY_PURE_POWER));
    EXPECT(RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(SPECIES_WIMPOD, ABILITY_EMERGENCY_EXIT));
    EXPECT(RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(SPECIES_WIMPOD, ABILITY_WIMP_OUT));
}

TEST("Nine bit standard ability IDs round trip in both non Legendary formats")
{
    u32 originalId = DynamicOriginalCustomMonId(1, 2, ABILITY_POISON_PUPPETEER);
    u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 1, 0, ABILITY_POISON_PUPPETEER);
    u32 flavorId = DynamicOriginalCustomMonId(1, 2, ABILITY_WIMP_OUT);
    u32 excludedId = DynamicOriginalCustomMonId(1, 2, ABILITY_FORECAST_PRIORITY);

    EXPECT_EQ(RogueGift_GetCustomMonAbility(originalId, 0), ABILITY_POISON_PUPPETEER);
    EXPECT_EQ(RogueGift_GetCustomMonAbility(typedId, 0), ABILITY_POISON_PUPPETEER);
    EXPECT_EQ(RogueGift_GetCustomMonAbility(flavorId, 0), ABILITY_WIMP_OUT);
    EXPECT_EQ(RogueGift_GetCustomMonAbility(excludedId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedId), 2);
}
#endif

TEST("Dynamic original format decodes epic payload without unique ability")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 1);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_EPIC);
}

TEST("Dynamic type format decodes epic payload without unique ability")
{
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 1, 0, 1);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_EPIC);
}

TEST("Dynamic original unique ability format decodes legendary payload")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_IMPACT);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
}

TEST("Dynamic unique ability pool includes Starmobile")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_STARMOBILE);
}

TEST("Dynamic unique ability pool serializes generated-only anomalous abilities")
{
    u32 chaosId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_CHAOS_THEORY);
    u32 shipId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_SHIP_OF_THESEUS);
    u32 engineId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_WORLD_ENGINE);
    u32 fluxId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_CHROMATIC_FLUX);
    u32 mirrorId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_MIRROR_WORLD);
    u32 libraryId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_LIVING_LIBRARY);

    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_CHAOS_THEORY));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_SHIP_OF_THESEUS));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_WORLD_ENGINE));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_CHROMATIC_FLUX));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_MIRROR_WORLD));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_LIVING_LIBRARY));
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(chaosId), ABILITY_CHAOS_THEORY);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(shipId), ABILITY_SHIP_OF_THESEUS);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(engineId), ABILITY_WORLD_ENGINE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(fluxId), ABILITY_CHROMATIC_FLUX);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(mirrorId), ABILITY_MIRROR_WORLD);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(libraryId), ABILITY_LIVING_LIBRARY);
}

TEST("Debug anomalous generator creates a Legendary with an anomalous ability")
{
    u32 customMonId = RogueGift_DebugCreateAnomalousMonId(SPECIES_BULBASAUR);

    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
    EXPECT(RogueGift_IsAnomalousUniqueAbility(RogueGift_GetCustomMonUniqueAbility(customMonId)));
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
}

TEST("Dynamic Unique custom gift transfer preserves its encrypted payload")
{
    struct Pokemon mon;
    u32 customMonId = RogueGift_DebugCreateAnomalousMonId(SPECIES_BULBASAUR);

    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    RogueGift_CreateMon(customMonId, &mon, SPECIES_BULBASAUR, 50, USE_RANDOM_IVS);

    EXPECT_EQ(GiveTradedMonToPlayer(&mon), MON_GIVEN_TO_PARTY);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SANITY_IS_BAD_EGG), FALSE);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_BULBASAUR);
    EXPECT_EQ(RogueGift_GetCustomMonId(&gPlayerParty[0]), customMonId);
}

TEST("Dynamic unique ability eligibility audit contains 583 abilities")
{
    static const u16 sExpectedExclusions[] =
    {
        ABILITY_UNUSED_320,
        ABILITY_PASSIVE_INCOME,
        ABILITY_DEBUG,
        ABILITY_BOUNTY,
        ABILITY_BITTER_RUSE,
        ABILITY_ULTRA_FALLOUT,
        ABILITY_ULTRA_SWOLE,
        ABILITY_ULTRA_STRUT,
        ABILITY_ULTRA_CONDUCTOR,
        ABILITY_ULTRA_ASCENT,
        ABILITY_ULTRA_EDGE,
        ABILITY_ULTRA_DEVOUR,
        ABILITY_ULTRA_VENIN,
        ABILITY_ULTRA_INJECTION,
        ABILITY_ULTRA_BASTION,
        ABILITY_ULTRA_DETONATION,
        ABILITY_UNSPEAKABLE,
        ABILITY_BLACK_ICE,
        ABILITY_ECHO_INSTINCT,
        ABILITY_SPORE_INSTINCT,
        ABILITY_HEX_INSTINCT,
        ABILITY_MOLT_INSTINCT,
        ABILITY_FLUX_INSTINCT,
        ABILITY_DRIVE_PROTOCOL,
        ABILITY_AIM_PROTOCOL,
        ABILITY_IMPACT_PROTOCOL,
        ABILITY_LOCK_PROTOCOL,
        ABILITY_HEAT_PROTOCOL,
        ABILITY_SURGE_PROTOCOL,
        ABILITY_HUNT_INSTINCT,
        ABILITY_EDGE_PROTOCOL,
        ABILITY_MIND_PROTOCOL,
        ABILITY_ARRAY_PROTOCOL,
        ABILITY_GUARD_INSTINCT,
        ABILITY_STORM_INSTINCT,
        ABILITY_COUNTERPUNCH,
        ABILITY_KINGS_DOMAIN,
        ABILITY_COLOSSAL,
        ABILITY_TRASH_ALCHEMY,
        ABILITY_POTENTIAL,
        ABILITY_WHY_NOT,
        ABILITY_SIEGE_INSTINCT,
        ABILITY_CRASH_PROTOCOL,
        ABILITY_HIDDEN_STASH,
        ABILITY_VANISHING_ACT,
        ABILITY_GLACIAL_FORTRESS,
    };
    u16 ability;
    u16 eligibleCount = 0;
    u16 i;

    for(ability = ABILITY_STRONG_WINDS; ability <= ABILITY_GLACIAL_FORTRESS; ++ability)
    {
        if(RogueGift_IsDynamicUniqueAbilityEligible(ability))
            ++eligibleCount;
    }

    EXPECT_EQ(eligibleCount, 583);
    EXPECT_EQ(RogueGift_GetDynamicUniqueAbilityPoolCount(), 583);
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_STRONG_WINDS));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_STARMOBILE));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FLOCK_STEP));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_COLD_PREMONITION));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_DEAD_HEAT));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SPIRIT_FEAST));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_EMERALD_ACCORD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SAND_COMMAND));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_BLAST_SHIELD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FLAK_SHIELD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_BOG_BODY));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_GUIDING_FLAMES));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FALSE_GROUND));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_TUNDRA_REIGN));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_ASTRAL_REIGN));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SWORD_AND_BOARD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_WANDERING_HUNTER));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SMOG_REFINERY));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SNOWBALL_FIGHT));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_PREMONITION));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_HORN_LOCK));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_COUNTERSPELL));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_FEVER_PITCH));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_RAPID_REPLICA));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_MENHIR));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_COUNTERSTEP));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_CHAOS_THEORY));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_SHIP_OF_THESEUS));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_WORLD_ENGINE));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_CHROMATIC_FLUX));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_MIRROR_WORLD));
    EXPECT(RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_LIVING_LIBRARY));
    EXPECT(!RogueGift_IsDynamicUniqueAbilityEligible(ABILITY_GLACIAL_FORTRESS));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_CHAOS_THEORY));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_SHIP_OF_THESEUS));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_WORLD_ENGINE));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_CHROMATIC_FLUX));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_MIRROR_WORLD));
    EXPECT(RogueGift_IsAnomalousUniqueAbility(ABILITY_LIVING_LIBRARY));
    EXPECT(!RogueGift_IsAnomalousUniqueAbility(ABILITY_ADAPTIVE_ORIGIN));

    for(i = 0; i < ARRAY_COUNT(sExpectedExclusions); ++i)
        EXPECT(!RogueGift_IsDynamicUniqueAbilityEligible(sExpectedExclusions[i]));
}

TEST("Dynamic legendary payload stores direct ten bit unique ability ids")
{
    u32 customMonId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 3, ABILITY_COUNTERSPELL);

    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_COUNTERSPELL);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
}

TEST("Dynamic legendary standard ability seed decodes stably")
{
    u32 seedOneId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 1, ABILITY_STARMOBILE);
    u32 seedThreeId = DynamicOriginalUniqueAbilityCustomMonId(1, 2, 3, ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonAbility(seedOneId, 0), RogueGift_GetCustomMonAbility(seedOneId, 0));
    EXPECT_NE(RogueGift_GetCustomMonAbility(seedOneId, 0), ABILITY_NONE);
    EXPECT_NE(RogueGift_GetCustomMonAbility(seedThreeId, 0), ABILITY_NONE);
    EXPECT_NE(RogueGift_GetCustomMonAbility(seedOneId, 0), RogueGift_GetCustomMonAbility(seedThreeId, 0));
}



TEST("Dynamic unique ability synergy profiles preserve all audited pairings")
{
    u16 ability;
    u16 pairedAbilityCount = 0;

    for(ability = ABILITY_STRONG_WINDS; ability <= ABILITY_LIVING_LIBRARY; ++ability)
    {
        u16 profileId = RogueGift_DebugGetDynamicSynergyProfileId(ability);
        u16 moves[3];
        u8 moveCount = 0;
        u8 i;
        u8 j;

        if(profileId == 0)
            continue;

        ++pairedAbilityCount;
        for(i = 0; i < ARRAY_COUNT(moves); ++i)
        {
            moves[i] = RogueGift_DebugGetDynamicSynergyMove(ability, i);
            if(moves[i] == MOVE_NONE)
                continue;

            EXPECT_LT(moves[i], MOVES_COUNT);
            EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ability, moves[i]));
            EXPECT(RogueGift_DebugIsMoveInDynamicPool(moves[i]));
            ++moveCount;
            for(j = 0; j < i; ++j)
                EXPECT_NE(moves[i], moves[j]);
        }

        EXPECT_GE(moveCount, 1);
        EXPECT_LE(moveCount, 3);
    }

    EXPECT_EQ(pairedAbilityCount, 253);
}

TEST("Newest dynamic unique abilities use their required synergy profiles")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_TUNDRA_REIGN), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_ASTRAL_REIGN), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SWORD_AND_BOARD, 0), MOVE_PROTECT);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_WANDERING_HUNTER), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_FALSE_GROUND), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_SMOG_REFINERY), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_PREMONITION), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_COUNTERSPELL), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_FEVER_PITCH), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_VANISHING_ACT), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_MENHIR), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_COUNTERSTEP), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_CHAOS_THEORY), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_SHIP_OF_THESEUS), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_WORLD_ENGINE), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_CHROMATIC_FLUX), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_MIRROR_WORLD), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyProfileId(ABILITY_LIVING_LIBRARY), 0);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_RAPID_REPLICA, 0), MOVE_TRANSFORM);
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RAPID_REPLICA, MOVE_TRANSFORM));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RAPID_REPLICA, MOVE_COPYCAT));
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_HORN_LOCK, 0), MOVE_RAGING_BULL);
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_HORN_LOCK, MOVE_RAGING_BULL));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_HORN_LOCK, MOVE_CLOSE_COMBAT));
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_HORN_LOCK, MOVE_TACKLE, MOVE_CELEBRATE), MOVE_RAGING_BULL);
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_HORN_LOCK, MOVE_RAGING_BULL, MOVE_CELEBRATE), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SNOWBALL_FIGHT, 0), MOVE_FLING);
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SNOWBALL_FIGHT, MOVE_FLING));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SNOWBALL_FIGHT, MOVE_U_TURN));
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_SNOWBALL_FIGHT, MOVE_TACKLE, MOVE_CELEBRATE), MOVE_FLING);
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_SNOWBALL_FIGHT, MOVE_FLING, MOVE_CELEBRATE), MOVE_NONE);
}

TEST("Direct move synergy uses battle semantics instead of learnability")
{
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_PROTECT));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_DETECT));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_SPIKY_SHIELD));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_BANEFUL_BUNKER));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SWORD_AND_BOARD, MOVE_SWORDS_DANCE));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_GUIDING_FLAMES, MOVE_SHADOW_BALL));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_GUIDING_FLAMES, MOVE_CONFUSE_RAY));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_GUIDING_FLAMES, MOVE_FLAMETHROWER));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FLOWER_FIELD, MOVE_PROTECT));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FLOWER_FIELD, MOVE_TAUNT));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FLOWER_FIELD, MOVE_BODY_SLAM));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_BRAVERY, MOVE_ROOST));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_WATER_GLIDE, MOVE_HURRICANE));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_WATER_GLIDE, MOVE_ROOST));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_BAG_OF_TRICKS, MOVE_TAUNT));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_BAG_OF_TRICKS, MOVE_HEAL_PULSE));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RUMBLE_ROLL, MOVE_EARTH_POWER));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_RUMBLE_ROLL, MOVE_SPIKES));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_DYNAMIC_PUNCH));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_INFERNO));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_SING));
    EXPECT(!RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SIGHTING_SYSTEM, MOVE_FOCUS_BLAST));

    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_SOLARBOOST, MOVE_WEATHER_BALL));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_STEALTH, MOVE_PHANTOM_FORCE));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FAULT_FINDER, MOVE_EARTHQUAKE));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_FAULT_FINDER, MOVE_IRON_HEAD));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_HOT_SHELLS, MOVE_AURA_SPHERE));
    EXPECT(RogueGift_DebugDoesMoveMatchDynamicSynergy(ABILITY_HECKLER, MOVE_BODY_SLAM));

    SeedRng(0);
    EXPECT_LT(RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_DITTO, ABILITY_SWORD_AND_BOARD, MOVE_NONE), 3);
}

TEST("Qualifying custom moves preserve the rest of the rolled payload")
{
    // A random exotic move can satisfy the interaction.
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_TOXIC_TANDEM, MOVE_GUNK_SHOT, MOVE_HYDRO_STEAM), MOVE_NONE);

    // The changed-type move is checked by the same semantic matcher.
    EXPECT_EQ(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_THERMAL_LIFT, MOVE_FIRE_LASH, MOVE_HYDRO_STEAM), MOVE_NONE);

    // Without a qualifying custom move, one curated candidate is requested.
    SeedRng(0);
    EXPECT_NE(RogueGift_DebugSelectDynamicSynergyMove(
        SPECIES_DITTO, ABILITY_SWORD_AND_BOARD, MOVE_HYDRO_STEAM, MOVE_CHILLING_WATER), MOVE_NONE);
}

TEST("Galarian bird unique abilities use the required legendary synergy profiles")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_COLD_PREMONITION, 0), MOVE_FREEZING_GLARE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_COLD_PREMONITION, 1), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_COLD_PREMONITION, 2), MOVE_NONE);

    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_DEAD_HEAT, 0), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_DEAD_HEAT, 1), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_DEAD_HEAT, 2), MOVE_NONE);

    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SPIRIT_FEAST, 0), MOVE_NIGHT_SLASH);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SPIRIT_FEAST, 1), MOVE_DARK_PULSE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_SPIRIT_FEAST, 2), MOVE_FOUL_PLAY);
}

TEST("Final Step generated synergy guarantees a sound move")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_FINAL_STEP, 0), MOVE_NONE);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_FINAL_STEP, 1), MOVE_BOOMBURST);
    EXPECT_EQ(RogueGift_DebugGetDynamicSynergyMove(ABILITY_FINAL_STEP, 2), MOVE_SNARL);
}

TEST("Dynamic Unique pool only includes Gimmighoul Chest Form")
{
#ifdef ROGUE_EXPANSION
    EXPECT(RogueGift_DebugIsSpeciesInDynamicUniquePool(SPECIES_GIMMIGHOUL_CHEST));
    EXPECT(!RogueGift_DebugIsSpeciesInDynamicUniquePool(SPECIES_GIMMIGHOUL_ROAMING));
#else
    ASSUME(FALSE);
#endif
}

TEST("Every eligible dynamic species has enough family-exotic moves")
{
    EXPECT(RogueGift_DebugAllDynamicSpeciesHaveExoticMoves(2));
}


TEST("Dynamic synergy selection respects physical special and mixed profiles")
{
    u16 physicalCounts[3] = {0};
    u16 specialCounts[3] = {0};
    u16 seed;

    for(seed = 0; seed < 256; ++seed)
    {
        SeedRng(seed);
        ++physicalCounts[RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_NINJASK, ABILITY_TOXIC_TANDEM, MOVE_NONE)];
        SeedRng(seed);
        ++specialCounts[RogueGift_DebugSelectDynamicSynergyChoice(SPECIES_ALAKAZAM, ABILITY_TOXIC_TANDEM, MOVE_NONE)];
    }

    // All valid categories remain possible, while the stronger offensive stat
    // makes its matching category more likely.
    EXPECT_GT(physicalCounts[0], 0);
    EXPECT_GT(physicalCounts[1], 0);
    EXPECT_GT(physicalCounts[2], 0);
    EXPECT_GT(specialCounts[0], 0);
    EXPECT_GT(specialCounts[1], 0);
    EXPECT_GT(specialCounts[2], 0);
    EXPECT_GT(physicalCounts[0], physicalCounts[1]);
    EXPECT_GT(specialCounts[1], specialCounts[0]);
}






TEST("Dynamic type unique ability format decodes legendary payload")
{
    u32 customMonId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 0, 0, 1, 1, ABILITY_IMPACT);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 2);
    EXPECT_NE(RogueGift_GetCustomMonAbility(customMonId, 0), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_IMPACT);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_LEGENDARY);
}

TEST("Existing dynamic type format decodes without a unique ability")
{
    u32 customMonId = DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonType(customMonId, 0), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(customMonId), ABILITY_NONE);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), UNIQUE_RARITY_COMMON);
}

TEST("Dynamic type formats decode all four altered-type move selectors")
{
    static const u16 sExpectedMoves[] =
    {
        MOVE_FIRE_LASH,
        MOVE_WILL_O_WISP,
        MOVE_OVERHEAT,
        MOVE_SACRED_FIRE,
    };
    u8 typeSlot;
    u8 typeMoveChoice;

    for(typeSlot = 0; typeSlot < 2; ++typeSlot)
    {
        for(typeMoveChoice = 0; typeMoveChoice < 4; ++typeMoveChoice)
        {
            u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, typeSlot, typeMoveChoice, 0, 0, 0);
            u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, typeSlot, typeMoveChoice, 0, 1, ABILITY_STARMOBILE);

            EXPECT_EQ(RogueGift_GetCustomMonType(typedId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 0), sExpectedMoves[typeMoveChoice]);
            EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, typeSlot), TYPE_FIRE);
            EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 0), sExpectedMoves[typeMoveChoice]);
        }
    }
}

TEST("Generated altered typings can roll every move choice in either slot")
{
    bool8 seen[2][4] = {{FALSE}};
    u16 seed;
    u8 seenCount = 0;

    for(seed = 0; seed < 2048 && seenCount < 8; ++seed)
    {
        u32 customMonId;
        u8 slot;
        u8 choice;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_COMMON, SPECIES_BULBASAUR);
        if(((customMonId >> 28) & 3) != TEST_FORMAT_MON_TYPE)
            continue;

        slot = (customMonId >> 5) & 1;
        choice = (customMonId >> 6) & 3;
        if(!seen[slot][choice])
        {
            seen[slot][choice] = TRUE;
            ++seenCount;
        }
    }

    EXPECT_EQ(seenCount, 8);
}

TEST("Dynamic exotic move pool includes expanded curated moves")
{
    static const u16 sExpectedMoves[] =
    {
        MOVE_OBLIVION_WING,
        MOVE_DUAL_WINGBEAT,
        MOVE_ICE_SPINNER,
        MOVE_CLANGING_SCALES,
        MOVE_THOUSAND_ARROWS,
        MOVE_DIRE_CLAW,
        MOVE_TIDY_UP,
        MOVE_SHIMMER,
        MOVE_COLD_SNAP,
        MOVE_NIGHTFALL,
        MOVE_JETSTREAM,
        MOVE_THUNDERCRUSH,
        MOVE_FOUL_MIXTURE,
        MOVE_STAGE_FRIGHT,
        MOVE_WILD_GROWTH,
        MOVE_STATIC_BURST,
        MOVE_CHEAP_TRICK,
        MOVE_BRAINSTORM,
    };
    u16 firstCustomMoveIndex = RogueGift_DebugGetDynamicExoticMoveCount() - ARRAY_COUNT(sExpectedMoves) + 1;
    u8 i;

    EXPECT_EQ(RogueGift_DebugGetDynamicExoticMoveCount(), 207);

    for(i = 0; i < ARRAY_COUNT(sExpectedMoves); ++i)
        EXPECT_EQ(RogueGift_DebugGetDynamicMoveByIndex(firstCustomMoveIndex + i), sExpectedMoves[i]);
}

TEST("Standalone synergy moves are eligible as exotics")
{
    static const u16 sPromotedMoves[] =
    {
        MOVE_AIR_SLASH,
        MOVE_AQUA_STEP,
        MOVE_ATTACK_ORDER,
        MOVE_BITTER_BLADE,
        MOVE_BONEMERANG,
        MOVE_BONE_RUSH,
        MOVE_DRAGON_TAIL,
        MOVE_DRAIN_PUNCH,
        MOVE_FLARE_BLITZ,
        MOVE_FROST_BREATH,
        MOVE_JAW_LOCK,
        MOVE_PSYCHIC_FANGS,
        MOVE_SHADOW_BONE,
        MOVE_WATER_SHURIKEN,
        MOVE_DRAGON_HAMMER,
        MOVE_FREEZING_GLARE,
        MOVE_PHOTON_GEYSER,
        MOVE_SHELL_SIDE_ARM,
        MOVE_SPACIAL_REND,
        MOVE_TECHNO_BLAST,
        MOVE_TERA_STARSTORM,
        MOVE_UPROAR,
        MOVE_FUTURE_SIGHT,
        MOVE_POISON_JAB,
        MOVE_SACRED_FIRE,
        MOVE_SHIFT_GEAR,
        MOVE_BLAZE_KICK,
        MOVE_RAGING_BULL,
    };
    static const u16 sSynergyOnlyMoves[] =
    {
        MOVE_CHARGE,
        MOVE_MAGNET_RISE,
        MOVE_SWEET_KISS,
    };
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sPromotedMoves); ++i)
        EXPECT(RogueGift_DebugIsMoveExotic(sPromotedMoves[i]));

    for(i = 0; i < ARRAY_COUNT(sSynergyOnlyMoves); ++i)
    {
        EXPECT(RogueGift_DebugIsMoveInDynamicPool(sSynergyOnlyMoves[i]));
        EXPECT(!RogueGift_DebugIsMoveExotic(sSynergyOnlyMoves[i]));
    }

    EXPECT(!RogueGift_DebugIsMoveInDynamicPool(MOVE_MIRROR_COAT));
}

TEST("Dynamic general move index 207 decodes the expanded pool boundary")
{
    u32 customMonId = DynamicOriginalCustomMonId(207, 0, 0);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), 1);
    EXPECT_EQ(RogueGift_GetCustomMonMove(customMonId, 0), MOVE_BRAINSTORM);
}

TEST("Dynamic move pools preserve their shared serialized boundary")
{
    EXPECT_EQ(RogueGift_DebugGetDynamicExoticMoveCount(), 207);
    EXPECT_EQ(RogueGift_DebugGetDynamicMovePoolCount(), 254);
    EXPECT_EQ(RogueGift_DebugGetDynamicMoveByIndex(207), MOVE_BRAINSTORM);
    EXPECT_EQ(RogueGift_DebugGetDynamicMoveByIndex(208), MOVE_ACID_ARMOR);
    EXPECT_EQ(RogueGift_DebugGetDynamicMoveByIndex(254), MOVE_SWEET_KISS);
}

TEST("Expanded dynamic move selections decode high pairs in every payload format")
{
    u16 upper = RogueGift_DebugGetDynamicMovePoolCount();
    u32 originalId = DynamicOriginalCustomMonId(upper - 1, upper, 1);
    u32 typedId = DynamicTypeCustomMonId(TYPE_FIRE, 1, 1, upper - 1, upper, 0);
    u32 originalUniqueId = DynamicOriginalUniqueAbilityCustomMonId(upper - 1, upper, 1, ABILITY_STARMOBILE);
    u32 typedUniqueId = DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 1, 1, upper, 1, ABILITY_STARMOBILE);

    EXPECT_LE(upper, 255);
    EXPECT_EQ(RogueGift_DebugGetDynamicExoticMoveCount(), 207);

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 0), RogueGift_DebugGetDynamicMoveByIndex(upper - 1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonType(typedId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedId), 3);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper - 1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedId, 2), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(originalUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 0), RogueGift_DebugGetDynamicMoveByIndex(upper - 1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(originalUniqueId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(originalUniqueId), ABILITY_STARMOBILE);

    EXPECT_EQ(RogueGift_GetCustomMonType(typedUniqueId, 1), TYPE_FIRE);
    EXPECT_EQ(RogueGift_GetCustomMonMoveCount(typedUniqueId), 2);
    EXPECT_EQ(RogueGift_GetCustomMonMove(typedUniqueId, 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(typedUniqueId), ABILITY_STARMOBILE);
}

TEST("Generated dynamic general moves are distinct and exclude the species profile")
{
    u16 seed;
    u8 checkedCount = 0;

    for(seed = 0; seed < 256 && checkedCount < 32; ++seed)
    {
        u32 customMonId;
        u16 move1;
        u16 move2;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_EPIC, SPECIES_BULBASAUR);

        if(((customMonId >> 28) & 3) != 0)
            continue;

        move1 = RogueGift_GetCustomMonMove(customMonId, 0);
        move2 = RogueGift_GetCustomMonMove(customMonId, 1);
        EXPECT_NE(move1, move2);
        EXPECT(!RogueGift_DebugIsMoveNativeToEvolutionFamily(SPECIES_BULBASAUR, move1));
        EXPECT(!RogueGift_DebugIsMoveNativeToEvolutionFamily(SPECIES_BULBASAUR, move2));
        ++checkedCount;
    }

    EXPECT_EQ(checkedCount, 32);
}

TEST("Received movesets keep custom moves first without duplicates after evolution")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 0);
    struct Pokemon baseMon;
    struct Pokemon evolvedMon;
    u8 i;
    u8 j;

    RogueGift_CreateMon(customMonId, &baseMon, SPECIES_BULBASAUR, 50, USE_RANDOM_IVS);
    RogueGift_CreateMon(customMonId, &evolvedMon, SPECIES_VENUSAUR, 50, USE_RANDOM_IVS);

    EXPECT_EQ(GetMonData(&baseMon, MON_DATA_MOVE1), RogueGift_GetCustomMonMove(customMonId, 0));
    EXPECT_EQ(GetMonData(&baseMon, MON_DATA_MOVE2), RogueGift_GetCustomMonMove(customMonId, 1));
    EXPECT_EQ(GetMonData(&evolvedMon, MON_DATA_MOVE1), RogueGift_GetCustomMonMove(customMonId, 0));
    EXPECT_EQ(GetMonData(&evolvedMon, MON_DATA_MOVE2), RogueGift_GetCustomMonMove(customMonId, 1));

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 baseMove = GetMonData(&baseMon, MON_DATA_MOVE1 + i);
        u16 evolvedMove = GetMonData(&evolvedMon, MON_DATA_MOVE1 + i);

        for(j = i + 1; j < MAX_MON_MOVES; ++j)
        {
            if(baseMove != MOVE_NONE)
                EXPECT_NE(baseMove, GetMonData(&baseMon, MON_DATA_MOVE1 + j));
            if(evolvedMove != MOVE_NONE)
                EXPECT_NE(evolvedMove, GetMonData(&evolvedMon, MON_DATA_MOVE1 + j));
        }
    }
}

TEST("Evolved dynamic gifts can permanently relearn their custom moves")
{
    u32 customMonId = DynamicOriginalCustomMonId(1, 2, 0);
    struct Pokemon evolvedMon;
    u16 relearnableMoves[255];
    u16 replacementMove = MOVE_TACKLE;
    u8 relearnableCount;
    u8 customMove;
    u8 i;

    RogueGift_CreateMon(customMonId, &evolvedMon, SPECIES_VENUSAUR, 50, USE_RANDOM_IVS);
    for(i = 0; i < MAX_MON_MOVES; ++i)
        SetMonData(&evolvedMon, MON_DATA_MOVE1 + i, &replacementMove);

    relearnableCount = GetMoveRelearnerMoves(&evolvedMon, relearnableMoves);
    for(customMove = 0; customMove < RogueGift_GetCustomMonMoveCount(customMonId); ++customMove)
    {
        bool8 found = FALSE;

        for(i = 0; i < relearnableCount; ++i)
        {
            if(relearnableMoves[i] == RogueGift_GetCustomMonMove(customMonId, customMove))
                found = TRUE;
        }
        EXPECT(found);
    }
}

TEST("Generated abilities are non-native across rarity and evolution branches")
{
    static const u16 sSpecies[] =
    {
        SPECIES_BULBASAUR,
        SPECIES_EEVEE,
        SPECIES_RALTS,
    };
    static const u8 sRarities[] =
    {
        UNIQUE_RARITY_RARE,
        UNIQUE_RARITY_EPIC,
        UNIQUE_RARITY_LEGENDARY,
    };
    u8 speciesIndex;
    u8 rarityIndex;
    u16 seed;

    for(speciesIndex = 0; speciesIndex < ARRAY_COUNT(sSpecies); ++speciesIndex)
    {
        for(rarityIndex = 0; rarityIndex < ARRAY_COUNT(sRarities); ++rarityIndex)
        {
            for(seed = 0; seed < 32; ++seed)
            {
                u32 customMonId;
                u16 standardAbility;
                u16 uniqueAbility;

                SeedRng(seed);
                customMonId = RogueGift_CreateDynamicMonIdRaw(sRarities[rarityIndex], sSpecies[speciesIndex]);
                standardAbility = RogueGift_GetCustomMonAbility(customMonId, 0);
                uniqueAbility = RogueGift_GetCustomMonUniqueAbility(customMonId);

                if(standardAbility != ABILITY_NONE)
                    EXPECT(!RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(sSpecies[speciesIndex], standardAbility));
                if(uniqueAbility != ABILITY_NONE)
                    EXPECT(!RogueGift_DebugIsUniqueAbilityNativeToEvolutionFamily(sSpecies[speciesIndex], uniqueAbility));
            }
        }
    }
}

TEST("Generated altered type is distinct from both natural types")
{
    static const u16 sSpecies[] =
    {
        SPECIES_BULBASAUR,
        SPECIES_CHARIZARD,
        SPECIES_GYARADOS,
    };
    u8 speciesIndex;
    u16 seed;

    for(speciesIndex = 0; speciesIndex < ARRAY_COUNT(sSpecies); ++speciesIndex)
    {
        for(seed = 0; seed < 256; ++seed)
        {
            u32 customMonId;
            u8 format;
            u8 newType;

            SeedRng(seed);
            customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_COMMON, sSpecies[speciesIndex]);
            format = (customMonId >> 28) & 3;
            if(format != TEST_FORMAT_MON_TYPE)
                continue;

            newType = customMonId & 0x1F;
            EXPECT_NE(newType, gSpeciesInfo[sSpecies[speciesIndex]].types[0]);
            EXPECT_NE(newType, gSpeciesInfo[sSpecies[speciesIndex]].types[1]);
        }
    }
}

TEST("Every packed rarity format decodes low and upper move indices")
{
    u16 upper = RogueGift_DebugGetDynamicMovePoolCount();
    u32 ids[] =
    {
        DynamicOriginalCustomMonId(1, upper, 0),
        DynamicOriginalCustomMonId(upper, 0, 1),
        DynamicOriginalCustomMonId(1, upper, 1),
        DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 0, 0, 0),
        DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, 1, upper, 0),
        DynamicTypeCustomMonId(TYPE_FIRE, 0, 0, upper, 0, 1),
        DynamicOriginalUniqueAbilityCustomMonId(1, upper, 1, ABILITY_STARMOBILE),
        DynamicTypeUniqueAbilityCustomMonId(TYPE_FIRE, 0, 0, upper, 1, ABILITY_STARMOBILE),
    };

    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[0]), UNIQUE_RARITY_COMMON);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[0], 0), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[0], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[1]), UNIQUE_RARITY_RARE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[1], 0), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[2]), UNIQUE_RARITY_EPIC);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[2], 0), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[2], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[3]), UNIQUE_RARITY_COMMON);
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[4]), UNIQUE_RARITY_RARE);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[4], 1), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[4], 2), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[5]), UNIQUE_RARITY_EPIC);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[5], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));

    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[6]), UNIQUE_RARITY_LEGENDARY);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[6], 0), RogueGift_DebugGetDynamicMoveByIndex(1));
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[6], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
    EXPECT_EQ(RogueGift_GetCustomMonRarity(ids[7]), UNIQUE_RARITY_LEGENDARY);
    EXPECT_EQ(RogueGift_GetCustomMonMove(ids[7], 1), RogueGift_DebugGetDynamicMoveByIndex(upper));
}

TEST("Generated legendary standard ability seeds avoid native abilities")
{
    u16 seed;

    for(seed = 0; seed < 128; ++seed)
    {
        u32 customMonId;
        u16 generatedAbility;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_BULBASAUR);
        generatedAbility = RogueGift_GetCustomMonAbility(customMonId, 0);

        EXPECT_NE(generatedAbility, ABILITY_NONE);
        EXPECT(!RogueGift_DebugIsStandardAbilityNativeToEvolutionFamily(SPECIES_BULBASAUR, generatedAbility));
    }
}

TEST("Generated legendary unique ability never duplicates the species native unique ability")
{
    static const struct
    {
        u16 species;
        u16 uniqueAbility;
    } sCases[] =
    {
        { SPECIES_SHUCKLE,  ABILITY_SILVER_LINING },
        { SPECIES_RAYQUAZA, ABILITY_EMERALD_ACCORD },
        { SPECIES_LANDORUS, ABILITY_SAND_COMMAND },
        { SPECIES_MINIOR,   ABILITY_BLAST_SHIELD },
        { SPECIES_CLODSIRE, ABILITY_BOG_BODY },
    };
    u8 i;
    u16 seed;

    for(i = 0; i < ARRAY_COUNT(sCases); ++i)
    {
        for(seed = 0; seed < 128; ++seed)
        {
            u32 customMonId;

            SeedRng(seed);
            customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, sCases[i].species);
            EXPECT(!RogueGift_DebugIsUniqueAbilityNativeToEvolutionFamily(sCases[i].species, RogueGift_GetCustomMonUniqueAbility(customMonId)));
            EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(customMonId), sCases[i].uniqueAbility);
        }
    }
}

TEST("Generated legendary direct interactions are usable in both typing formats")
{
    bool8 sawOriginal = FALSE;
    bool8 sawTyped = FALSE;
    u16 seed;

    for(seed = 0; seed < 4096 && (!sawOriginal || !sawTyped); ++seed)
    {
        u32 customMonId;
        u16 ability;
        u8 format;
        u8 i;
        bool8 found = FALSE;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_DITTO);
        ability = RogueGift_GetCustomMonUniqueAbility(customMonId);
        if(RogueGift_DebugGetDynamicSynergyProfileId(ability) == 0)
            continue;

        for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
        {
            if(RogueGift_DebugDoesMoveMatchDynamicSynergy(ability, RogueGift_GetCustomMonMove(customMonId, i)))
                found = TRUE;
        }
        EXPECT(found);

        format = (customMonId >> 28) & 3;
        if(format == TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY)
            sawOriginal = TRUE;
        else if(format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
            sawTyped = TRUE;
    }

    EXPECT(sawOriginal);
    EXPECT(sawTyped);
}



TEST("Generated Creation legendary includes a status move matching its effective primary type")
{
    bool8 sawOriginal = FALSE;
    bool8 sawTyped = FALSE;
    u32 seed;

    for(seed = 0; seed < 8192 && (!sawOriginal || !sawTyped); ++seed)
    {
        u32 customMonId;
        u8 format;
        u8 creationType;
        u16 i;
        bool8 foundMove = FALSE;

        SeedRng(seed);
        customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_DITTO);
        if(RogueGift_GetCustomMonUniqueAbility(customMonId) != ABILITY_CREATION)
            continue;

        format = (customMonId >> 28) & 3;
        creationType = RogueGift_GetCustomMonType(customMonId, 0);
        if(creationType == TYPE_NONE)
            creationType = TYPE_NORMAL;
        for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
        {
            if(RogueGift_DebugDoesMoveMatchCreationSynergy(creationType, RogueGift_GetCustomMonMove(customMonId, i)))
                foundMove = TRUE;
        }

        EXPECT(foundMove);
        if(format == TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY)
            sawOriginal = TRUE;
        else if(format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
            sawTyped = TRUE;
    }

    EXPECT(sawOriginal);
    EXPECT(sawTyped);
}

TEST("Creation synergy profiles select valid moves for every supported type")
{
    static const bool8 sSupportedTypes[NUMBER_OF_MON_TYPES] =
    {
        [TYPE_NORMAL] = TRUE,
        [TYPE_FIGHTING] = TRUE,
        [TYPE_FLYING] = TRUE,
        [TYPE_FIRE] = TRUE,
        [TYPE_WATER] = TRUE,
        [TYPE_POISON] = TRUE,
        [TYPE_GROUND] = TRUE,
        [TYPE_ROCK] = TRUE,
        [TYPE_BUG] = TRUE,
        [TYPE_GHOST] = TRUE,
        [TYPE_STEEL] = TRUE,
        [TYPE_GRASS] = TRUE,
        [TYPE_ELECTRIC] = TRUE,
        [TYPE_PSYCHIC] = TRUE,
        [TYPE_ICE] = TRUE,
        [TYPE_DRAGON] = TRUE,
        [TYPE_DARK] = TRUE,
        [TYPE_FAIRY] = TRUE,
    };
    u8 type;

    for(type = 0; type < NUMBER_OF_MON_TYPES; ++type)
    {
        u16 move;

        SeedRng(type);
        move = RogueGift_DebugSelectCreationSynergyMove(SPECIES_DITTO, type, MOVE_NONE);
        if(sSupportedTypes[type])
            EXPECT(IsExpectedCreationSynergyMove(type, move));
        else
            EXPECT_EQ(move, MOVE_NONE);
    }
}

TEST("Dynamic generation round-trips every rarity and typing format")
{
    static const u8 sRarities[] =
    {
        UNIQUE_RARITY_COMMON,
        UNIQUE_RARITY_RARE,
        UNIQUE_RARITY_EPIC,
        UNIQUE_RARITY_LEGENDARY,
    };
    static const u8 sOriginalMoveCounts[] = { 2, 1, 2, 2 };
    static const u8 sTypedMoveCounts[] = { 1, 3, 2, 2 };
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sRarities); ++i)
    {
        bool8 sawOriginal = FALSE;
        bool8 sawTyped = FALSE;
        u16 seed;

        for(seed = 0; seed < 256 && (!sawOriginal || !sawTyped); ++seed)
        {
            u8 format;
            u32 customMonId;

            SeedRng(seed);
            customMonId = RogueGift_CreateDynamicMonIdRaw(sRarities[i], SPECIES_BULBASAUR);
            format = (customMonId >> 28) & 3;

            EXPECT_EQ(RogueGift_GetCustomMonRarity(customMonId), sRarities[i]);
            if(format == TEST_FORMAT_MON_TYPE || format == TEST_FORMAT_MON_TYPE_UNIQUE_ABILITY)
            {
                sawTyped = TRUE;
                EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), sTypedMoveCounts[i]);
            }
            else
            {
                sawOriginal = TRUE;
                EXPECT_EQ(RogueGift_GetCustomMonMoveCount(customMonId), sOriginalMoveCounts[i]);
            }
        }

        EXPECT(sawOriginal);
        EXPECT(sawTyped);
    }
}
