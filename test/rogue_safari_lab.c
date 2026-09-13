#include "global.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"
#include "constants/map_types.h"
#include "constants/metatile_behaviors.h"
#include "constants/songs.h"
#include "event_data.h"
#include "field_door.h"
#include "fieldmap.h"
#include "load_save.h"
#include "malloc.h"
#include "overworld.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_save.h"
#include "test/test.h"

#include "constants/event_objects.h"
#include "constants/species.h"
#include "constants/layouts.h"
#include "rogue_safari.h"
#include "rogue_followmon.h"

TEST("Safari lab: three exit lanes and cave gate work for every connection state")
{
    struct MapHeader oldHeader = gMapHeader;
    struct BackupMapLayout oldBackup = gBackupMapLayout;
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    u16 *grid = Alloc((38 + MAP_OFFSET_W) * (32 + MAP_OFFSET_H) * sizeof(u16));
    u8 tutorial, mask, cave, x, y;
    static const u8 bits[] = {1, 4, 2};
    static const u8 originals[] = {0, 4, 2};
    EXPECT(oldHub != NULL && grid != NULL);
    if (oldHub == NULL || grid == NULL) { Free(oldHub); Free(grid); return; }
    *oldHub = gRogueSaveBlock->hubMap;
    gBackupMapLayout.width = 38 + MAP_OFFSET_W;
    gBackupMapLayout.height = 32 + MAP_OFFSET_H;
    gBackupMapLayout.map = grid;
    for (tutorial = 0; tutorial < 2; ++tutorial)
    {
        const struct MapHeader *header = Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), tutorial ? MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL) : MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
        gMapHeader = *header;
        EXPECT_EQ(header->mapType, MAP_TYPE_INDOOR);
        for (mask = 0; mask < 8; ++mask)
        {
            memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
            RogueHub_BuildArea(HUB_AREA_SAFARI_ZONE, 0, 0);
            if (mask & 1) RogueHub_BuildArea(HUB_AREA_LABS, -1, 0);
            if (mask & 2) RogueHub_BuildArea(HUB_AREA_MARTS, 0, -1);
            if (mask & 4) RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, 1, 0);
            for (cave = 0; cave < 2; ++cave)
            {
                RogueHub_SetUpgrade(HUB_UPGRADE_SAFARI_ZONE_LEGENDS_CAVE, cave);
                for (y = 0; y < 32; ++y)
                    for (x = 0; x < 38; ++x)
                        grid[(y + MAP_OFFSET) * gBackupMapLayout.width + x + MAP_OFFSET] = header->mapLayout->map[y * 38 + x];
                RogueHub_ApplyMapMetatiles();
                for (y = 14; y <= 16; ++y)
                {
                    EXPECT_EQ(!!MapGridIsImpassableAt(4 + MAP_OFFSET, y + MAP_OFFSET), !(mask & 1));
                    EXPECT_EQ(!!MapGridIsImpassableAt(32 + MAP_OFFSET, y + MAP_OFFSET), !(mask & 4));
                    EXPECT_EQ(MapGridGetMetatileBehaviorAt(4 + MAP_OFFSET, y + MAP_OFFSET), mask & 1 ? MB_WEST_ARROW_WARP : MB_NORMAL);
                    EXPECT_EQ(MapGridGetMetatileBehaviorAt(32 + MAP_OFFSET, y + MAP_OFFSET), mask & 4 ? MB_EAST_ARROW_WARP : MB_NORMAL);
                }
                for (x = 17; x <= 19; ++x)
                {
                    EXPECT_EQ(!!MapGridIsImpassableAt(x + MAP_OFFSET, 27 + MAP_OFFSET), !(mask & 2));
                    EXPECT_EQ(MapGridGetMetatileBehaviorAt(x + MAP_OFFSET, 27 + MAP_OFFSET), mask & 2 ? MB_SOUTH_ARROW_WARP : MB_NORMAL);
                }
                EXPECT_EQ(!!MapGridIsImpassableAt(18 + MAP_OFFSET, 5 + MAP_OFFSET), !(cave && !tutorial));
                // Unlocking the cave must preserve the approach beneath it.
                for (y = 6; y <= 7; ++y)
                    EXPECT_EQ(MapGridGetMetatileIdAt(18 + MAP_OFFSET, y + MAP_OFFSET), header->mapLayout->map[y * 38 + 18] & MAPGRID_METATILE_ID_MASK);
                for (x = 0; x < 3; ++x)
                    if (mask & bits[x])
                    {
                        u8 index = (tutorial ? 6 : 8) + x;
                        struct WarpEvent added = header->events->warps[index];
                        struct WarpEvent original = header->events->warps[originals[x]];
                        RogueHub_ModifyMapWarpEvent(&gMapHeader, index, &added);
                        RogueHub_ModifyMapWarpEvent(&gMapHeader, originals[x], &original);
                        EXPECT_EQ(added.mapGroup, original.mapGroup);
                        EXPECT_EQ(added.mapNum, original.mapNum);
                        EXPECT_EQ(added.warpId, original.warpId);
                    }
            }
        }
    }
    gRogueSaveBlock->hubMap = *oldHub; gMapHeader = oldHeader; gBackupMapLayout = oldBackup;
    Free(grid); Free(oldHub);
}

TEST("Safari lab: buffer displays preserve identity, shrink after dismissal and keep follower slot")
{
    struct RogueSafariMon *oldMons = Alloc(sizeof(gRogueSaveBlock->safariMons));
    struct MapHeader oldHeader = gMapHeader;
    bool8 oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    bool8 oldFlags[4];
    u16 oldGraphics[6], selected[4];
    u8 i, j, index;
    static const u16 species[] = {SPECIES_BULBASAUR, SPECIES_CHARMANDER, SPECIES_SQUIRTLE, SPECIES_PIKACHU};
    EXPECT(oldMons != NULL);
    if (oldMons == NULL) return;
    memcpy(oldMons, gRogueSaveBlock->safariMons, sizeof(gRogueSaveBlock->safariMons));
    for (i = 0; i < 6; ++i) oldGraphics[i] = FollowMon_GetGraphics(i);
    for (i = 0; i < 4; ++i) oldFlags[i] = FlagGet(FLAG_TEMP_1 + i);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
    memset(gRogueSaveBlock->safariMons, 0, sizeof(gRogueSaveBlock->safariMons));
    RogueSafari_SetupLabDisplays();
    for (i = 0; i < 4; ++i) { EXPECT(FlagGet(FLAG_TEMP_1 + i)); EXPECT_EQ(RogueSafari_GetLabDisplayIndex(i), 255); }
    for (i = 0; i < 4; ++i) gRogueSaveBlock->safariMons[i * 5].species = species[i];
    gRogueSaveBlock->safariMons[5].shinyFlag = TRUE;
    gRogueSaveBlock->safariMons[ROGUE_SAFARI_LEGENDS_START_INDEX].species = SPECIES_MEWTWO;
    RogueSafari_SetupLabDisplays();
    for (i = 0; i < 4; ++i)
    {
        index = RogueSafari_GetLabDisplayIndex(i);
        EXPECT(index < ROGUE_SAFARI_LEGENDS_START_INDEX);
        EXPECT(!FlagGet(FLAG_TEMP_1 + i));
        selected[i] = index;
        for (j = 0; j < i; ++j) EXPECT(selected[j] != index);
        EXPECT_EQ(FollowMon_GetGraphics(i), (index == 15 ? SPECIES_PICHU : gRogueSaveBlock->safariMons[index].species) + (index == 5 ? FOLLOWMON_SHINY_OFFSET : 0));
    }
    for (i = 0; i < 4; ++i)
    {
        struct ObjectEvent display = {.localId = 4 + i, .graphicsId = OBJ_EVENT_GFX_FOLLOW_MON_0 + i};
        FollowMon_OnObjectEventRemoved(&display);
        FollowMon_OnObjectEventSpawned(&display);
    }
    FollowMon_OnWarp(); // Camera/warp bookkeeping must not reassign buffer identities.
    for (i = 0; i < 4; ++i) EXPECT_EQ(RogueSafari_GetLabDisplayIndex(i), selected[i]);
    RogueSafari_ClearSafariMonAtIdx(selected[1]);
    RogueSafari_CompactEmptyEntries();
    RogueSafari_SetupLabDisplays();
    for (i = 0; i < 3; ++i)
    {
        index = RogueSafari_GetLabDisplayIndex(i);
        EXPECT(index < 3);
        EXPECT(gRogueSaveBlock->safariMons[index].species != SPECIES_NONE);
        EXPECT(!FlagGet(FLAG_TEMP_1 + i));
    }
    EXPECT(FlagGet(FLAG_TEMP_4));
    EXPECT_EQ(RogueSafari_GetLabDisplayIndex(3), 255);
    EXPECT_EQ(FollowMon_GetGraphics(5), oldGraphics[5]);
    RogueSafari_ResetAllRegularEntries(); RogueSafari_SetupLabDisplays();
    for (i = 0; i < 4; ++i) EXPECT(FlagGet(FLAG_TEMP_1 + i));
    EXPECT_EQ((u16)gRogueSaveBlock->safariMons[ROGUE_SAFARI_LEGENDS_START_INDEX].species, SPECIES_MEWTWO);
    // A full buffer fills each enclosure once, without consuming any entries.
    for (i = 0; i < ROGUE_SAFARI_LEGENDS_START_INDEX; ++i) gRogueSaveBlock->safariMons[i].species = SPECIES_BULBASAUR;
    RogueSafari_SetupLabDisplays();
    for (i = 0; i < 4; ++i)
    {
        EXPECT(!FlagGet(FLAG_TEMP_1 + i));
        for (j = 0; j < i; ++j) EXPECT(RogueSafari_GetLabDisplayIndex(i) != RogueSafari_GetLabDisplayIndex(j));
    }
    RogueSafari_ResetAllRegularEntries();
    gRogueSaveBlock->safariMons[0].species = SPECIES_DONDOZO;
    FollowMon_SetGraphics(0, SPECIES_DONDOZO, FALSE, 0);
    EXPECT(GetFollowMonObjectEventInfo(OBJ_EVENT_GFX_FOLLOW_MON_0)->height > 32);
    RogueSafari_SetupLabDisplays();
    EXPECT(FlagGet(FLAG_TEMP_1));
    EXPECT_EQ((u16)gRogueSaveBlock->safariMons[0].species, SPECIES_DONDOZO); // Still available at the PC.
    memcpy(gRogueSaveBlock->safariMons, oldMons, sizeof(gRogueSaveBlock->safariMons)); Free(oldMons);
    for (i = 0; i < 6; ++i) FollowMon_SetGraphicsRaw(i, oldGraphics[i], 0);
    for (i = 0; i < 4; ++i) { if (oldFlags[i]) FlagSet(FLAG_TEMP_1 + i); else FlagClear(FLAG_TEMP_1 + i); }
    gMapHeader = oldHeader;
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE);
}

TEST("Safari lab: display objects are solid without changing tutorial encounter collision")
{
    struct MapHeader oldHeader = gMapHeader;
    struct ObjectEvent display = {.localId = 4, .graphicsId = OBJ_EVENT_GFX_FOLLOW_MON_0};
    struct ObjectEvent player = {.localId = OBJ_EVENT_ID_PLAYER};
    gMapHeader = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
    EXPECT(RogueSafari_IsLabDisplay(&display));
    EXPECT(!FollowMon_IsCollisionExempt(&display, &player));
    EXPECT(!FollowMon_IsCollisionExempt(&player, &display));
    gMapHeader.mapLayoutId = LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL;
    EXPECT(!RogueSafari_IsLabDisplay(&display));
    gMapHeader = oldHeader;
}

TEST("Safari lab: old outdoor saves recover according to catch progress without changing starters")
{
    struct ObjectEventTemplate objects[OBJECT_EVENT_TEMPLATES_COUNT];
    struct WarpData oldWarp = gSaveBlock1Ptr->continueGameWarp;
    struct RogueHubMap *oldHub = Alloc(sizeof(*oldHub));
    s16 oldX = gSaveBlock1Ptr->pos.x, oldY = gSaveBlock1Ptr->pos.y;
    u16 oldIntro = VarGet(VAR_ROGUE_INTRO_STATE), oldStarter = VarGet(VAR_ROGUE_STARTER0);
    bool8 oldStatus = UseContinueGameWarp(), oldRun = FlagGet(FLAG_ROGUE_RUN_ACTIVE);
    u8 count, direction, intro;
    EXPECT(oldHub != NULL);
    if (oldHub == NULL) return;
    *oldHub = gRogueSaveBlock->hubMap;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    gSaveBlock1Ptr->pos.x = 18; gSaveBlock1Ptr->pos.y = 18;
    VarSet(VAR_ROGUE_STARTER0, SPECIES_BULBASAUR);
    for (direction = 0; direction < 3; ++direction)
    {
        memset(gRogueSaveBlock->hubMap.areaBuiltFlags, 0, sizeof(gRogueSaveBlock->hubMap.areaBuiltFlags));
        RogueHub_BuildArea(HUB_AREA_SAFARI_ZONE, 0, 0);
        RogueHub_BuildArea(HUB_AREA_ADVENTURE_ENTRANCE, direction == 0 ? -1 : direction == 1 ? 0 : 1, direction == 1 ? -1 : 0);
        for (intro = ROGUE_INTRO_STATE_CATCH_MON; intro <= ROGUE_INTRO_STATE_COMPLETE; ++intro)
        {
            struct MapHeader header = *Overworld_GetMapHeaderByGroupAndId(MAP_GROUP(ROGUE_AREA_SAFARI_ZONE), intro <= ROGUE_INTRO_STATE_REPORT_TO_PROF ? MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL) : MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
            count = header.events->objectEventCount;
            memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
            for (u8 i = 0; i < count; ++i)
                if (objects[i].graphicsId == OBJ_EVENT_GFX_ADVENTURE_CONSOLE) objects[i].x = 3;
            VarSet(VAR_ROGUE_INTRO_STATE, intro); ClearContinueGameWarpStatus();
            Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
            EXPECT(UseContinueGameWarp());
            EXPECT_EQ(VarGet(VAR_ROGUE_INTRO_STATE), intro);
            EXPECT_EQ(VarGet(VAR_ROGUE_STARTER0), SPECIES_BULBASAUR);
            EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.mapNum, intro <= ROGUE_INTRO_STATE_REPORT_TO_PROF ? MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL) : MAP_NUM(ROGUE_AREA_SAFARI_ZONE));
            EXPECT_EQ(gSaveBlock1Ptr->continueGameWarp.warpId, intro == ROGUE_INTRO_STATE_CATCH_MON ? direction * 2 : intro == ROGUE_INTRO_STATE_REPORT_TO_PROF ? 9 : 7);
            memcpy(objects, header.events->objectEvents, count * sizeof(*objects));
            ClearContinueGameWarpStatus();
            Rogue_ModifyObjectEvents(&header, TRUE, objects, &count, ARRAY_COUNT(objects));
            EXPECT(!UseContinueGameWarp());
        }
    }
    gRogueSaveBlock->hubMap = *oldHub; Free(oldHub);
    gSaveBlock1Ptr->pos.x = oldX; gSaveBlock1Ptr->pos.y = oldY;
    VarSet(VAR_ROGUE_INTRO_STATE, oldIntro); VarSet(VAR_ROGUE_STARTER0, oldStarter);
    gSaveBlock1Ptr->continueGameWarp = oldWarp;
    if (oldStatus) SetContinueGameWarpStatus(); else ClearContinueGameWarpStatus();
    if (oldRun) FlagSet(FLAG_ROGUE_RUN_ACTIVE);
}
