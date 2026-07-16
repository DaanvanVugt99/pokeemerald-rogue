#include "global.h"
#include "constants/abilities.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_save.h"
#include "test/test.h"

static void SetUniqueLegendaryUpgradeState(bool8 state)
{
    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_RARE, state);
    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_EPIC, state);
    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY, state);
    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES, state);
}

static void ExpectGeneratedAbilityIsRerolled(u16 species, u32 customMonId)
{
    u8 i;
    u16 generatedAbility = RogueGift_GetCustomMonAbility(customMonId, 0);

    EXPECT_NE(generatedAbility, ABILITY_NONE);
    for(i = 0; i < NUM_ABILITY_SLOTS; ++i)
        EXPECT_NE(generatedAbility, GetAbilityBySpecies(species, i, 0));
}

static void ExpectNoDangerousLegendaryMoves(struct Pokemon *mon)
{
    u8 i;

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 move = GetMonData(mon, MON_DATA_MOVE1 + i);

        EXPECT_NE(move, MOVE_ROAR);
        EXPECT_NE(move, MOVE_WHIRLWIND);
        EXPECT_NE(move, MOVE_EXPLOSION);
        EXPECT_NE(move, MOVE_SELF_DESTRUCT);
        EXPECT_NE(move, MOVE_TELEPORT);
    }
}

TEST("A portal-pregenerated initial path is reused on map entry")
{
    u8 originalRoomCount = gRogueAdvPath.roomCount;
    bool8 originalJustGenerated = gRogueAdvPath.justGenerated;
    u8 originalRoomId = gRogueRun.adventureRoomId;

    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = TRUE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT_EQ(gRogueAdvPath.roomCount, 1);
    EXPECT(gRogueAdvPath.justGenerated);

    gRogueAdvPath.roomCount = originalRoomCount;
    gRogueAdvPath.justGenerated = originalJustGenerated;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("An exhausted path is replaced after its boss")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    u8 originalRoomId = gRogueRun.adventureRoomId;

    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = FALSE;
    gRogueAdvPath.rooms[0].roomType = ADVPATH_ROOM_BOSS;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;

    EXPECT(RogueAdv_GenerateAdventurePathsIfRequired());
    EXPECT(gRogueAdvPath.justGenerated);
    EXPECT_GT(gRogueAdvPath.roomCount, 1);

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Unique Legendary upgrade is the final Lab upgrade")
{
    u8 originalUpgradeFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags)];
    u8 originalAreaBuiltFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.areaBuiltFlags)];

    memcpy(originalUpgradeFlags, gRogueSaveBlock->hubMap.upgradeFlags, sizeof(originalUpgradeFlags));
    memcpy(originalAreaBuiltFlags, gRogueSaveBlock->hubMap.areaBuiltFlags, sizeof(originalAreaBuiltFlags));
    gRogueSaveBlock->hubMap.areaBuiltFlags[HUB_AREA_LABS / 8] |= 1 << (HUB_AREA_LABS % 8);

    EXPECT_EQ(ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags), 7);
    EXPECT_EQ(gRogueHubUpgrades[HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES].targetArea, HUB_AREA_LABS);
    EXPECT_EQ(gRogueHubUpgrades[HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES].buildCost, 12);
    EXPECT_EQ(gRogueHubUpgrades[HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES].requiredUpgrades[0], HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY);

    SetUniqueLegendaryUpgradeState(FALSE);
    EXPECT(!RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES));
    EXPECT(!RogueHub_HasUpgradeRequirements(HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES));

    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_RARITY_LEGENDARY, TRUE);
    EXPECT(RogueHub_HasUpgradeRequirements(HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES));

    RogueHub_SetUpgrade(HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES, TRUE);
    EXPECT(RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_LEGENDARIES));

    memcpy(gRogueSaveBlock->hubMap.upgradeFlags, originalUpgradeFlags, sizeof(originalUpgradeFlags));
    memcpy(gRogueSaveBlock->hubMap.areaBuiltFlags, originalAreaBuiltFlags, sizeof(originalAreaBuiltFlags));
}

TEST("Unique Legendary generation is gated, deterministic, and supports both rarities")
{
    u16 seed;
    u16 successSeed = 0;
    u16 nativeUniqueAbility = GetUniqueAbilityBySpecies(SPECIES_HO_OH);
    u32 customMonId;
    u32 epicId = 0;
    u32 legendaryId = 0;
    u32 replayId;
    bool8 foundFailure = FALSE;
    bool8 foundTwoSuccesses = FALSE;
    RAND_TYPE originalRng = gRngValue;
    RAND_TYPE originalRogueRng = gRngRogueValue;
    RAND_TYPE rngBefore;
    u8 originalUpgradeFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags)];

    memcpy(originalUpgradeFlags, gRogueSaveBlock->hubMap.upgradeFlags, sizeof(originalUpgradeFlags));
    SetUniqueLegendaryUpgradeState(FALSE);

    for(seed = 0; seed < 128; ++seed)
    {
        SeedRogueRng(seed);
        EXPECT_EQ(RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH), 0);
    }

    SetUniqueLegendaryUpgradeState(TRUE);
    for(seed = 0; seed < 1024 && (epicId == 0 || legendaryId == 0 || !foundFailure); ++seed)
    {
        SeedRogueRng(seed);
        customMonId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);

        if(customMonId == 0)
        {
            foundFailure = TRUE;
        }
        else
        {
            if(successSeed == 0)
                successSeed = seed;

            if(RogueGift_GetCustomMonRarity(customMonId) == UNIQUE_RARITY_EPIC)
                epicId = customMonId;
            else if(RogueGift_GetCustomMonRarity(customMonId) == UNIQUE_RARITY_LEGENDARY)
                legendaryId = customMonId;
        }
    }

    EXPECT(foundFailure);
    EXPECT_NE(epicId, 0);
    EXPECT_NE(legendaryId, 0);

    ExpectGeneratedAbilityIsRerolled(SPECIES_HO_OH, epicId);
    EXPECT_EQ(RogueGift_GetCustomMonUniqueAbility(epicId), ABILITY_NONE);
    EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(SPECIES_HO_OH, epicId), nativeUniqueAbility);

    ExpectGeneratedAbilityIsRerolled(SPECIES_HO_OH, legendaryId);
    EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(legendaryId), ABILITY_NONE);
    EXPECT_NE(RogueGift_GetCustomMonUniqueAbility(legendaryId), nativeUniqueAbility);

    SeedRng(12345);
    rngBefore = gRngValue;
    SeedRogueRng(successSeed);
    customMonId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);
    EXPECT_EQ(memcmp(&gRngValue, &rngBefore, sizeof(rngBefore)), 0);

    SeedRng(54321);
    SeedRogueRng(successSeed);
    replayId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);
    EXPECT_EQ(replayId, customMonId);

    for(seed = 0; seed < 4096 && !foundTwoSuccesses; ++seed)
    {
        SeedRogueRng(seed);
        customMonId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_HO_OH);
        replayId = RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(SPECIES_MEWTWO);
        foundTwoSuccesses = customMonId != 0 && replayId != 0;
    }
    EXPECT(foundTwoSuccesses);

    memcpy(gRogueSaveBlock->hubMap.upgradeFlags, originalUpgradeFlags, sizeof(originalUpgradeFlags));
    gRngValue = originalRng;
    gRngRogueValue = originalRogueRng;
}

TEST("Unique Legendary rooms use a gold statue")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    struct ObjectEventTemplate objectEvents[1] = {0};
    u8 objectEventCount = 0;

    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueAdvPath.roomCount = 1;
    gRogueAdvPath.justGenerated = TRUE;
    gRogueAdvPath.rooms[0].roomType = ADVPATH_ROOM_LEGENDARY;

    RogueAdv_ModifyObjectEvents(NULL, objectEvents, &objectEventCount, ARRAY_COUNT(objectEvents));
    EXPECT_EQ(objectEventCount, 1);
    EXPECT_EQ(objectEvents[0].graphicsId, OBJ_EVENT_GFX_TRICK_HOUSE_STATUE);

    gRogueAdvPath.rooms[0].roomParams.perType.legendary.customMonId = 1;
    RogueAdv_ModifyObjectEvents(NULL, objectEvents, &objectEventCount, ARRAY_COUNT(objectEvents));
    EXPECT_EQ(objectEventCount, 1);
    EXPECT_EQ(objectEvents[0].graphicsId, OBJ_EVENT_GFX_GOLD_LEGENDARY_STATUE);

    gRogueAdvPath = originalPath;
}

TEST("Unique Legendary battles and catches preserve their generated payload")
{
    u8 i;
    u8 rarities[] = { UNIQUE_RARITY_EPIC, UNIQUE_RARITY_LEGENDARY };
    bool8 wasRunActive = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    struct RogueAdvPath originalPath = gRogueAdvPath;
    RAND_TYPE originalRng = gRngValue;
    u8 originalUpgradeFlags[ARRAY_COUNT(gRogueSaveBlock->hubMap.upgradeFlags)];

    memcpy(originalUpgradeFlags, gRogueSaveBlock->hubMap.upgradeFlags, sizeof(originalUpgradeFlags));
    SetUniqueLegendaryUpgradeState(TRUE);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_LEGENDARY;

    for(i = 0; i < ARRAY_COUNT(rarities); ++i)
    {
        struct Pokemon mon;
        u32 customMonId;
        u32 shinyState = TRUE;

        SeedRng(100 + i);
        customMonId = RogueGift_CreateDynamicMonIdRaw(rarities[i], SPECIES_HO_OH);
        gRogueAdvPath.currentRoomParams.perType.legendary.customMonId = customMonId;

        CreateMon(&mon, SPECIES_HO_OH, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_RANDOM_NO_SHINY, 0);
        SetMonData(&mon, MON_DATA_IS_SHINY, &shinyState);
        Rogue_ModifyWildMon(&mon);

        EXPECT_EQ(RogueGift_GetCustomMonId(&mon), customMonId);
        EXPECT_EQ(RogueGift_GetCustomMonRarity(RogueGift_GetCustomMonId(&mon)), rarities[i]);
        EXPECT(GetMonData(&mon, MON_DATA_IS_SHINY));
        ExpectNoDangerousLegendaryMoves(&mon);

        Rogue_ModifyCaughtMon(&mon);
        EXPECT_EQ(RogueGift_GetCustomMonId(&mon), customMonId);
        EXPECT_EQ(RogueGift_GetCustomBoxMonId(&mon.box), customMonId);
    }

    gRogueAdvPath = originalPath;
    memcpy(gRogueSaveBlock->hubMap.upgradeFlags, originalUpgradeFlags, sizeof(originalUpgradeFlags));
    gRngValue = originalRng;
    if(!wasRunActive)
        FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
