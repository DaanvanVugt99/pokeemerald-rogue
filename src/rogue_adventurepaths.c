#include "global.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/layouts.h"
#include "constants/metatile_labels.h"
#include "constants/trainer_types.h"
#include "constants/rogue.h"
#include "gba/isagbprint.h"
#include "event_data.h"
#include "event_object_movement.h"
#include "fieldmap.h"
#include "field_screen_effect.h"
#include "item.h"
#include "malloc.h"
#include "overworld.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_debug.h"
#include "rogue_followmon.h"

#include "rogue_adventurepaths.h"
#include "rogue_campaign.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_route_events.h"
#include "rogue_route_scenes.h"


#define ROOM_TO_WORLD_X 3
#define ROOM_TO_WORLD_Y 2

#define PATH_MAP_OFFSET_X (4)
#define PATH_MAP_OFFSET_Y (4)

#define ADVENTURE_PATHS_MAP_WIDTH 44
#define ADVENTURE_PATHS_MAP_HEIGHT 44
#define ADVENTURE_PATHS_MAP_CELL_COUNT (ADVENTURE_PATHS_MAP_WIDTH * ADVENTURE_PATHS_MAP_HEIGHT)
#define ADVENTURE_PATHS_MASK_SIZE ((ADVENTURE_PATHS_MAP_CELL_COUNT + 3) / 4)
#define ADVENTURE_PATHS_LEVEL_SIZE ((ADVENTURE_PATHS_MAP_CELL_COUNT + 1) / 2)

#define ISLAND_MASK_TRAIL   (1 << 0)
#define ISLAND_MASK_BLOCKED (1 << 1)

#define ADVENTURE_ISLAND_MAX_EXPANSION 6
#define ADVENTURE_ISLAND_DISTANCE_UNREACHED 0xF
#define ADVENTURE_ISLAND_INNER_FRINGE_THRESHOLD 72
#define ADVENTURE_ISLAND_OUTER_FRINGE_THRESHOLD 150
#define ADVENTURE_TEMPLATE_OFFSET_RADIUS 2
#define ADVENTURE_FORMATION_BACKGROUND_COUNT 15
#define ADVENTURE_SATELLITE_MAX_COUNT 2
#define ADVENTURE_SATELLITE_MIN_WIDTH 2
#define ADVENTURE_SATELLITE_MAX_WIDTH 4
#define ADVENTURE_SATELLITE_HEIGHT 2
#define ADVENTURE_DEBRIS_MIN_COUNT 15
#define ADVENTURE_DEBRIS_MAX_COUNT 23
#define ADVENTURE_DEBRIS_SINGLE_VARIANT_COUNT 10
#define ADVENTURE_PERIPHERAL_PLACEMENT_ATTEMPTS 64

#define ADJUST_COORDS_X(val) (gRogueAdvPath.pathLength - val - 1)   // invert so we place the first node at the end
#define ADJUST_COORDS_Y(val) (val - gRogueAdvPath.pathMinY + 1)     // start at coord 0


#define ROOM_TO_METATILE_X(val) ((ADJUST_COORDS_X(val) * ROOM_TO_WORLD_X) + MAP_OFFSET + PATH_MAP_OFFSET_X)
#define ROOM_TO_METATILE_Y(val) ((ADJUST_COORDS_Y(val) * ROOM_TO_WORLD_Y) + MAP_OFFSET + PATH_MAP_OFFSET_Y)

#define ROOM_TO_MAP_X(val) (ROOM_TO_METATILE_X(val) - MAP_OFFSET)
#define ROOM_TO_MAP_Y(val) (ROOM_TO_METATILE_Y(val) - MAP_OFFSET)

#define ROOM_TO_OBJECT_EVENT_X(val) ((ADJUST_COORDS_X(val) * ROOM_TO_WORLD_X) + PATH_MAP_OFFSET_X + 2)
#define ROOM_TO_OBJECT_EVENT_Y(val) ((ADJUST_COORDS_Y(val) * ROOM_TO_WORLD_Y) + PATH_MAP_OFFSET_Y)

#define ROOM_TO_WARP_X(val) (ROOM_TO_OBJECT_EVENT_X(val) + 1)
#define ROOM_TO_WARP_Y(val) (ROOM_TO_OBJECT_EVENT_Y(val))

#define ROOM_CONNECTION_TOP     0
#define ROOM_CONNECTION_MID     1
#define ROOM_CONNECTION_BOT     2
#define ROOM_CONNECTION_COUNT   3

#define ROOM_CONNECTION_MASK_TOP     (1 << ROOM_CONNECTION_TOP)
#define ROOM_CONNECTION_MASK_MID     (1 << ROOM_CONNECTION_MID)
#define ROOM_CONNECTION_MASK_BOT     (1 << ROOM_CONNECTION_BOT)

#define MAX_CONNECTION_GENERATOR_COLUMNS 5

#define ITEM_ROOM_SCHEDULE_COUNT 3
#define ITEM_ROOM_SCHEDULE_SALT 0x4954454Du

#define gSpecialVar_ScriptNodeID        gSpecialVar_0x8004
#define gSpecialVar_ScriptNodeParam0    gSpecialVar_0x8005
#define gSpecialVar_ScriptNodeParam1    gSpecialVar_0x8006

struct AdvPathConnectionSettings
{
    u8 minCount;
    u8 maxCount;
    u8 branchingChance[ROOM_CONNECTION_COUNT];
};

struct AdvPathGenerator
{
    struct AdvPathConnectionSettings connectionsSettingsPerColumn[MAX_CONNECTION_GENERATOR_COLUMNS];
};

struct AdvPathRoomSettings
{
    struct Coords8 currentCoords;
    struct RogueAdvPathRoomParams roomParams;
    u16 rngSeed;
    u8 roomType;
    u8 connectionMask;
};

struct AdvPathSettings
{
    const struct AdvPathGenerator* generator;
    struct AdvPathRoomSettings roomScratch[ROGUE_ADVPATH_ROOM_CAPACITY];
    u8 numOfRooms[ADVPATH_ROOM_COUNT];
    struct Coords8 currentCoords;
    u8 totalLength;
    u8 nodeCount;
};

struct AdventureIslandVisualStats
{
    u16 keptInnerFringe;
    u16 prunedInnerFringe;
    u16 outerFringe;
    u16 narrowFringe;
};

struct AdventureSatelliteIsland
{
    u8 x;
    u8 y;
    u8 width;
};

struct AdventureFloatingDebris
{
    u8 x;
    u8 y;
    u8 width;
};

struct AdventureIslandVisualSettings
{
    const u16 *surfaceTemplate;
    u32 visualHash;
    s8 templateOffsetX;
    s8 templateOffsetY;
    u8 satelliteCount;
    u8 debrisCount;
    struct AdventureSatelliteIsland satellites[ADVENTURE_SATELLITE_MAX_COUNT];
    struct AdventureFloatingDebris debris[ADVENTURE_DEBRIS_MAX_COUNT];
};

static EWRAM_DATA u8 *sAdventureIslandMask = NULL;
static EWRAM_DATA u8 *sAdventureIslandLevels = NULL;

static bool8 IsObjectEventVisible(struct RogueAdvPathRoom* room);
static bool8 ShouldBlockObjectEvent(struct RogueAdvPathRoom* room);
static void BufferTypeAdjective(u8 type);
static void CacheAdventureIslandSurface(struct AdventureIslandVisualStats *visualStats);
static void BuildAdventurePeripheralPlan(void);
static u16 GetAdventureSurfaceTemplateMetatile(s16 x, s16 y);
static bool8 IsAdventureFormationMetatile(u16 metatile);

static void GeneratePath(struct AdvPathSettings* pathSettings);
static void GenerateFloorLayout(struct Coords8 currentCoords, struct AdvPathSettings* pathSettings);
static void GenerateRoomPlacements(struct AdvPathSettings* pathSettings);
static void AssignRoomInstance(struct AdvPathSettings* pathSettings, u8 roomId, u8 roomType);
static void MaterializePath(struct AdvPathSettings* pathSettings);
static void MaterializeRoom(u8 roomId);
static void EnsureStandardPathRivalStartingLevelingRoom(struct AdvPathSettings* pathSettings);
static u8 CountRoomConnections(u8 mask);

static u8 GenerateRoomConnectionMask(struct Coords8 coords, struct AdvPathSettings* pathSettings);
static bool8 DoesRoomExists(s8 x, s8 y, struct AdvPathSettings* pathSettings);

static u16 SelectObjectGfxForRoom(struct RogueAdvPathRoom* room);
static u8 SelectObjectMovementTypeForRoom(struct RogueAdvPathRoom* room);

static u8 GetPathGenerationDifficulty()
{
    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
    {
        if(Rogue_GetCurrentDifficulty() == 0)
        {
            // Generate full path under late game difficulty balance
            return ROGUE_ELITE_START_DIFFICULTY - 1;
        }
    }

    // Skip ahead for the fake out
    if(Rogue_AssumeFinalQuestFakeChamp())
        return Rogue_GetCurrentDifficulty() + 1;
    else
        return Rogue_GetCurrentDifficulty();
}

struct ItemRoomScheduleEntry
{
    u16 itemId;
    u8 difficulty;
};

static u32 AdvanceItemRoomScheduleRng(u32 *state)
{
    *state = *state * 1664525u + 1013904223u;
    return *state;
}

static void GetItemRoomSchedule(struct ItemRoomScheduleEntry *schedule)
{
    u16 items[ITEM_ROOM_REWARD_COUNT] =
    {
        ITEM_CURSED_LENS,
        ITEM_VOW_OF_SILENCE,
        ITEM_BLOOD_OATH,
        ITEM_HOLLOW_SUN,
        ITEM_MALICE_ORB,
        ITEM_GRAVEGLASS,
        ITEM_ASHEN_CROWN,
        ITEM_WITCHS_THREAD,
        ITEM_PETRIFIED_HEART,
        ITEM_FALSE_IDOL,
        ITEM_RUSTED_ANCHOR,
        ITEM_GAMBLERS_CLAW,
        ITEM_TEMPO_DIAL,
        ITEM_TURNABOUT_TOTEM,
        ITEM_JESTER_SWITCH,
        ITEM_WAYWARD_INCENSE,
        ITEM_CHAOS_CHARM,
        ITEM_MISCHIEF_QUILL,
        ITEM_FINALE_BELL,
        ITEM_ECLIPSE_TOTEM,
        ITEM_SUN_TOTEM,
        ITEM_RAIN_TOTEM,
        ITEM_SAND_TOTEM,
        ITEM_SNOW_TOTEM,
        ITEM_ACID_RAIN_TOTEM,
        ITEM_ELECTRIC_TIKI,
        ITEM_GRASSY_TIKI,
        ITEM_MISTY_TIKI,
        ITEM_PSYCHIC_TIKI,
        ITEM_GLASS_SWORD,
        ITEM_WONDER_SHIELD,
        ITEM_ECHO_SCEPTER,
        ITEM_GLYPH_CODEX,
        ITEM_PRECISE_LENS,
        ITEM_FURY_MANTLE,
        ITEM_COMPOUND_GOGGLES,
        ITEM_GREEDY_GLOVES,
        ITEM_IMPACT_PLATING,
        ITEM_CHIME_JEWEL,
        ITEM_AMBUSH_TALON,
        ITEM_PURITY_JEWEL,
    };
    u32 state = ((u32)gRogueRun.baseSeed << 16)
        ^ gRogueRun.baseSeed
        ^ ((u32)Rogue_GetConfigRange(CONFIG_RANGE_GAME_MODE_NUM) << 8)
        ^ ITEM_ROOM_SCHEDULE_SALT;
    bool8 hasBonusRoom;
    u8 i;

    for(i = 0; i < ITEM_ROOM_SCHEDULE_COUNT; ++i)
    {
        schedule[i].itemId = ITEM_NONE;
        schedule[i].difficulty = ROGUE_MAX_BOSS_COUNT;
    }

    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
        return;

    schedule[0].difficulty = 1 + AdvanceItemRoomScheduleRng(&state) % 7;
    hasBonusRoom = (AdvanceItemRoomScheduleRng(&state) & 1) != 0;

    if(hasBonusRoom)
    {
        schedule[1].difficulty = 1 + AdvanceItemRoomScheduleRng(&state) % 6;
        if(schedule[1].difficulty >= schedule[0].difficulty)
            ++schedule[1].difficulty;
    }

    schedule[2].difficulty = 9 + AdvanceItemRoomScheduleRng(&state) % 4;

    for(i = ITEM_ROOM_REWARD_COUNT - 1; i != 0; --i)
    {
        u8 other = AdvanceItemRoomScheduleRng(&state) % (i + 1);
        u16 temp = items[i];
        items[i] = items[other];
        items[other] = temp;
    }

    for(i = 0; i < ITEM_ROOM_SCHEDULE_COUNT; ++i)
    {
        if(schedule[i].difficulty != ROGUE_MAX_BOSS_COUNT)
            schedule[i].itemId = items[i];
    }
}

static bool8 GetScheduledItemRoom(u8 difficulty, u8 *scheduleSlot, u16 *itemId)
{
    struct ItemRoomScheduleEntry schedule[ITEM_ROOM_SCHEDULE_COUNT];
    u8 i;

    GetItemRoomSchedule(schedule);

    for(i = 0; i < ITEM_ROOM_SCHEDULE_COUNT; ++i)
    {
        if(schedule[i].difficulty == difficulty)
        {
            *scheduleSlot = i;
            *itemId = schedule[i].itemId;
            return TRUE;
        }
    }

    return FALSE;
}

#ifdef ROGUE_DEBUG
static bool8 GetDebugForcedItemRoom(u8 *scheduleSlot, u16 *itemId)
{
    struct ItemRoomScheduleEntry schedule[ITEM_ROOM_SCHEDULE_COUNT];
    u8 i;

    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET
        || RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_ITEM_ROOM) == 0)
        return FALSE;

    // Use the first active scheduled reward so the debug room still consumes
    // a real schedule slot and cannot duplicate a normal Item Room later.
    GetItemRoomSchedule(schedule);
    for(i = 0; i < ITEM_ROOM_SCHEDULE_COUNT; ++i)
    {
        if(schedule[i].itemId != ITEM_NONE)
        {
            if(scheduleSlot != NULL)
                *scheduleSlot = i;
            if(itemId != NULL)
                *itemId = schedule[i].itemId;
            return TRUE;
        }
    }

    return FALSE;
}
#endif

static u16 GetItemRoomClaimFlag(u8 scheduleSlot)
{
    switch(scheduleSlot)
    {
    case 0:
        return FLAG_ROGUE_ITEM_ROOM_CLAIMED_0;
    case 1:
        return FLAG_ROGUE_ITEM_ROOM_CLAIMED_1;
    case 2:
        return FLAG_ROGUE_ITEM_ROOM_CLAIMED_2;
    default:
        return 0;
    }
}

bool8 RogueAdv_IsItemRoomRewardClaimed(u8 scheduleSlot)
{
    u16 flag = GetItemRoomClaimFlag(scheduleSlot);

    return flag != 0 && FlagGet(flag);
}

bool8 RogueAdv_TryClaimItemRoomReward(u8 scheduleSlot, u16 itemId)
{
    u16 flag = GetItemRoomClaimFlag(scheduleSlot);

    if(flag == 0 || !Rogue_IsItemRoomReward(itemId) || FlagGet(flag))
        return FALSE;

    if(!AddBagItem(itemId, 1))
        return FALSE;

    FlagSet(flag);
    return TRUE;
}

#ifdef ROGUE_DEBUG
bool8 RogueAdv_Debug_GetItemRoomSchedule(u8 slot, u8 *difficulty, u16 *itemId)
{
    struct ItemRoomScheduleEntry schedule[ITEM_ROOM_SCHEDULE_COUNT];

    if(slot >= ITEM_ROOM_SCHEDULE_COUNT)
        return FALSE;

    GetItemRoomSchedule(schedule);
    *difficulty = schedule[slot].difficulty;
    *itemId = schedule[slot].itemId;
    return schedule[slot].difficulty != ROGUE_MAX_BOSS_COUNT;
}
#endif

void RogueAdv_CacheMiniBossPreviews(void)
{
    u8 i;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_MINIBOSS
            && !gRogueAdvPath.rooms[i].roomParams.perType.miniboss.hasRewardPreview)
            Rogue_CacheMiniBossPreview(i);
    }
}

static void GeneratePath(struct AdvPathSettings* pathSettings)
{
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 assignmentStartClock;
    u32 materializationStartClock;
#endif

    AGB_ASSERT(pathSettings->generator != NULL);

    memset(pathSettings->roomScratch, 0, sizeof(pathSettings->roomScratch));
    memset(pathSettings->numOfRooms, 0, sizeof(pathSettings->numOfRooms));
    pathSettings->nodeCount = 0;

    // First assign the layout and room choices in the compact scratch buffer.
    // Expensive room payloads are materialized only after all replacements are
    // complete.
    gRogueAdvPath.pathLength = pathSettings->totalLength;
    {
        struct Coords8 coords = {0, 0};

#ifdef DEBUG_FEATURE_FRAME_TIMERS
        assignmentStartClock = RogueDebug_SampleClock();
#endif
        GenerateFloorLayout(coords, pathSettings);
        GenerateRoomPlacements(pathSettings);
#ifdef DEBUG_FEATURE_FRAME_TIMERS
        materializationStartClock = RogueDebug_SampleClock();
        DebugPrintf("[Run Load] Path assignment: %d us", RogueDebug_ClockToDisplayUnits(materializationStartClock - assignmentStartClock));
#endif
    }

    // Store min/max Y coords
    {
        u8 i;

        for(i = 0; i < pathSettings->nodeCount; ++i)
        {
            if(i == 0)
            {
                gRogueAdvPath.pathMinY = pathSettings->roomScratch[i].currentCoords.y;
                gRogueAdvPath.pathMaxY = pathSettings->roomScratch[i].currentCoords.y;
            }
            else
            {
                gRogueAdvPath.pathMinY = min(gRogueAdvPath.pathMinY, pathSettings->roomScratch[i].currentCoords.y);
                gRogueAdvPath.pathMaxY = max(gRogueAdvPath.pathMaxY, pathSettings->roomScratch[i].currentCoords.y);
            }
        }
    }

#ifdef DEBUG_FEATURE_FRAME_TIMERS
    materializationStartClock = RogueDebug_SampleClock();
#endif
    MaterializePath(pathSettings);
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Run Load] Path materialization: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - materializationStartClock));
#endif
}

static void GenerateFloorLayout(struct Coords8 currentCoords, struct AdvPathSettings* pathSettings)
{
    if(pathSettings->nodeCount >= ROGUE_ADVPATH_ROOM_CAPACITY)
    {
        // Cannot generate any more
        DebugPrint("ADVPATH: \tReached room/node capacity.");
        return;
    }
    else
    {
        u8 nodeId = pathSettings->nodeCount++;
        struct AdvPathRoomSettings* room = &pathSettings->roomScratch[nodeId];

        // Write base settings for this room (These will likely be overriden later)
        room->currentCoords = currentCoords;
        room->roomType = ADVPATH_ROOM_NONE;
        room->connectionMask = 0;
        room->rngSeed = RogueRandom();

        
        // Generate children
        //
        if(currentCoords.x + 1 < pathSettings->totalLength)
        {
            struct Coords8 newCoords;
            u8 connectionMask;

            newCoords.x = currentCoords.x + 1;
            newCoords.y = currentCoords.y;

            connectionMask = GenerateRoomConnectionMask(currentCoords, pathSettings);
            room->connectionMask = connectionMask;

            newCoords.y = currentCoords.y + 1;
            if((connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && !DoesRoomExists(newCoords.x, newCoords.y, pathSettings))
            {
                GenerateFloorLayout(newCoords, pathSettings);
            }
            
            newCoords.y = currentCoords.y + 0;
            if((connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && !DoesRoomExists(newCoords.x, newCoords.y, pathSettings))
            {
                GenerateFloorLayout(newCoords, pathSettings);
            }

            newCoords.y = currentCoords.y - 1;
            if((connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && !DoesRoomExists(newCoords.x, newCoords.y, pathSettings))
            {
                GenerateFloorLayout(newCoords, pathSettings);
            }
        }
    }
}

static bool8 IsPrecededByRoomType(struct AdvPathSettings* pathSettings, struct AdvPathRoomSettings* room, u8 roomType)
{
    u8 i;

    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        struct AdvPathRoomSettings* nextRoom = &pathSettings->roomScratch[i];

        if(nextRoom->currentCoords.x == room->currentCoords.x + 1)
        {
            // ROOM_CONNECTION_MASK_TOP
            if((room->connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && nextRoom->currentCoords.y == room->currentCoords.y + 1)
            {
                if(nextRoom->roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_MID
            else if((room->connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && nextRoom->currentCoords.y == room->currentCoords.y + 0)
            {
                if(nextRoom->roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_BOT
            else if((room->connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && nextRoom->currentCoords.y == room->currentCoords.y - 1)
            {
                if(nextRoom->roomType == roomType)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

static bool8 IsProceededByRoomType(struct AdvPathSettings* pathSettings, struct AdvPathRoomSettings* room, u8 roomType)
{
    u8 i;

    if(room->currentCoords.x == 0)
        return FALSE;
    else if(room->currentCoords.x == 1)
        return roomType == ADVPATH_ROOM_BOSS;

    // Check the inverse mask to see if we are connected
    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        struct AdvPathRoomSettings* previousRoom = &pathSettings->roomScratch[i];

        if(previousRoom->currentCoords.x == room->currentCoords.x - 1)
        {
            // ROOM_CONNECTION_MASK_TOP
            if((previousRoom->connectionMask & ROOM_CONNECTION_MASK_BOT) != 0 && previousRoom->currentCoords.y == room->currentCoords.y + 1)
            {
                if(previousRoom->roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_MID
            else if((previousRoom->connectionMask & ROOM_CONNECTION_MASK_MID) != 0 && previousRoom->currentCoords.y == room->currentCoords.y + 0)
            {
                if(previousRoom->roomType == roomType)
                    return TRUE;
            }
            // ROOM_CONNECTION_MASK_BOT
            else if((previousRoom->connectionMask & ROOM_CONNECTION_MASK_TOP) != 0 && previousRoom->currentCoords.y == room->currentCoords.y - 1)
            {
                if(previousRoom->roomType == roomType)
                    return TRUE;
            }
        }
    }

    return FALSE;
}

static u8 CountRoomType(struct AdvPathSettings* pathSettings, u16 roomType)
{
    return pathSettings->numOfRooms[roomType];
}

static u8 CountSubRoomType(struct AdvPathSettings* pathSettings, u16 roomType, u16 roomIndex)
{
    u8 i;
    u8 count = 0;

    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        if(pathSettings->roomScratch[i].roomType == roomType && pathSettings->roomScratch[i].roomParams.roomIdx == roomIndex)
            ++count;
    }

    return count;
}

static u8 SelectRoomType_CalculateWeight(u16 weightIndex, u16 roomType, void* data)
{
    struct AdvPathSettings* pathSettings = data;
    u8 count;

    switch (roomType)
    {
    case ADVPATH_ROOM_RESTSTOP:
        count = CountRoomType(pathSettings, roomType);

        // Always want at least 1 rest stop
        if(count == 0)
            return 100;
        // Prefer a 2nd rest stop
        else if(count == 1)
            return 20;
        // If we already have 4 perfer most other encounters
        else if(count >= 4)
            return 1;
        break;

    // Only allow 1 but we really want to place it
    case ADVPATH_ROOM_LEGENDARY:
    case ADVPATH_ROOM_UNIQUE_DEN:
        count = CountRoomType(pathSettings, roomType);
        if(count == 0)
            return 200;
        else
            return 0;
        break;

    // Only allow 1 but we really want to place it (less so than other encounters)
    case ADVPATH_ROOM_TEAM_HIDEOUT:
        count = CountRoomType(pathSettings, roomType);
        if(count == 0)
            return 50;
        else
            return 0;
        break;

    // Only allow 1 but we prefer it over others
    case ADVPATH_ROOM_HONEY_TREE:
        count = CountRoomType(pathSettings, roomType);
        if(count == 0)
        {
            // Every other badge we want to increase weight otherwise decrease weight but not impossible
            if((GetPathGenerationDifficulty() - 1) % 2 == 0)
                return 15;
            else
                return 1;
        }
        else
            return 0;
        break;

    // Only allow 1 and cycle weighting every third difficulty
    case ADVPATH_ROOM_DARK_DEAL:
        count = CountRoomType(pathSettings, roomType);
        if(count != 0)
            return 0;
        else if((GetPathGenerationDifficulty() % 2) != 0)
            return 15;
        else
            return 1;
        break;

    // Only allow 1 and cycle weighting every third difficulty (offset from dark deal rates)
    case ADVPATH_ROOM_LAB:
        count = CountRoomType(pathSettings, roomType);
        if(count != 0)
            return 0;
        else if(((GetPathGenerationDifficulty() + 1) % 2) != 0)
            return 20;
        else
            return 1;
        break;


    // Only allow 1 of this type at once
    case ADVPATH_ROOM_GAMESHOW:
    case ADVPATH_ROOM_CATCHING_CONTEST:
    case ADVPATH_ROOM_SIGN:
    case ADVPATH_ROOM_BATTLE_SIM:
    case ADVPATH_ROOM_BATTLE_TOWER:
        if(roomType == ADVPATH_ROOM_BATTLE_SIM || roomType == ADVPATH_ROOM_BATTLE_TOWER)
            count = CountRoomType(pathSettings, ADVPATH_ROOM_BATTLE_SIM) + CountRoomType(pathSettings, ADVPATH_ROOM_BATTLE_TOWER);
        else
            count = CountRoomType(pathSettings, roomType);
        if(count != 0)
            return 0;
        break;

    // We really want this to spawn when we allow it to
    case ADVPATH_ROOM_SHRINE:
        count = CountRoomType(pathSettings, roomType);
        if(count != 0)
            return 0;
        return 100;

    default:
        AGB_ASSERT(FALSE);
        break;
    }

    return 5;
}

static u16 SelectRoomType(u16* activeTypeBuffer, u16 activeTypeCount, struct AdvPathSettings* pathSettings)
{
    u16 i;
    u16 result;

    RogueCustomQuery_Begin();

    for(i = 0; i < activeTypeCount; ++i)
        RogueMiscQuery_EditElement(QUERY_FUNC_INCLUDE, activeTypeBuffer[i]);

    RogueWeightQuery_Begin();
    {
        RogueWeightQuery_CalculateWeights(SelectRoomType_CalculateWeight, pathSettings);
        result = RogueWeightQuery_SelectRandomFromWeights(RogueRandom());
    }
    RogueWeightQuery_End();

    RogueCustomQuery_End();

    return result;
}

struct ReplaceRoomEncounterSettings
{
    struct AdvPathSettings* pathSettings;
    u8 roomType;
};

static u8 ReplaceRoomEncounters_CalculateWeight(u16 weightIndex, u16 roomId, void* data)
{
    struct ReplaceRoomEncounterSettings* settings = data;
    s16 weight = 10;
    u8 roomType = settings->roomType;
    struct AdvPathRoomSettings* existingRoom = &settings->pathSettings->roomScratch[roomId];

    switch (roomType)
    {
    case ADVPATH_ROOM_RESTSTOP:
        // Like being placed in the final column but can occasionally end up in other one
        if(existingRoom->currentCoords.x <= 2)
            weight += 90;

        // Don't want to place in first column
        if(existingRoom->currentCoords.x + 1 == settings->pathSettings->totalLength)
            weight -= 40;

        // Don't place after or before or other rest stop
        if(IsPrecededByRoomType(settings->pathSettings, existingRoom, ADVPATH_ROOM_RESTSTOP) || IsProceededByRoomType(settings->pathSettings, existingRoom, ADVPATH_ROOM_RESTSTOP))
            weight = 0;
   
        if(IsPrecededByRoomType(settings->pathSettings, existingRoom, ADVPATH_ROOM_LEGENDARY) || IsProceededByRoomType(settings->pathSettings, existingRoom, ADVPATH_ROOM_LEGENDARY))
            weight = 0;
        break;

    case ADVPATH_ROOM_LEGENDARY:
    case ADVPATH_ROOM_UNIQUE_DEN:
        // Like being placed in the final column but can occasionally end up in other one
        if(existingRoom->currentCoords.x <= 2)
            weight += 80;

        // Don't want to place in first column
        if(existingRoom->currentCoords.x + 1 == settings->pathSettings->totalLength)
            weight -= 40;

        // Prefer route where we are locked into this path
        if(CountRoomConnections(existingRoom->connectionMask) == 1)
            weight += 40;

        // We like having the legend be behind the team hideout
        if(roomType == ADVPATH_ROOM_LEGENDARY && IsPrecededByRoomType(settings->pathSettings, existingRoom, ADVPATH_ROOM_TEAM_HIDEOUT))
            weight += 200;
        break;

    case ADVPATH_ROOM_TEAM_HIDEOUT:
        // Don't want to place in final column
        if(existingRoom->currentCoords.x <= 2)
            weight -= 40;

        // Prefer route where we are locked into this path
        if(CountRoomConnections(existingRoom->connectionMask) == 1)
            weight += 10;

        // We like having the legend be behind the team hideout
        if(IsProceededByRoomType(settings->pathSettings, existingRoom, ADVPATH_ROOM_LEGENDARY))
            weight += 200;
        break;

    case ADVPATH_ROOM_SHRINE:
    case ADVPATH_ROOM_LAB:
        // Like being placed in the final column but can occasionally end up in other one
        if(existingRoom->currentCoords.x <= 2)
            weight += 80;
        break;

    case ADVPATH_ROOM_CATCHING_CONTEST:
    case ADVPATH_ROOM_GAMESHOW:
    case ADVPATH_ROOM_BATTLE_SIM:
    case ADVPATH_ROOM_BATTLE_TOWER:
    case ADVPATH_ROOM_MINIBOSS:
    case ADVPATH_ROOM_ITEM:
        // Don't want to place in first column
        if(existingRoom->currentCoords.x + 1 == settings->pathSettings->totalLength)
            weight -= 40;
        // Like being placed in the middle columns but can occasionally end up in other one
        else if(existingRoom->currentCoords.x > 2)
            weight += 80;
        break;

    case ADVPATH_ROOM_SIGN:
        // Prefer being placed in first column
        if(existingRoom->currentCoords.x + 1 == settings->pathSettings->totalLength)
            weight += 80;

        // Like being placed in the middle columns but can occasionally end up in other one
        if(existingRoom->currentCoords.x > 2)
            weight += 40;
        break;
    }

    // If we've got this encounter immediately before or after prefer not this one
    if(IsPrecededByRoomType(settings->pathSettings, existingRoom, roomType))
        weight -= 5;
    if(IsProceededByRoomType(settings->pathSettings, existingRoom, roomType))
        weight -= 5;


    return (u8)(min(255, max(0, weight)));
}

static bool8 ReplaceRoomEncounter(struct AdvPathSettings* pathSettings, u8 fromRoomType, u8 toRoomType)
{
    u8 candidateIds[ROGUE_ADVPATH_ROOM_CAPACITY];
    u8 weights[ROGUE_ADVPATH_ROOM_CAPACITY];
    struct ReplaceRoomEncounterSettings settings = {pathSettings, toRoomType};
    u8 candidateCount = 0;
    u8 i;
    u16 totalWeight = 0;

    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        if(pathSettings->roomScratch[i].roomType == fromRoomType)
        {
            candidateIds[candidateCount] = i;
            weights[candidateCount] = ReplaceRoomEncounters_CalculateWeight(candidateCount, i, &settings);
            totalWeight += weights[candidateCount];
            ++candidateCount;
        }
    }

    if(totalWeight != 0)
    {
        u16 targetWeight = RogueRandom() % totalWeight;

        for(i = 0; i < candidateCount; ++i)
        {
            if(targetWeight < weights[i])
            {
                AssignRoomInstance(pathSettings, candidateIds[i], toRoomType);
                return TRUE;
            }

            targetWeight -= weights[i];
        }

        AGB_ASSERT(FALSE);
        return TRUE;
    }

    return FALSE;
}

static bool8 StandardPathAreRoutesHidden()
{
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_STANDARD)
        return FALSE;

    if(GetPathGenerationDifficulty() >= ROGUE_CHAMP_START_DIFFICULTY)
        return FALSE;

    return (GetPathGenerationDifficulty() % 2) == 1;
}

static bool8 IsStartingPathRoom(struct AdvPathSettings* pathSettings, struct AdvPathRoomSettings* room)
{
    return room->currentCoords.x + 1 == pathSettings->totalLength;
}

static bool8 IsLevelingRoomType(u16 roomType)
{
    return roomType == ADVPATH_ROOM_ROUTE || roomType == ADVPATH_ROOM_HONEY_TREE;
}

static bool8 IsStandardPathRivalLevelingFallbackType(u16 roomType)
{
    switch(roomType)
    {
    case ADVPATH_ROOM_SIGN:
    case ADVPATH_ROOM_GAMESHOW:
    case ADVPATH_ROOM_BATTLE_SIM:
    case ADVPATH_ROOM_BATTLE_TOWER:
    case ADVPATH_ROOM_CATCHING_CONTEST:
    case ADVPATH_ROOM_DARK_DEAL:
    case ADVPATH_ROOM_LAB:
        return TRUE;
    }

    return FALSE;
}

static void EnsureStandardPathRivalStartingLevelingRoom(struct AdvPathSettings* pathSettings)
{
    u8 i;
    u8 fallbackRoom = (u8)-1;
    u8 backupFallbackRoom = (u8)-1;

    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_STANDARD)
        return;

    if(!gRogueRun.hasPendingRivalBattle)
        return;

    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        struct AdvPathRoomSettings* room = &pathSettings->roomScratch[i];

        if(!IsStartingPathRoom(pathSettings, room))
            continue;

        if(IsLevelingRoomType(room->roomType))
            return;

        if(fallbackRoom == (u8)-1 && room->roomType == ADVPATH_ROOM_WILD_DEN)
            fallbackRoom = i;

        if(backupFallbackRoom == (u8)-1 && IsStandardPathRivalLevelingFallbackType(room->roomType))
            backupFallbackRoom = i;
    }

    if(fallbackRoom == (u8)-1)
        fallbackRoom = backupFallbackRoom;

    if(fallbackRoom != (u8)-1)
        AssignRoomInstance(pathSettings, fallbackRoom, ADVPATH_ROOM_ROUTE);
}

static void GenerateRoomPlacements(struct AdvPathSettings* pathSettings)
{
    u8 i;
    u8 freeRoomCount = 0;
    u8 validEncounterCount = 0;
    u16 validEncounterList[ADVPATH_ROOM_COUNT];
    u16 minReplaceCount = 1;
    bool8 standardPathHideRoutes = StandardPathAreRoutesHidden();

    // Place gym at very end
    AssignRoomInstance(pathSettings, 0, ADVPATH_ROOM_BOSS);

    // Place routes on all tiles for now, so other encounters can choose to replace them
    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        // Don't place them immediately before the gym
        if(pathSettings->roomScratch[i].currentCoords.x > 1)
        {
            AssignRoomInstance(pathSettings, i, ADVPATH_ROOM_ROUTE);
            ++freeRoomCount;
        }
    }

    // Now we're going to replace the routes based on the ideal placement
    // The order of these is important to decide the placement

    // For gauntlet, place full rest stop at end always
    if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
    {
        for(i = 0; i < pathSettings->nodeCount; ++i)
        {
            if(pathSettings->roomScratch[i].roomType == ADVPATH_ROOM_ROUTE && pathSettings->roomScratch[i].currentCoords.x <= 2)
            {
                AssignRoomInstance(pathSettings, i, ADVPATH_ROOM_RESTSTOP);
                --freeRoomCount;
            }
        }
    }

    // Frontier Brains are scheduled deterministically from the run seed. Claim
    // their ordinary route slot before other optional encounters are placed.
    if(Rogue_GetScheduledFrontierBrainTrainer(GetPathGenerationDifficulty()) != TRAINER_NONE)
    {
        bool8 placedFrontierBrain = ReplaceRoomEncounter(pathSettings, ADVPATH_ROOM_ROUTE, ADVPATH_ROOM_MINIBOSS);

        AGB_ASSERT(placedFrontierBrain);
        if(placedFrontierBrain)
            --freeRoomCount;
    }

    // Item Rooms are scheduled from an isolated local RNG. Claim their route
    // slot before any ordinary random special-room replacements are made.
    {
#ifdef ROGUE_DEBUG
        if(GetDebugForcedItemRoom(NULL, NULL))
        {
            u8 itemRoomId = (u8)-1;
            bool8 replacedRoute = FALSE;

            // Replace the first playable non-boss node so the room is
            // immediately available from a newly generated path.
            for(i = 0; i < pathSettings->nodeCount; ++i)
            {
                if(IsStartingPathRoom(pathSettings, &pathSettings->roomScratch[i])
                    && pathSettings->roomScratch[i].roomType != ADVPATH_ROOM_BOSS)
                {
                    itemRoomId = i;
                    break;
                }
            }

            // Fall back to any route if the generated layout has no playable
            // node at its starting line.
            if(itemRoomId == (u8)-1)
            {
                for(i = 0; i < pathSettings->nodeCount; ++i)
                {
                    if(pathSettings->roomScratch[i].roomType == ADVPATH_ROOM_ROUTE)
                    {
                        itemRoomId = i;
                        break;
                    }
                }
            }

            if(itemRoomId != (u8)-1)
            {
                replacedRoute = pathSettings->roomScratch[itemRoomId].roomType == ADVPATH_ROOM_ROUTE;
                AssignRoomInstance(pathSettings, itemRoomId, ADVPATH_ROOM_ITEM);
                if(replacedRoute)
                    --freeRoomCount;
            }

            AGB_ASSERT(itemRoomId != (u8)-1);
        }
        else
#endif
        {
            u8 scheduleSlot = 0;
            u16 itemId = ITEM_NONE;

            if(GetScheduledItemRoom(Rogue_GetCurrentDifficulty(), &scheduleSlot, &itemId))
            {
                bool8 placedItemRoom = ReplaceRoomEncounter(pathSettings, ADVPATH_ROOM_ROUTE, ADVPATH_ROOM_ITEM);

                AGB_ASSERT(placedItemRoom);
                if(placedItemRoom)
                    --freeRoomCount;
            }
        }
    }

    // Randomly replace a routes with empty tiles
    {
        u8 chance;
        u8 chanceFalloff;
        
        if(GetPathGenerationDifficulty() >=  ROGUE_CHAMP_START_DIFFICULTY)
        {
            chance = 30;
            chanceFalloff = 4;
        }
        else if(GetPathGenerationDifficulty() >=  ROGUE_ELITE_START_DIFFICULTY)
        {
            chance = 20;
            chanceFalloff = 5;
        }
        else
        {
            chance = 5;
            chanceFalloff = 5;
        }

        if(GetPathGenerationDifficulty() == 0)
            chance = 0;

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
            chance = 0;

        if(chance != 0)
        {
            for(i = 0; i < pathSettings->nodeCount; ++i)
            {
                if(pathSettings->roomScratch[i].roomType == ADVPATH_ROOM_ROUTE && RogueRandomChance(chance, 0))
                {
                    AssignRoomInstance(pathSettings, i, ADVPATH_ROOM_NONE);
                    --freeRoomCount;

                    if(chance <= chanceFalloff)
                        chance = 1;
                    else
                        chance -= chanceFalloff;
                }
            }
        }
    }

    // Populate special encounters into a single list
    //
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET) // In gauntlet we place these manually
    {
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_RESTSTOP;
        ++minReplaceCount;
    }

    // Honey tree
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET && GetPathGenerationDifficulty() >= 1 && RogueRandomChance(60, 0))
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_HONEY_TREE;

    // Mysterious Sign
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET && GetPathGenerationDifficulty() < ROGUE_ELITE_START_DIFFICULTY && RogueRandomChance(40, 0))
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_SIGN;

    // The shrine is not part of the back-to-back battle structure of Gauntlet modes.
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET
        && !gRogueRun.hasChallengedShrine
        && GetPathGenerationDifficulty() == gRogueRun.shrineSpawnDifficulty)
        validEncounterList[validEncounterCount++] = ADVPATH_ROOM_SHRINE;

    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_STANDARD || !standardPathHideRoutes)
    {
        // Legends
        for(i = 0; i < ADVPATH_LEGEND_COUNT; ++i)
        {
            if(gRogueRun.legendarySpecies[i] != SPECIES_NONE && gRogueRun.legendaryDifficulties[i] == GetPathGenerationDifficulty())
            {
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_LEGENDARY;
                ++minReplaceCount;
                break;
            }
        }

        // Team Encounters
        for(i = 0; i < ADVPATH_TEAM_ENCOUNTER_COUNT; ++i)
        {
            if(gRogueRun.teamEncounterDifficulties[i] == GetPathGenerationDifficulty())
            {
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_TEAM_HIDEOUT;
                ++minReplaceCount;
                break;
            }
        }

        // Catching contest
        if(RogueRandomChance(33, 0))
            validEncounterList[validEncounterCount++] = ADVPATH_ROOM_CATCHING_CONTEST;

        // Battle sim / Battle Tower
        if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_GAUNTLET && GetPathGenerationDifficulty() >= 1)
        {
            if(RogueRandomChance(50, 0))
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_BATTLE_SIM;
            else
                validEncounterList[validEncounterCount++] = ADVPATH_ROOM_BATTLE_TOWER;
        }

        {
            bool8 allowDarkDeal = (GetPathGenerationDifficulty() % 3 != 0);
            bool8 allowLab = (GetPathGenerationDifficulty() % 3 != 1);
            bool8 allowGameShow = RogueRandomChance(50, 0);

            if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
            {
                allowDarkDeal = TRUE;
                allowLab = FALSE;
                allowGameShow = FALSE;
            }

            allowDarkDeal = (allowDarkDeal && RogueRandomChance(25, 0));
            allowLab = (allowLab && RogueRandomChance(25, 0));


            if(allowLab)
            {
                // Lab
                if(GetPathGenerationDifficulty() >= ROGUE_GYM_MID_DIFFICULTY - 1)
                    validEncounterList[validEncounterCount++] = ADVPATH_ROOM_LAB;
            }

            // Dark deal / Game show
            if(GetPathGenerationDifficulty() >= ROGUE_GYM_MID_DIFFICULTY + 2)
            {
                // Only dark deals
                if(allowDarkDeal)
                    validEncounterList[validEncounterCount++] = ADVPATH_ROOM_DARK_DEAL;
            }
            else if(GetPathGenerationDifficulty() >= ROGUE_GYM_MID_DIFFICULTY - 1)
            {
                // Mix of both
                if(allowDarkDeal)
                    validEncounterList[validEncounterCount++] = ADVPATH_ROOM_DARK_DEAL;

                if(allowGameShow)
                    validEncounterList[validEncounterCount++] = ADVPATH_ROOM_GAMESHOW;
            }
            else
            {
                // Only game show
                if(allowGameShow)
                    validEncounterList[validEncounterCount++] = ADVPATH_ROOM_GAMESHOW;
            }
        }
    }

    // Unique Den
    if(Rogue_GetModeRules()->adventureGenerator != ADV_GENERATOR_STANDARD || !standardPathHideRoutes)
    {
        for(i = 0; i < ADVPATH_UNIQUE_DEN_COUNT; ++i)
        {
            if(gRogueRun.uniqueDenDifficulties[i] == GetPathGenerationDifficulty())
            {
                if(ReplaceRoomEncounter(pathSettings, ADVPATH_ROOM_ROUTE, ADVPATH_ROOM_UNIQUE_DEN))
                    --freeRoomCount;
                break;
            }
        }
    }

    // Replace % of route with special encounters
    {
        u16 replacePerc = 0;
        u16 replaceCount = freeRoomCount;

        switch (RogueRandom() % 3)
        {
        case 0:
            replacePerc = 25;
            break;
        case 1:
            replacePerc = 33;
            break;
        case 2:
            replacePerc = 50;
            break;
        }

        if(standardPathHideRoutes)
            replacePerc = 100;

        replaceCount = (replaceCount * replacePerc) / 100;
        replaceCount = max(replaceCount, minReplaceCount);
        replaceCount = min(replaceCount, freeRoomCount);

        if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
        {
            replaceCount = min(replaceCount, validEncounterCount);
        }

        for(i = 0; i < (u8)replaceCount; ++i)
        {
            u16 encounterType = SelectRoomType(validEncounterList, validEncounterCount, pathSettings);
            ReplaceRoomEncounter(pathSettings, ADVPATH_ROOM_ROUTE, encounterType);
            --freeRoomCount;
        }
    }

    // Wild dens
    if(standardPathHideRoutes)
    {
        for(i = 0; i < pathSettings->nodeCount; ++i)
        {
            if(pathSettings->roomScratch[i].roomType == ADVPATH_ROOM_ROUTE)
                AssignRoomInstance(pathSettings, i, ADVPATH_ROOM_WILD_DEN);
        }
    }
    else
    {
        u8 chance;
        u8 chanceFalloff;
        u8 minRouteCount;

        // Recoute number of regular routes remaining
        freeRoomCount = 0;
        for(i = 0; i < pathSettings->nodeCount; ++i)
        {
            if(pathSettings->roomScratch[i].roomType == ADVPATH_ROOM_ROUTE)
                ++freeRoomCount;
        }

        // If players get encounters they basically have to get lucky with wild den
        if(GetPathGenerationDifficulty() >=  ROGUE_CHAMP_START_DIFFICULTY)
        {
            chance = 90;
            chanceFalloff = 15;
            minRouteCount = 1;
        }
        else if(GetPathGenerationDifficulty() >=  ROGUE_ELITE_START_DIFFICULTY)
        {
            chance = 60;
            chanceFalloff = 10;
            minRouteCount = 2;
        }
        else if(GetPathGenerationDifficulty() >=  1)
        {
            chance = 40;
            chanceFalloff = 20;
            minRouteCount = 3;
        }
        else
        {
            chance = 5;
            chanceFalloff = 0;
            minRouteCount = 3;
        }

        // Always make sure there is at least 1 regular route which can be chosen
        for(i = 0; i < pathSettings->nodeCount && freeRoomCount > minRouteCount; ++i)
        {
            if(pathSettings->roomScratch[i].roomType == ADVPATH_ROOM_ROUTE && RogueRandomChance(chance, 0))
            {
                AssignRoomInstance(pathSettings, i, ADVPATH_ROOM_WILD_DEN);
                --freeRoomCount;

                if(chance <= chanceFalloff)
                    chance = 1;
                else
                    chance -= chanceFalloff;
            }
        }
    }

    EnsureStandardPathRivalStartingLevelingRoom(pathSettings);
}

static u8 FindRoomOfType(struct AdvPathSettings* pathSettings, u16 type)
{
    u16 i;

    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        if(pathSettings->roomScratch[i].roomType == type)
            return i;
    }

    AGB_ASSERT(FALSE);
    return 0;
}

static u32 GenerateUniqueLegendaryCustomMonId(u16 species)
{
    u32 customMonId;
    u8 rarity;
    RAND_TYPE rngState;

    if(!RogueRandomChance(10, OVERWORLD_FLAG))
        return 0;

    rarity = (RogueRandom() % 2) == 0 ? UNIQUE_RARITY_EPIC : UNIQUE_RARITY_LEGENDARY;

    // Dynamic custom data uses the general RNG. Seed it from the path RNG so
    // Adventure Replay reproduces the same payload without perturbing gameplay RNG.
    rngState = gRngValue;
    SeedRng(RogueRandom());
    customMonId = RogueGift_CreateDynamicMonIdRaw(rarity, species);
    gRngValue = rngState;

    return customMonId;
}

static u32 GenerateUniqueDenCustomMonId(u16 species)
{
    u32 customMonId;
    RAND_TYPE rngState = gRngValue;

    // Dynamic custom data uses the general RNG. Seed the complete payload roll,
    // including rarity, from the path RNG so Adventure Replay reproduces it
    // without perturbing gameplay RNG.
    SeedRng(RogueRandom());
    customMonId = RogueGift_CreateDynamicMonIdRaw(
        RogueGift_RollDynamicUniqueRarity(TRUE),
        species
    );
    gRngValue = rngState;

    return customMonId;
}

#ifdef ROGUE_DEBUG
u32 RogueAdv_Debug_GenerateUniqueLegendaryCustomMonId(u16 species)
{
    return GenerateUniqueLegendaryCustomMonId(species);
}

u32 RogueAdv_Debug_GenerateUniqueDenCustomMonId(u16 species)
{
    return GenerateUniqueDenCustomMonId(species);
}
#endif

static u16 GetFullRestStopWeight(u8 difficulty)
{
    if(difficulty >= ROGUE_ELITE_START_DIFFICULTY)
        return 20;
    if(difficulty >= ROGUE_GYM_START_DIFFICULTY + 2)
        return 6;
    return 0;
}

#ifdef ROGUE_DEBUG
u16 RogueAdv_Debug_GetFullRestStopWeight(u8 difficulty)
{
    return GetFullRestStopWeight(difficulty);
}
#endif

static void AssignRoomInstance(struct AdvPathSettings* pathSettings, u8 roomId, u8 roomType)
{
    u16 weights[ADVPATH_SUBROOM_WEIGHT_COUNT];
    struct AdvPathRoomSettings* room = &pathSettings->roomScratch[roomId];
    u8 previousRoomType = room->roomType;

    memset(weights, 0, sizeof(weights));

    if(previousRoomType != ADVPATH_ROOM_NONE && previousRoomType < ADVPATH_ROOM_COUNT)
        --pathSettings->numOfRooms[previousRoomType];

    memset(&room->roomParams, 0, sizeof(room->roomParams));
    room->roomType = ADVPATH_ROOM_NONE;

    switch(roomType)
    {
        case ADVPATH_ROOM_RESTSTOP:
            weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 15;
            weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 15;
            weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = 15;
            weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = GetFullRestStopWeight(GetPathGenerationDifficulty());

            // Prefer showing each rest stop type before having duplicates
            if(CountSubRoomType(pathSettings, ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_BATTLE) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 0;

            if(CountSubRoomType(pathSettings, ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_SHOP) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 0;

            if(CountSubRoomType(pathSettings, ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_DAYCARE) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = 0;

            if(weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] == 0 && weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] == 0 && weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] == 0)
            {
                // Make sure we place all other types first before placing duplicates
                weights[ADVPATH_SUBROOM_RESTSTOP_BATTLE] = 3;
                weights[ADVPATH_SUBROOM_RESTSTOP_SHOP] = 3;
                weights[ADVPATH_SUBROOM_RESTSTOP_DAYCARE] = 3;
            }

            // If we have a full rest stop, make it only appear once
            if(CountSubRoomType(pathSettings, ADVPATH_ROOM_RESTSTOP, ADVPATH_SUBROOM_RESTSTOP_FULL) != 0)
                weights[ADVPATH_SUBROOM_RESTSTOP_FULL] = 0;

            // For champ we will always spawn full rest stops, for balance
            if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET || GetPathGenerationDifficulty() >= ROGUE_CHAMP_START_DIFFICULTY)
            {
                room->roomParams.roomIdx = ADVPATH_SUBROOM_RESTSTOP_FULL;
            }
            else
            {
                room->roomParams.roomIdx = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
            }
            break;
        case ADVPATH_ROOM_SIGN:
            // Use the same RNG seed as the boss so we can generate their team.
            room->rngSeed = pathSettings->roomScratch[FindRoomOfType(pathSettings, ADVPATH_ROOM_BOSS)].rngSeed;
            break;
    }

    room->roomType = roomType;
    if(roomType != ADVPATH_ROOM_NONE && roomType < ADVPATH_ROOM_COUNT)
        ++pathSettings->numOfRooms[roomType];
}

static void MaterializeRoom(u8 roomId)
{
    u16 weights[ADVPATH_SUBROOM_WEIGHT_COUNT];
    struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[roomId];

    memset(weights, 0, sizeof(weights));

    switch(room->roomType)
    {
    case ADVPATH_ROOM_BOSS:
        // Specifically use the correct difficulty here regardless of if we are faking or not.
        AGB_ASSERT(Rogue_GetCurrentDifficulty() < ARRAY_COUNT(gRogueRun.bossTrainerNums));
        room->roomParams.perType.boss.trainerNum = gRogueRun.bossTrainerNums[Rogue_GetCurrentDifficulty()];
        break;

    case ADVPATH_ROOM_LEGENDARY:
    {
        u8 legendId = Rogue_GetCurrentLegendaryEncounterId();
        u16 species = gRogueRun.legendarySpecies[legendId];
        room->roomParams.roomIdx = Rogue_GetLegendaryRoomForSpecies(species);
        room->roomParams.perType.legendary.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
        room->roomParams.perType.legendary.customMonId = GenerateUniqueLegendaryCustomMonId(species);
        break;
    }

    case ADVPATH_ROOM_TEAM_HIDEOUT:
    {
        u8 encounterId = Rogue_GetCurrentTeamHideoutEncounterId();
        room->roomParams.roomIdx = gRogueRun.teamEncounterRooms[encounterId];
        break;
    }

    case ADVPATH_ROOM_MINIBOSS:
        room->roomParams.roomIdx = 0;
        room->roomParams.perType.miniboss.trainerNum = Rogue_GetScheduledFrontierBrainTrainer(GetPathGenerationDifficulty());
        AGB_ASSERT(room->roomParams.perType.miniboss.trainerNum != TRAINER_NONE);
        break;

    case ADVPATH_ROOM_ITEM:
    {
        u8 scheduleSlot = 0;
        u16 itemId = ITEM_NONE;
#ifdef ROGUE_DEBUG
        bool8 hasScheduledRoom = GetDebugForcedItemRoom(&scheduleSlot, &itemId);

        if(!hasScheduledRoom)
            hasScheduledRoom = GetScheduledItemRoom(Rogue_GetCurrentDifficulty(), &scheduleSlot, &itemId);

        AGB_ASSERT(hasScheduledRoom);
#else
        GetScheduledItemRoom(Rogue_GetCurrentDifficulty(), &scheduleSlot, &itemId);
#endif
        room->roomParams.roomIdx = 0;
        room->roomParams.perType.itemRoom.itemId = itemId;
        room->roomParams.perType.itemRoom.scheduleSlot = scheduleSlot;
        break;
    }

    case ADVPATH_ROOM_WILD_DEN:
        room->roomParams.roomIdx = 0;
        room->roomParams.perType.wildDen.species = Rogue_SelectWildDenEncounterRoom();
        room->roomParams.perType.wildDen.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
        break;

    case ADVPATH_ROOM_UNIQUE_DEN:
    {
        u16 species = Rogue_SelectUniqueDenEncounterRoom();
        room->roomParams.roomIdx = 0;
        room->roomParams.perType.uniqueDen.species = species;
        room->roomParams.perType.uniqueDen.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
        room->roomParams.perType.uniqueDen.customMonId = GenerateUniqueDenCustomMonId(species);
        break;
    }

    case ADVPATH_ROOM_HONEY_TREE:
        room->roomParams.roomIdx = 0;
        room->roomParams.perType.honeyTree.species = Rogue_SelectHoneyTreeEncounterRoom();
        room->roomParams.perType.honeyTree.shinyState = Rogue_RollShinyState(SHINY_ROLL_STATIC);
        break;

    case ADVPATH_ROOM_ROUTE:
        room->roomParams.roomIdx = Rogue_SelectRouteRoom(GetPathGenerationDifficulty());
        DebugPrintf("Route [%d] = %d", roomId, room->roomParams.roomIdx);

        if(GetPathGenerationDifficulty() > ROGUE_ELITE_START_DIFFICULTY)
        {
            weights[ADVPATH_SUBROOM_ROUTE_CALM] = 0;
            weights[ADVPATH_SUBROOM_ROUTE_AVERAGE] = 1;
            weights[ADVPATH_SUBROOM_ROUTE_TOUGH] = 8;
        }
        else
        {
            weights[ADVPATH_SUBROOM_ROUTE_CALM] = 3;
            weights[ADVPATH_SUBROOM_ROUTE_AVERAGE] = 4;
            weights[ADVPATH_SUBROOM_ROUTE_TOUGH] = 1;
        }

        room->roomParams.perType.route.difficulty = SelectIndexFromWeights(weights, ARRAY_COUNT(weights), RogueRandom());
        RogueRouteScenes_GenerateRoom(room);
        break;
    }
}

static void MaterializePath(struct AdvPathSettings* pathSettings)
{
    u8 i;

    gRogueAdvPath.roomCount = pathSettings->nodeCount;
    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        struct AdvPathRoomSettings* assignedRoom = &pathSettings->roomScratch[i];
        struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[i];

        memset(room, 0, sizeof(*room));
        room->coords = assignedRoom->currentCoords;
        room->roomParams = assignedRoom->roomParams;
        room->rngSeed = assignedRoom->rngSeed;
        room->roomType = assignedRoom->roomType;
        room->connectionMask = assignedRoom->connectionMask;
    }

    // Room choices are now stable, so create each surviving room payload once.
    for(i = 0; i < pathSettings->nodeCount; ++i)
        MaterializeRoom(i);
}

static u8 CountRoomConnections(u8 mask)
{
    u8 count = 0;

    if(mask == 0)
        return 0;

    if((mask & ROOM_CONNECTION_MASK_TOP) != 0)
        ++count;

    if((mask & ROOM_CONNECTION_MASK_MID) != 0)
        ++count;

    if((mask & ROOM_CONNECTION_MASK_BOT) != 0)
        ++count;

    return count;
}

static u8 GenerateRoomConnectionMask(struct Coords8 coords, struct AdvPathSettings* pathSettings)
{
    u8 mask;
    u8 connCount;
    u8 minConnCount = pathSettings->generator->connectionsSettingsPerColumn[min(coords.x, MAX_CONNECTION_GENERATOR_COLUMNS - 1)].minCount;
    u8 maxConnCount = pathSettings->generator->connectionsSettingsPerColumn[min(coords.x, MAX_CONNECTION_GENERATOR_COLUMNS - 1)].maxCount;
    u8 const* branchingChances = pathSettings->generator->connectionsSettingsPerColumn[min(coords.x, MAX_CONNECTION_GENERATOR_COLUMNS - 1)].branchingChance;

    do
    {
        mask = 0;

        if(RogueRandomChance(branchingChances[ROOM_CONNECTION_TOP], OVERWORLD_FLAG))
            mask |= ROOM_CONNECTION_MASK_TOP;

        if(RogueRandomChance(branchingChances[ROOM_CONNECTION_MID], OVERWORLD_FLAG))
            mask |= ROOM_CONNECTION_MASK_MID;

        if(RogueRandomChance(branchingChances[ROOM_CONNECTION_BOT], OVERWORLD_FLAG))
            mask |= ROOM_CONNECTION_MASK_BOT;

        connCount = CountRoomConnections(mask);
    }
    // keep going until we have the required number of connections
    while(!(connCount >= minConnCount && connCount <= maxConnCount));

    AGB_ASSERT(mask != 0);

    return mask;
}

static bool8 DoesRoomExists(s8 x, s8 y, struct AdvPathSettings* pathSettings)
{
    u8 i;

    for(i = 0; i < pathSettings->nodeCount; ++i)
    {
        if(pathSettings->roomScratch[i].currentCoords.x == x && pathSettings->roomScratch[i].currentCoords.y == y)
            return TRUE;
    }

    return FALSE;
}

bool8 RogueAdv_GenerateAdventurePathsIfRequired()
{
    if(gRogueAdvPath.roomCount != 0
        && gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID
        && gRogueAdvPath.justGenerated)
    {
        // The run portal already generated the initial path while the screen
        // was black. Reuse it on the first overview map load.
        return TRUE;
    }
    else if(gRogueRun.adventureRoomId != ADVPATH_INVALID_ROOM_ID
        && gRogueAdvPath.roomCount != 0
        && gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomType != ADVPATH_ROOM_BOSS)
    {
        // Path is still valid
        gRogueAdvPath.justGenerated = FALSE;
        return FALSE;
    }
    else
    {
#ifdef DEBUG_FEATURE_FRAME_TIMERS
        u32 pathStartClock = RogueDebug_SampleClock();
#endif
        struct AdvPathSettings pathSettings = {0};
        struct AdvPathGenerator generator = {0};

        // If we have a valid room ID, then we're reloading a previous save
        bool8 isNewGeneration = gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID;

        // The replacement roster cannot be encountered this early. Planning it
        // with the first post-boss path keeps run start responsive without moving
        // the work into a battle transition. Use actual run progress here because
        // Gauntlet applies late-game balance to its initial path.
        if(Rogue_GetCurrentDifficulty() >= ROGUE_GYM_START_DIFFICULTY + 1)
            Rogue_EnsureRivalLateTeamForNewAdventure();

        pathSettings.generator = &generator;

        switch(Rogue_GetModeRules()->adventureGenerator)
        {
        case ADV_GENERATOR_GAUNTLET:
            if(Rogue_GetCurrentDifficulty() == 0)
                pathSettings.totalLength = 5 + 2;
            else
                pathSettings.totalLength = 2;
            break;

        case ADV_GENERATOR_STANDARD:
            if(GetPathGenerationDifficulty() >= ROGUE_CHAMP_START_DIFFICULTY)
                pathSettings.totalLength = 3 + 2;
            else
                pathSettings.totalLength = (StandardPathAreRoutesHidden() ? 2 : 4) + 2;
            break;

        case ADV_GENERATOR_SLOW_PATH:
            pathSettings.totalLength = 3 + 2; // +2 to account for final encounter and initial split
            break;
        }

        // Select the correct seed
        {
            u8 i;
            u16 seed;
            SeedRogueRng(gRogueRun.baseSeed * 235 + 31897);

            seed = RogueRandom();
            for(i = 0; i < GetPathGenerationDifficulty(); ++i)
            {
                seed = RogueRandom();
            }

            // This is the seed for this path
            SeedRogueRng(seed);
        }

        // Select some branching presets for the layout generation
        {
            u8 i;

            // Gym split
            generator.connectionsSettingsPerColumn[0].minCount = 2;
            generator.connectionsSettingsPerColumn[0].maxCount = 3;
            generator.connectionsSettingsPerColumn[0].branchingChance[ROOM_CONNECTION_TOP] = 33;
            generator.connectionsSettingsPerColumn[0].branchingChance[ROOM_CONNECTION_MID] = 33;
            generator.connectionsSettingsPerColumn[0].branchingChance[ROOM_CONNECTION_BOT] = 33;
            
            for(i = 1; i < MAX_CONNECTION_GENERATOR_COLUMNS; ++i)
            {
                if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_GAUNTLET)
                {
                    // Mixed but not too wide
                    generator.connectionsSettingsPerColumn[i].minCount = 1;
                    generator.connectionsSettingsPerColumn[i].maxCount = 2;
                    generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                    generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 40;
                    generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                }
                else if(Rogue_GetModeRules()->adventureGenerator == ADV_GENERATOR_STANDARD && !StandardPathAreRoutesHidden())
                {
                    // Reduce variation to avoid splitting too wide during longer Standard segments.
                    switch (RogueRandom() % 3)
                    {
                    // Branches
                    case 0:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 2;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 0;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Lines
                    case 1:
                        generator.connectionsSettingsPerColumn[i].minCount = 2;
                        generator.connectionsSettingsPerColumn[i].maxCount = 2;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 10;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 50;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 10;
                        break;

                    // Wiggling line
                    case 2:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 1;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 0;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    default:
                        AGB_ASSERT(FALSE);
                        break;
                    }
                }
                else
                {
                    // Random column variant switches
                    switch (RogueRandom() % 6)
                    {
                    // Mixed/Standard
                    case 0:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 3;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Branches
                    case 1:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 2;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 0;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Lines
                    case 2:
                        generator.connectionsSettingsPerColumn[i].minCount = 2;
                        generator.connectionsSettingsPerColumn[i].maxCount = 2;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 10;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 50;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 10;
                        break;

                    // Wiggling line
                    case 3:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 1;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 40;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 0;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 40;
                        break;

                    // Fork
                    case 4:
                        generator.connectionsSettingsPerColumn[i].minCount = 3;
                        generator.connectionsSettingsPerColumn[i].maxCount = 3;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 100;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 100;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 100;
                        break;

                    // Mixed/Standard Alt
                    case 5:
                        generator.connectionsSettingsPerColumn[i].minCount = 1;
                        generator.connectionsSettingsPerColumn[i].maxCount = 3;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_TOP] = 50;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_MID] = 10;
                        generator.connectionsSettingsPerColumn[i].branchingChance[ROOM_CONNECTION_BOT] = 50;
                        break;
                    
                    default:
                        AGB_ASSERT(FALSE);
                        break;
                    }
                }
            }
        }

        DebugPrintf("ADVPATH: Generating path for seed %d.", gRngRogueValue);
        Rogue_ResetAdventurePathBuffers();
        GeneratePath(&pathSettings);

        DebugPrint("ADVPATH: Finished generating path.");

#ifdef DEBUG_FEATURE_FRAME_TIMERS
        DebugPrintf("[Run Load] Path generation: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - pathStartClock));
#endif

        gRogueAdvPath.justGenerated = isNewGeneration;

        if(!isNewGeneration)
        {
            // Remember the room type/params
            gRogueAdvPath.currentRoomType = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomType;
            memcpy(&gRogueAdvPath.currentRoomParams, &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].roomParams, sizeof(gRogueAdvPath.currentRoomParams));
        }

        return isNewGeneration;
    }
}

void RogueAdv_Debug_ForceRegenerateAdventurePaths()
{
#ifdef ROGUE_DEBUG
    struct WarpData warp;

    gRogueAdvPath.roomCount = 0;
    gRogueAdvPath.isOverviewActive = FALSE;
    gRogueRun.adventureRoomId = ADVPATH_INVALID_ROOM_ID;
    
    // Fill with dud warp
    warp.mapGroup = MAP_GROUP(ROGUE_HUB_TRANSITION);
    warp.mapNum = MAP_NUM(ROGUE_HUB_TRANSITION);
    warp.warpId = 0;
    warp.x = -1;
    warp.y = -1;

    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
#endif
}

u8 RogueAdv_GetTileNum()
{
    if(!gRogueAdvPath.isOverviewActive && gRogueRun.adventureRoomId < gRogueAdvPath.roomCount)
    {
        return gRogueAdvPath.pathLength - gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x - 1;
    }

    // Fallback so we are viewing the same thing
    return 0;
}

bool8 RogueAdv_IsViewingPath()
{
    return gRogueAdvPath.isOverviewActive != 0;
}

static bool8 IsIslandCoordInBounds(s16 x, s16 y)
{
    return x >= 0 && x < ADVENTURE_PATHS_MAP_WIDTH
        && y >= 0 && y < ADVENTURE_PATHS_MAP_HEIGHT;
}

static bool8 AllocAdventureIslandMask(void)
{
    AGB_ASSERT(sAdventureIslandMask == NULL);
    AGB_ASSERT(sAdventureIslandLevels == NULL);
    sAdventureIslandMask = AllocZeroed(ADVENTURE_PATHS_MASK_SIZE);
    sAdventureIslandLevels = AllocZeroed(
        ADVENTURE_PATHS_LEVEL_SIZE + sizeof(struct AdventureIslandVisualSettings)
    );
    if(sAdventureIslandMask == NULL || sAdventureIslandLevels == NULL)
    {
        Free(sAdventureIslandMask);
        Free(sAdventureIslandLevels);
        sAdventureIslandMask = NULL;
        sAdventureIslandLevels = NULL;
        return FALSE;
    }
    return TRUE;
}

static void FreeAdventureIslandMask(void)
{
    Free(sAdventureIslandMask);
    Free(sAdventureIslandLevels);
    sAdventureIslandMask = NULL;
    sAdventureIslandLevels = NULL;
}

static u8 GetAdventureIslandLevel(s16 x, s16 y)
{
    u16 index;
    u8 shift;

    if(!IsIslandCoordInBounds(x, y))
        return 0;
    index = y * ADVENTURE_PATHS_MAP_WIDTH + x;
    shift = (index & 1) * 4;
    return (sAdventureIslandLevels[index / 2] >> shift) & 0xF;
}

static void SetAdventureIslandLevel(s16 x, s16 y, u8 level)
{
    u16 index;
    u8 shift;
    u8 mask;

    if(!IsIslandCoordInBounds(x, y))
        return;
    index = y * ADVENTURE_PATHS_MAP_WIDTH + x;
    shift = (index & 1) * 4;
    mask = 0xF << shift;
    sAdventureIslandLevels[index / 2] = (sAdventureIslandLevels[index / 2] & ~mask) | ((level & 0xF) << shift);
}

static u8 GetIslandMaskCell(s16 x, s16 y)
{
    u16 index = y * ADVENTURE_PATHS_MAP_WIDTH + x;
    u8 shift = (index & 3) * 2;

    return (sAdventureIslandMask[index / 4] >> shift) & 0x3;
}

static void SetIslandMaskCell(s16 x, s16 y, u8 value)
{
    u16 index = y * ADVENTURE_PATHS_MAP_WIDTH + x;
    u8 shift = (index & 3) * 2;
    u8 mask = 0x3 << shift;

    sAdventureIslandMask[index / 4] = (sAdventureIslandMask[index / 4] & ~mask) | ((value & 0x3) << shift);
}

static bool8 HasIslandMaskFlag(s16 x, s16 y, u8 flag)
{
    return IsIslandCoordInBounds(x, y) && ((GetIslandMaskCell(x, y) & flag) != 0);
}

static bool8 SetTrailMaskCell(s16 x, s16 y, bool8 blocked)
{
    u8 cell;

    if(!IsIslandCoordInBounds(x, y))
        return FALSE;

    cell = GetIslandMaskCell(x, y);
    cell |= ISLAND_MASK_TRAIL;
    if(blocked)
        cell |= ISLAND_MASK_BLOCKED;
    else
        cell &= ~ISLAND_MASK_BLOCKED;
    SetIslandMaskCell(x, y, cell);
    return TRUE;
}

static u32 GetAdventureIslandCoordHash(u16 x, u16 y, u32 salt)
{
    u32 hash = 2166136261u;

    hash = (hash ^ gRogueRun.baseSeed) * 16777619u;
    hash = (hash ^ x) * 16777619u;
    hash = (hash ^ y) * 16777619u;
    hash = (hash ^ salt) * 16777619u;
    hash ^= hash >> 13;
    hash *= 0x85EBCA6Bu;
    return hash ^ (hash >> 16);
}

static u32 GetAdventureIslandSegmentHash(u32 salt)
{
    return GetAdventureIslandCoordHash(
        gRogueAdvPath.pathLength,
        GetPathGenerationDifficulty(),
        salt
    );
}

static struct AdventureIslandVisualSettings *GetAdventureIslandVisualSettings(void)
{
    return (struct AdventureIslandVisualSettings *)&sAdventureIslandLevels[ADVENTURE_PATHS_LEVEL_SIZE];
}

static s8 GetAdventureTemplateOffsetFromVisualHash(u32 visualHash, u32 salt)
{
    u32 hash = visualHash ^ salt;

    hash ^= hash >> 13;
    hash *= 0x85EBCA6Bu;
    hash ^= hash >> 16;
    return hash
        % (ADVENTURE_TEMPLATE_OFFSET_RADIUS * 2 + 1)
        - ADVENTURE_TEMPLATE_OFFSET_RADIUS;
}

static void CacheAdventureIslandVisualSettings(void)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();

    settings->surfaceTemplate = GetMapLayout(LAYOUT_ROGUE_ADVENTURE_PATHS)->map;
    settings->visualHash = GetAdventureIslandSegmentHash(0x5345474D);
    settings->templateOffsetX = GetAdventureTemplateOffsetFromVisualHash(settings->visualHash, 0x544D5048);
    settings->templateOffsetY = GetAdventureTemplateOffsetFromVisualHash(settings->visualHash, 0x544D5056);
    settings->satelliteCount = 0;
    settings->debrisCount = 0;
}

static u8 GetAdventureIslandSilhouetteNoise(s16 x, s16 y, u32 visualHash)
{
    u32 broadNoise = GetAdventureIslandCoordHash(x / 4, y / 4, 0x42524F41 ^ visualHash) & 0xFF;
    u32 mediumNoise = GetAdventureIslandCoordHash(x / 2, y / 2, 0x4D454449 ^ visualHash) & 0xFF;
    u32 detailNoise = GetAdventureIslandCoordHash(x, y, 0x44455441 ^ visualHash) & 0xFF;

    // Broad noise supplies recognizable lobes; the lighter higher-frequency
    // layers stop their borders from looking like aligned rectangles.
    return (broadNoise * 2 + mediumNoise + detailNoise) / 4;
}

static bool8 HasAdventureIslandInwardNeighbour(s16 x, s16 y, u8 encodedDistance)
{
    u8 north = GetAdventureIslandLevel(x, y - 1);
    u8 east = GetAdventureIslandLevel(x + 1, y);
    u8 south = GetAdventureIslandLevel(x, y + 1);
    u8 west = GetAdventureIslandLevel(x - 1, y);

    return (north != 0 && north < encodedDistance)
        || (east != 0 && east < encodedDistance)
        || (south != 0 && south < encodedDistance)
        || (west != 0 && west < encodedDistance);
}

static bool8 IsAdventureIslandCellInSolid2x2(s16 x, s16 y)
{
    s16 anchorX;
    s16 anchorY;

    // A cell may occupy any corner of its supporting 2x2 block.
    for(anchorY = y - 1; anchorY <= y; ++anchorY)
    {
        for(anchorX = x - 1; anchorX <= x; ++anchorX)
        {
            if(GetAdventureIslandLevel(anchorX, anchorY) != 0
                && GetAdventureIslandLevel(anchorX + 1, anchorY) != 0
                && GetAdventureIslandLevel(anchorX, anchorY + 1) != 0
                && GetAdventureIslandLevel(anchorX + 1, anchorY + 1) != 0)
            {
                return TRUE;
            }
        }
    }
    return FALSE;
}

static void RemoveNarrowAdventureIslandFringe(u8 innerFringeDistance, u8 outerFringeDistance)
{
    s16 x;
    s16 y;

    // Mark first and remove second so every decision sees the same silhouette.
    // Requiring membership in a filled 2x2 block eliminates one-tile teeth in
    // either direction without eroding the guaranteed inner route buffer.
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 encodedDistance = GetAdventureIslandLevel(x, y);

            if((encodedDistance == innerFringeDistance
                    || encodedDistance == outerFringeDistance)
                && !IsAdventureIslandCellInSolid2x2(x, y))
            {
                SetIslandMaskCell(x, y, GetIslandMaskCell(x, y) | ISLAND_MASK_BLOCKED);
            }
        }
    }

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            if(!HasIslandMaskFlag(x, y, ISLAND_MASK_TRAIL)
                && HasIslandMaskFlag(x, y, ISLAND_MASK_BLOCKED))
            {
                SetAdventureIslandLevel(x, y, 0);
                SetIslandMaskCell(x, y, 0);
            }
        }
    }
}

static void ShapeAdventureIslandFringe(u8 expansion, struct AdventureIslandVisualStats *visualStats)
{
    u8 innerFringeDistance = expansion + 1;
    u8 outerFringeDistance = expansion + 2;
    u32 visualHash = GetAdventureIslandVisualSettings()->visualHash;
    u16 innerFringeCandidates = 0;
    u16 keptInnerFringe = 0;
    u16 outerFringe = 0;
    s16 x;
    s16 y;

    // The original dilation remains intact through expansion - 1. Vary its
    // outer ring and selectively retain one additional ring, producing safe
    // coves and lobes without ever touching the traversable trail network.
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 encodedDistance = GetAdventureIslandLevel(x, y);

            if(encodedDistance == innerFringeDistance
                || encodedDistance == outerFringeDistance)
            {
                u8 noise = GetAdventureIslandSilhouetteNoise(x, y, visualHash);

                if(encodedDistance == innerFringeDistance)
                    ++innerFringeCandidates;
                if((encodedDistance == innerFringeDistance
                        && noise < ADVENTURE_ISLAND_INNER_FRINGE_THRESHOLD)
                    || (encodedDistance == outerFringeDistance
                        && noise < ADVENTURE_ISLAND_OUTER_FRINGE_THRESHOLD))
                {
                    // Keep authored multi-cell formations whole; their own
                    // validation still removes any footprint that did not fit.
                    if(!IsAdventureFormationMetatile(GetAdventureSurfaceTemplateMetatile(x, y)))
                        SetAdventureIslandLevel(x, y, 0);
                }
            }
        }
    }

    // Remove fringe cells that the noisy cut disconnected from the preceding
    // ring. Requiring a cardinal attachment also rounds square dilation corners.
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 encodedDistance = GetAdventureIslandLevel(x, y);

            if((encodedDistance == innerFringeDistance
                    || encodedDistance == outerFringeDistance)
                && !HasAdventureIslandInwardNeighbour(x, y, encodedDistance)
                && !IsAdventureFormationMetatile(GetAdventureSurfaceTemplateMetatile(x, y)))
            {
                SetAdventureIslandLevel(x, y, 0);
            }
        }
    }

    RemoveNarrowAdventureIslandFringe(innerFringeDistance, outerFringeDistance);

    if(visualStats != NULL)
    {
        visualStats->narrowFringe = 0;
        for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
        {
            for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
            {
                if(GetAdventureIslandLevel(x, y) == innerFringeDistance)
                {
                    ++keptInnerFringe;
                    if(!IsAdventureIslandCellInSolid2x2(x, y))
                        ++visualStats->narrowFringe;
                }
                else if(GetAdventureIslandLevel(x, y) == outerFringeDistance)
                {
                    ++outerFringe;
                    if(!IsAdventureIslandCellInSolid2x2(x, y))
                        ++visualStats->narrowFringe;
                }
            }
        }
        visualStats->keptInnerFringe = keptInnerFringe;
        visualStats->prunedInnerFringe = innerFringeCandidates - keptInnerFringe;
        visualStats->outerFringe = outerFringe;
    }
}

static bool8 BuildAdventureIslandMask(struct AdventureIslandVisualStats *visualStats)
{
    bool8 allTrailCellsInBounds = TRUE;
    s16 x;
    s16 y;
    u32 i;
    u32 j;

    memset(sAdventureIslandMask, 0, ADVENTURE_PATHS_MASK_SIZE);
    CacheAdventureIslandVisualSettings();
    if(gRogueAdvPath.roomCount == 0)
        return FALSE;

    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        x = ROOM_TO_MAP_X(gRogueAdvPath.rooms[i].coords.x);
        y = ROOM_TO_MAP_Y(gRogueAdvPath.rooms[i].coords.y);

        allTrailCellsInBounds &= SetTrailMaskCell(x + 2, y, ShouldBlockObjectEvent(&gRogueAdvPath.rooms[i]));
        allTrailCellsInBounds &= SetTrailMaskCell(x + 1, y, FALSE);

        if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_TOP) != 0)
            allTrailCellsInBounds &= SetTrailMaskCell(x + 1, y + 1, FALSE);
        if((gRogueAdvPath.rooms[i].connectionMask & ROOM_CONNECTION_MASK_BOT) != 0)
            allTrailCellsInBounds &= SetTrailMaskCell(x + 1, y - 1, FALSE);

        if(i != 0)
        {
            for(j = 1; j < ROOM_TO_WORLD_X; ++j)
            {
                bool8 blocked = j == 1 && IsObjectEventVisible(&gRogueAdvPath.rooms[i]);
                allTrailCellsInBounds &= SetTrailMaskCell(x + 2 + j, y, blocked);
            }
        }
    }

    {
        s16 minY = ADVENTURE_PATHS_MAP_HEIGHT;
        s16 maxY = -1;
        s16 startX = -1;

        for(i = 0; i < gRogueAdvPath.roomCount; ++i)
        {
            if(gRogueAdvPath.rooms[i].coords.x == gRogueAdvPath.pathLength - 1)
            {
                startX = ROOM_TO_MAP_X(gRogueAdvPath.rooms[i].coords.x);
                y = ROOM_TO_MAP_Y(gRogueAdvPath.rooms[i].coords.y);
                minY = min(minY, y);
                maxY = max(maxY, y);
            }
        }

        if(startX < 0)
        {
            allTrailCellsInBounds = FALSE;
        }
        else
        {
            for(y = minY; y <= maxY; ++y)
                allTrailCellsInBounds &= SetTrailMaskCell(startX + 1, y, FALSE);
        }
    }

    CacheAdventureIslandSurface(visualStats);
    BuildAdventurePeripheralPlan();
    return allTrailCellsInBounds;
}

static u8 GetAdventureTerraceStage(void)
{
    u8 difficulty = GetPathGenerationDifficulty();

    if(difficulty >= ROGUE_FINAL_CHAMP_DIFFICULTY)
        return 3;
    if(difficulty >= ROGUE_CHAMP_START_DIFFICULTY)
        return 2;
    if(difficulty >= ROGUE_ELITE_START_DIFFICULTY)
        return 1;
    return 0;
}

static u8 GetAdventureSurfaceLevelForDistance(u8 terraceStage, u8 distance)
{
    if(terraceStage == 0)
        return 1;
    if(distance <= 1)
        return terraceStage + 1;
    if(distance <= 3)
        return terraceStage;
    if(distance >= terraceStage + 3)
        return 1;
    return terraceStage - (distance - 3);
}

static bool8 IsAdventureIslandSurfaceCell(s16 x, s16 y)
{
    return GetAdventureIslandLevel(x, y) != 0;
}

static u8 GetAdventureIslandEncodedDistance(s16 x, s16 y)
{
    if(!IsIslandCoordInBounds(x, y))
        return ADVENTURE_ISLAND_DISTANCE_UNREACHED;
    return GetAdventureIslandLevel(x, y);
}

static void CacheAdventureIslandSurface(struct AdventureIslandVisualStats *visualStats)
{
    u8 terraceStage = GetAdventureTerraceStage();
    u8 expansion = 2 + terraceStage;
    u8 maxExpansion = min(expansion + 1, ADVENTURE_ISLAND_MAX_EXPANSION);
    s16 x;
    s16 y;

    memset(sAdventureIslandLevels, 0, ADVENTURE_PATHS_LEVEL_SIZE);

    // Store distance + 1 temporarily. Unreached cells use the largest value
    // available in the packed nibble until the two distance-transform sweeps.
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            if(HasIslandMaskFlag(x, y, ISLAND_MASK_TRAIL))
                SetAdventureIslandLevel(x, y, 1);
            else
                SetAdventureIslandLevel(x, y, ADVENTURE_ISLAND_DISTANCE_UNREACHED);
        }
    }

    // An eight-neighbour distance field is a Chebyshev distance transform.
    // Forward and backward sweeps produce the same result as expanding every
    // frontier separately, but visit each map cell only twice.
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 encodedDistance = GetAdventureIslandLevel(x, y);
            u8 neighbourDistance = min(
                min(GetAdventureIslandEncodedDistance(x - 1, y - 1), GetAdventureIslandEncodedDistance(x, y - 1)),
                min(GetAdventureIslandEncodedDistance(x + 1, y - 1), GetAdventureIslandEncodedDistance(x - 1, y))
            );

            if(neighbourDistance < ADVENTURE_ISLAND_DISTANCE_UNREACHED
                && neighbourDistance + 1 < encodedDistance)
                SetAdventureIslandLevel(x, y, neighbourDistance + 1);
        }
    }

    for(y = ADVENTURE_PATHS_MAP_HEIGHT - 1; y >= 0; --y)
    {
        for(x = ADVENTURE_PATHS_MAP_WIDTH - 1; x >= 0; --x)
        {
            u8 encodedDistance = GetAdventureIslandLevel(x, y);
            u8 neighbourDistance = min(
                min(GetAdventureIslandEncodedDistance(x + 1, y), GetAdventureIslandEncodedDistance(x - 1, y + 1)),
                min(GetAdventureIslandEncodedDistance(x, y + 1), GetAdventureIslandEncodedDistance(x + 1, y + 1))
            );

            if(neighbourDistance < ADVENTURE_ISLAND_DISTANCE_UNREACHED
                && neighbourDistance + 1 < encodedDistance)
                SetAdventureIslandLevel(x, y, neighbourDistance + 1);
        }
    }

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            if(GetAdventureIslandLevel(x, y) > maxExpansion + 1)
                SetAdventureIslandLevel(x, y, 0);
        }
    }

    ShapeAdventureIslandFringe(expansion, visualStats);

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 encodedDistance = GetAdventureIslandLevel(x, y);

            if(encodedDistance != 0)
            {
                SetAdventureIslandLevel(x, y, GetAdventureSurfaceLevelForDistance(terraceStage, encodedDistance - 1));
                if(!HasIslandMaskFlag(x, y, ISLAND_MASK_TRAIL))
                    SetIslandMaskCell(x, y, ISLAND_MASK_BLOCKED);
            }
        }
    }

    // Fill isolated one-cell pinholes at the lowest surrounding plane.
    for(y = 1; y < ADVENTURE_PATHS_MAP_HEIGHT - 1; ++y)
    {
        for(x = 1; x < ADVENTURE_PATHS_MAP_WIDTH - 1; ++x)
        {
            if(GetAdventureIslandLevel(x, y) == 0
                && GetAdventureIslandLevel(x, y - 1) != 0
                && GetAdventureIslandLevel(x + 1, y) != 0
                && GetAdventureIslandLevel(x, y + 1) != 0
                && GetAdventureIslandLevel(x - 1, y) != 0)
            {
                u8 level = min(
                    min(GetAdventureIslandLevel(x, y - 1), GetAdventureIslandLevel(x + 1, y)),
                    min(GetAdventureIslandLevel(x, y + 1), GetAdventureIslandLevel(x - 1, y))
                );

                SetAdventureIslandLevel(x, y, level);
                SetIslandMaskCell(x, y, ISLAND_MASK_BLOCKED);
            }
        }
    }
}

static bool8 IsAdventureSatelliteCell(s16 x, s16 y)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    u8 i;

    for(i = 0; i < settings->satelliteCount; ++i)
    {
        const struct AdventureSatelliteIsland *satellite = &settings->satellites[i];

        if(x >= satellite->x && x < satellite->x + satellite->width
            && y >= satellite->y && y < satellite->y + ADVENTURE_SATELLITE_HEIGHT)
            return TRUE;
    }
    return FALSE;
}

static bool8 IsAdventureSatelliteCandidate(s16 x, s16 y, u8 width)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    bool8 nearMainIsland = FALSE;
    s16 offsetX;
    s16 offsetY;
    u8 i;

    if(x < 2 || y < 2
        || x + width + 2 > ADVENTURE_PATHS_MAP_WIDTH
        || y + ADVENTURE_SATELLITE_HEIGHT + 2 > ADVENTURE_PATHS_MAP_HEIGHT)
        return FALSE;

    // A two-cell void moat keeps satellites visually separate from the main
    // island and guarantees every exposed side remains at least two cells.
    for(offsetY = -2; offsetY < ADVENTURE_SATELLITE_HEIGHT + 2; ++offsetY)
    {
        for(offsetX = -2; offsetX < width + 2; ++offsetX)
        {
            if(IsAdventureIslandSurfaceCell(x + offsetX, y + offsetY))
                return FALSE;
        }
    }

    // Keep them close enough to the primary silhouette to enter the camera's
    // peripheral view instead of disappearing into unused map corners.
    for(offsetY = -6; offsetY < ADVENTURE_SATELLITE_HEIGHT + 6 && !nearMainIsland; ++offsetY)
    {
        for(offsetX = -6; offsetX < width + 6; ++offsetX)
        {
            if(IsAdventureIslandSurfaceCell(x + offsetX, y + offsetY))
            {
                nearMainIsland = TRUE;
                break;
            }
        }
    }
    if(!nearMainIsland)
        return FALSE;

    for(i = 0; i < settings->satelliteCount; ++i)
    {
        const struct AdventureSatelliteIsland *satellite = &settings->satellites[i];

        if(x < satellite->x + satellite->width + 2
            && x + width + 2 > satellite->x
            && y < satellite->y + ADVENTURE_SATELLITE_HEIGHT + 2
            && y + ADVENTURE_SATELLITE_HEIGHT + 2 > satellite->y)
            return FALSE;
    }
    return TRUE;
}

static bool8 TryAddAdventureSatellite(u8 ordinal)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    u8 attempt;

    for(attempt = 0; attempt < ADVENTURE_PERIPHERAL_PLACEMENT_ATTEMPTS; ++attempt)
    {
        u32 hash = GetAdventureIslandCoordHash(attempt, ordinal, 0x53415445);
        u8 width = ADVENTURE_SATELLITE_MIN_WIDTH
            + (hash % (ADVENTURE_SATELLITE_MAX_WIDTH - ADVENTURE_SATELLITE_MIN_WIDTH + 1));
        s16 x = 2 + ((hash >> 8) % (ADVENTURE_PATHS_MAP_WIDTH - width - 3));
        s16 y = 2 + ((hash >> 16) % (ADVENTURE_PATHS_MAP_HEIGHT - ADVENTURE_SATELLITE_HEIGHT - 3));

        if(!IsAdventureSatelliteCandidate(x, y, width))
            continue;

        settings->satellites[settings->satelliteCount].x = x;
        settings->satellites[settings->satelliteCount].y = y;
        settings->satellites[settings->satelliteCount].width = width;
        ++settings->satelliteCount;
        return TRUE;
    }
    return FALSE;
}

static bool8 IsAdventureDebrisCandidate(s16 x, s16 y, u8 width)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    bool8 nearLand = FALSE;
    s16 offsetX;
    s16 offsetY;
    u8 i;

    if(x < 1 || y < 1 || x + width + 1 > ADVENTURE_PATHS_MAP_WIDTH || y + 1 >= ADVENTURE_PATHS_MAP_HEIGHT)
        return FALSE;

    for(offsetX = -1; offsetX <= width; ++offsetX)
    {
        // Avoid both the main surface and its hanging cliff row.
        if(IsAdventureIslandSurfaceCell(x + offsetX, y)
            || IsAdventureIslandSurfaceCell(x + offsetX, y - 1)
            || IsAdventureSatelliteCell(x + offsetX, y))
            return FALSE;
    }

    for(offsetY = -4; offsetY <= 4 && !nearLand; ++offsetY)
    {
        for(offsetX = -4; offsetX < width + 4; ++offsetX)
        {
            if(IsAdventureIslandSurfaceCell(x + offsetX, y + offsetY)
                || IsAdventureSatelliteCell(x + offsetX, y + offsetY))
            {
                nearLand = TRUE;
                break;
            }
        }
    }
    if(!nearLand)
        return FALSE;

    for(i = 0; i < settings->debrisCount; ++i)
    {
        const struct AdventureFloatingDebris *debris = &settings->debris[i];

        // The debris artwork has transparent breathing room within each cell,
        // so neighboring placements remain distinct. Only reject actual map-
        // cell overlap; a larger exclusion halo prevents dense debris fields.
        if(y == debris->y
            && x < debris->x + debris->width
            && x + width > debris->x)
            return FALSE;
    }
    return TRUE;
}

static bool8 TryAddAdventureDebris(u8 ordinal, u8 width)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    u8 attempt;

    for(attempt = 0; attempt < ADVENTURE_PERIPHERAL_PLACEMENT_ATTEMPTS; ++attempt)
    {
        u32 hash = GetAdventureIslandCoordHash(attempt, ordinal, 0x44454252 ^ width);
        s16 x = 1 + ((hash >> 8) % (ADVENTURE_PATHS_MAP_WIDTH - width - 1));
        s16 y = 1 + ((hash >> 16) % (ADVENTURE_PATHS_MAP_HEIGHT - 2));

        if(!IsAdventureDebrisCandidate(x, y, width))
            continue;

        settings->debris[settings->debrisCount].x = x;
        settings->debris[settings->debrisCount].y = y;
        settings->debris[settings->debrisCount].width = width;
        ++settings->debrisCount;
        return TRUE;
    }
    return FALSE;
}

static void BuildAdventurePeripheralPlan(void)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    u8 satelliteTarget = (settings->visualHash >> 8) % (ADVENTURE_SATELLITE_MAX_COUNT + 1);
    u8 debrisTarget = ADVENTURE_DEBRIS_MIN_COUNT
        + ((settings->visualHash >> 16) % (ADVENTURE_DEBRIS_MAX_COUNT - ADVENTURE_DEBRIS_MIN_COUNT + 1));
    u8 i;
    u8 debrisOrdinal;

    settings->satelliteCount = 0;
    settings->debrisCount = 0;
    for(i = 0; i < satelliteTarget; ++i)
    {
        if(!TryAddAdventureSatellite(i))
            break;
    }

    // Keep one coherent two-cell formation, then densely scatter single-cell
    // debris around it. This yields 14-22 singles depending on the visual seed.
    for(debrisOrdinal = 0;
        settings->debrisCount < debrisTarget && debrisOrdinal < ADVENTURE_DEBRIS_MAX_COUNT * 4;
        ++debrisOrdinal)
    {
        u8 width = settings->debrisCount == 1 ? 2 : 1;

        TryAddAdventureDebris(debrisOrdinal, width);
    }
}

static u16 GetAdventureVoidMetatile(u16 x, u16 y)
{
    switch(GetAdventureIslandCoordHash(x, y, 0x564F4944) % 10)
    {
    case 0:
        return METATILE_AdventurePaths_Void_Stars0;
    case 1:
        return METATILE_AdventurePaths_Void_Stars1;
    case 2:
        return METATILE_AdventurePaths_Void_Stars2;
    default:
        return METATILE_AdventurePaths_Void;
    }
}

static bool8 GetAdventureSatelliteMetatile(s16 x, s16 y, u16 *metatile)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    u8 i;

    for(i = 0; i < settings->satelliteCount; ++i)
    {
        const struct AdventureSatelliteIsland *satellite = &settings->satellites[i];
        s16 relativeX;
        s16 relativeY;

        if(x < satellite->x || x >= satellite->x + satellite->width
            || y < satellite->y || y >= satellite->y + ADVENTURE_SATELLITE_HEIGHT)
            continue;

        relativeX = x - satellite->x;
        relativeY = y - satellite->y;
        if(relativeY == 0)
        {
            if(relativeX == 0)
                *metatile = METATILE_AdventurePaths_Satellite_SurfaceLeft;
            else if(relativeX == satellite->width - 1)
                *metatile = METATILE_AdventurePaths_Satellite_SurfaceRight;
            else
                *metatile = METATILE_AdventurePaths_Satellite_SurfaceMiddle0
                    + (GetAdventureIslandCoordHash(x, y, 0x53415454) % 3);
        }
        else if(relativeX == 0)
        {
            *metatile = METATILE_AdventurePaths_Island_Underside_Left;
        }
        else if(relativeX == satellite->width - 1)
        {
            *metatile = METATILE_AdventurePaths_Island_Underside_Right;
        }
        else
        {
            *metatile = METATILE_AdventurePaths_Island_Underside_Middle0
                + (GetAdventureIslandCoordHash(x, y, 0x53415442) % 3);
        }
        return TRUE;
    }
    return FALSE;
}

static bool8 GetAdventureDebrisMetatile(s16 x, s16 y, u16 *metatile)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();
    u8 i;

    for(i = 0; i < settings->debrisCount; ++i)
    {
        const struct AdventureFloatingDebris *debris = &settings->debris[i];

        if(y != debris->y || x < debris->x || x >= debris->x + debris->width)
            continue;
        if(debris->width == 1)
            *metatile = METATILE_AdventurePaths_Debris_Single0
                + (GetAdventureIslandCoordHash(x, y, 0x53484152) % ADVENTURE_DEBRIS_SINGLE_VARIANT_COUNT);
        else if(x == debris->x)
            *metatile = METATILE_AdventurePaths_Debris_PairLeft;
        else
            *metatile = METATILE_AdventurePaths_Debris_PairRight;
        return TRUE;
    }
    return FALSE;
}

static u16 GetAdventureIslandMetatile(u16 x, u16 y)
{
    u8 level = GetAdventureIslandLevel(x, y);
    bool8 north = GetAdventureIslandLevel(x, y - 1) >= level;
    bool8 east = GetAdventureIslandLevel(x + 1, y) >= level;
    bool8 south = GetAdventureIslandLevel(x, y + 1) >= level;
    bool8 west = GetAdventureIslandLevel(x - 1, y) >= level;
    bool8 northEast = GetAdventureIslandLevel(x + 1, y - 1) >= level;
    bool8 southEast = GetAdventureIslandLevel(x + 1, y + 1) >= level;
    bool8 southWest = GetAdventureIslandLevel(x - 1, y + 1) >= level;
    bool8 northWest = GetAdventureIslandLevel(x - 1, y - 1) >= level;

    // Keep the walkable surface on the ordinary 47-case cardinal outline.
    // The horizontal perspective rules belong to the visible wall row below,
    // where platform, edge, and void are distinguishable from one another.
    if(!north && !east)
        return METATILE_AdventurePaths_Island_Corner_NorthEast;
    if(!east && !south)
        return METATILE_AdventurePaths_Island_Corner_SouthEast;
    if(!south && !west)
        return METATILE_AdventurePaths_Island_Corner_SouthWest;
    if(!west && !north)
        return METATILE_AdventurePaths_Island_Corner_NorthWest;
    if(!north)
        return METATILE_AdventurePaths_Island_Edge_North;
    if(!east)
        return METATILE_AdventurePaths_Island_Edge_East;
    if(!south)
    {
        switch(GetAdventureIslandCoordHash(x, y, 0x534F5554) % 3)
        {
        case 1:
            return METATILE_AdventurePaths_Island_Edge_South1;
        case 2:
            return METATILE_AdventurePaths_Island_Edge_South2;
        default:
            return METATILE_AdventurePaths_Island_Edge_South;
        }
    }
    if(!west)
        return METATILE_AdventurePaths_Island_Edge_West;

    if(!northEast)
        return METATILE_AdventurePaths_Island_InnerCorner_NorthEast;
    if(!southEast)
        return METATILE_AdventurePaths_Island_InnerCorner_SouthEast;
    if(!southWest)
        return METATILE_AdventurePaths_Island_InnerCorner_SouthWest;
    if(!northWest)
        return METATILE_AdventurePaths_Island_InnerCorner_NorthWest;

    return METATILE_AdventurePaths_Island_Interior0
        + (GetAdventureIslandCoordHash(x, y, 0x524F434B) % 3);
}

static bool8 IsAdventureCliffCell(s16 x, s16 y)
{
    return !IsAdventureIslandSurfaceCell(x, y)
        && IsAdventureIslandSurfaceCell(x, y - 1);
}

static bool8 IsAdventureTerraceFaceCell(s16 x, s16 y)
{
    u8 level = GetAdventureIslandLevel(x, y);

    return level != 0 && GetAdventureIslandLevel(x, y - 1) > level;
}

enum AdventureWallNeighbor
{
    ADVENTURE_WALL_NEIGHBOR_NOTHING,
    ADVENTURE_WALL_NEIGHBOR_EDGE,
    ADVENTURE_WALL_NEIGHBOR_PLATFORM,
};

enum AdventureWallStyle
{
    ADVENTURE_WALL_STYLE_LEFT_SLANT,
    ADVENTURE_WALL_STYLE_SQUARE,
    ADVENTURE_WALL_STYLE_SHADOW,
    ADVENTURE_WALL_STYLE_RIGHT_SLANT,
};

static u8 GetAdventureWallNeighbor(s16 x, s16 y)
{
    if(IsAdventureCliffCell(x, y) || IsAdventureTerraceFaceCell(x, y))
        return ADVENTURE_WALL_NEIGHBOR_EDGE;
    if(IsAdventureIslandSurfaceCell(x, y))
        return ADVENTURE_WALL_NEIGHBOR_PLATFORM;
    return ADVENTURE_WALL_NEIGHBOR_NOTHING;
}

static u8 GetAdventureWallStyle(s16 x, s16 y)
{
    u8 left = GetAdventureWallNeighbor(x - 1, y);
    u8 right = GetAdventureWallNeighbor(x + 1, y);

    // These are deliberately horizontal contour rules, not a symmetric blob
    // autotiler. EDGE includes both square and slanted wall cells, so a style
    // never depends recursively on which artwork its neighbour selected.
    if(left == ADVENTURE_WALL_NEIGHBOR_NOTHING)
        return ADVENTURE_WALL_STYLE_LEFT_SLANT;
    if(left == ADVENTURE_WALL_NEIGHBOR_EDGE
        && right == ADVENTURE_WALL_NEIGHBOR_PLATFORM)
        return ADVENTURE_WALL_STYLE_SQUARE;
    if(left == ADVENTURE_WALL_NEIGHBOR_PLATFORM
        && right == ADVENTURE_WALL_NEIGHBOR_EDGE)
        return ADVENTURE_WALL_STYLE_SHADOW;
    if(right == ADVENTURE_WALL_NEIGHBOR_NOTHING)
        return ADVENTURE_WALL_STYLE_RIGHT_SLANT;
    return ADVENTURE_WALL_STYLE_SQUARE;
}

static u16 GetAdventureTerraceFaceMetatile(u16 x, u16 y)
{
    u8 variant = GetAdventureIslandCoordHash(x, y, 0x54455252) % 3;

    switch(GetAdventureWallStyle(x, y))
    {
    case ADVENTURE_WALL_STYLE_LEFT_SLANT:
        return METATILE_AdventurePaths_Terrace_FaceLeftSlant0 + variant;
    case ADVENTURE_WALL_STYLE_SHADOW:
        return METATILE_AdventurePaths_Terrace_FaceShadow0 + variant;
    case ADVENTURE_WALL_STYLE_RIGHT_SLANT:
        return METATILE_AdventurePaths_Terrace_FaceRightSlant0 + variant;
    default:
        return METATILE_AdventurePaths_Terrace_Face0 + variant;
    }
}

static u16 GetAdventureCliffMetatile(u16 x, u16 y)
{
    switch(GetAdventureWallStyle(x, y))
    {
    case ADVENTURE_WALL_STYLE_LEFT_SLANT:
        return METATILE_AdventurePaths_Island_Underside_Left;
    case ADVENTURE_WALL_STYLE_SHADOW:
        return METATILE_AdventurePaths_Island_Underside_Shadow0
            + (GetAdventureIslandCoordHash(x, y, 0x53484457) % 3);
    case ADVENTURE_WALL_STYLE_RIGHT_SLANT:
        return METATILE_AdventurePaths_Island_Underside_Right;
    default:
        break;
    }

    switch(GetAdventureIslandCoordHash(x, y, 0x434C4946) % 3)
    {
    case 1:
        return METATILE_AdventurePaths_Island_Underside_Middle1;
    case 2:
        return METATILE_AdventurePaths_Island_Underside_Middle2;
    default:
        return METATILE_AdventurePaths_Island_Underside_Middle0;
    }
}

#if 0
static bool8 IsAdventureLandmarkDecorationCandidate(s16 x, s16 y)
{
    s16 offsetX;
    s16 offsetY;

    // The anchor is the bottom cell of a 1x2 decoration. Both artwork cells
    // must be ordinary impassable rock, never a trail or blocker.
    if(!IsIslandCoordInBounds(x, y)
        || !IsIslandCoordInBounds(x, y - 1)
        || GetIslandMaskCell(x, y - 1) != ISLAND_MASK_BLOCKED
        || GetIslandMaskCell(x, y) != ISLAND_MASK_BLOCKED)
        return FALSE;

    // Preserve one complete island-surface tile around the artwork. This is
    // what prevents its baked rock background from cutting into the void.
    for(offsetY = -2; offsetY <= 1; ++offsetY)
    {
        for(offsetX = -1; offsetX <= 1; ++offsetX)
        {
            if(!IsAdventureIslandSurfaceCell(x + offsetX, y + offsetY))
                return FALSE;
        }
    }
    return TRUE;
}

static bool8 IsAdventureSurfaceDecorationCandidate(s16 x, s16 y)
{
    s16 offsetX;
    s16 offsetY;

    if(!IsIslandCoordInBounds(x, y)
        || GetIslandMaskCell(x, y) != ISLAND_MASK_BLOCKED)
        return FALSE;

    // Surface decals use an interior metatile background, so retain one ring
    // of island surface around them to keep that background seamless.
    for(offsetY = -1; offsetY <= 1; ++offsetY)
    {
        for(offsetX = -1; offsetX <= 1; ++offsetX)
        {
            if(!IsAdventureIslandSurfaceCell(x + offsetX, y + offsetY))
                return FALSE;
        }
    }
    return TRUE;
}

static bool8 IsAdventureEdgeDecorationCandidate(s16 x, s16 y, u8 *type)
{
    bool8 north;
    bool8 east;
    bool8 south;
    bool8 west;

    if(!IsIslandCoordInBounds(x, y)
        || GetIslandMaskCell(x, y) != ISLAND_MASK_BLOCKED)
        return FALSE;

    north = IsAdventureIslandSurfaceCell(x, y - 1);
    east = IsAdventureIslandSurfaceCell(x + 1, y);
    south = IsAdventureIslandSurfaceCell(x, y + 1);
    west = IsAdventureIslandSurfaceCell(x - 1, y);

    // V1 edge art supports a single exposed north, west, or east edge. South
    // edges are reserved for the cliff face and underside silhouette.
    if(!north && east && south && west)
        *type = GetAdventureIslandCoordHash(x, y, 0x45444745) % 2;
    else if(north && east && south && !west)
        *type = 2 + (GetAdventureIslandCoordHash(x, y, 0x45444745) % 2);
    else if(north && !east && south && west)
        *type = 4 + (GetAdventureIslandCoordHash(x, y, 0x45444745) % 2);
    else
        return FALSE;
    return TRUE;
}

static bool8 IsAdventureDecorationSeparated(const struct AdventureDecorationPlan *plan, s16 x, s16 y, u8 kind)
{
    u8 i;

    for(i = 0; i < plan->count; ++i)
    {
        s16 deltaX = x - plan->entries[i].x;
        s16 deltaY = y - plan->entries[i].y;

        if(kind == ADVENTURE_EDGE_DECORATION || plan->entries[i].kind == ADVENTURE_EDGE_DECORATION)
        {
            if(kind == ADVENTURE_EDGE_DECORATION
                && plan->entries[i].kind == ADVENTURE_EDGE_DECORATION
                && deltaX >= -3 && deltaX <= 3
                && deltaY >= -3 && deltaY <= 3)
                return FALSE;
            if(deltaX == 0 && deltaY == 0)
                return FALSE;
        }
        else if(kind == ADVENTURE_LANDMARK_DECORATION
            && plan->entries[i].kind == ADVENTURE_LANDMARK_DECORATION)
        {
            // A 1x2 footprint retains a complete empty row between landmarks.
            if(deltaX >= -1 && deltaX <= 1 && deltaY >= -2 && deltaY <= 2)
                return FALSE;
        }
        else if(kind == ADVENTURE_SURFACE_DECORATION
            && plan->entries[i].kind == ADVENTURE_SURFACE_DECORATION)
        {
            if(deltaX == 0 && deltaY == 0)
                return FALSE;
        }
        else
        {
            s16 surfaceX = kind == ADVENTURE_SURFACE_DECORATION ? x : plan->entries[i].x;
            s16 surfaceY = kind == ADVENTURE_SURFACE_DECORATION ? y : plan->entries[i].y;
            s16 landmarkX = kind == ADVENTURE_LANDMARK_DECORATION ? x : plan->entries[i].x;
            s16 landmarkY = kind == ADVENTURE_LANDMARK_DECORATION ? y : plan->entries[i].y;

            if(surfaceX == landmarkX
                && (surfaceY == landmarkY || surfaceY == landmarkY - 1))
                return FALSE;
        }
    }
    return TRUE;
}

static bool8 TryAddAdventureDecoration(struct AdventureDecorationPlan *plan, u8 kind, u8 ordinal, s16 minY, s16 maxY)
{
    bool8 foundCandidate = FALSE;
    u32 bestScore = 0;
    s16 bestX = 0;
    s16 bestY = 0;
    s16 x;
    s16 y;

    for(y = minY; y < maxY; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u32 score;
            u8 edgeType = 0;
            bool8 isCandidate;

            if(kind == ADVENTURE_LANDMARK_DECORATION)
                isCandidate = IsAdventureLandmarkDecorationCandidate(x, y);
            else if(kind == ADVENTURE_SURFACE_DECORATION)
                isCandidate = IsAdventureSurfaceDecorationCandidate(x, y);
            else
                isCandidate = IsAdventureEdgeDecorationCandidate(x, y, &edgeType);

            if(!isCandidate || !IsAdventureDecorationSeparated(plan, x, y, kind))
                continue;

            score = GetAdventureIslandCoordHash(x, y, (0x4445434F + kind * 0x11111111) ^ ordinal);
            if(!foundCandidate || score > bestScore)
            {
                foundCandidate = TRUE;
                bestScore = score;
                bestX = x;
                bestY = y;
            }
        }
    }

    if(!foundCandidate)
        return FALSE;

    plan->entries[plan->count].x = bestX;
    plan->entries[plan->count].y = bestY;
    plan->entries[plan->count].kind = kind;
    if(kind == ADVENTURE_EDGE_DECORATION)
        IsAdventureEdgeDecorationCandidate(bestX, bestY, &plan->entries[plan->count].type);
    else if(ordinal < ADVENTURE_DECORATION_TYPE_COUNT)
        plan->entries[plan->count].type = ((GetAdventureIslandCoordHash(0, 0, kind == ADVENTURE_SURFACE_DECORATION ? 0x53555246 : 0x4C414E44) % ADVENTURE_DECORATION_TYPE_COUNT) + ordinal * 5) % ADVENTURE_DECORATION_TYPE_COUNT;
    else
        plan->entries[plan->count].type = GetAdventureIslandCoordHash(bestX, bestY, kind == ADVENTURE_SURFACE_DECORATION ? 0x53555246 : 0x4C414E44) % ADVENTURE_DECORATION_TYPE_COUNT;
    ++plan->count;
    return TRUE;
}

static bool8 TryAddSurfaceDecorationNearLandmark(struct AdventureDecorationPlan *plan, u8 landmarkIndex, u8 ordinal)
{
    bool8 foundCandidate = FALSE;
    u32 bestScore = 0;
    s16 bestX = 0;
    s16 bestY = 0;
    s16 landmarkX = plan->entries[landmarkIndex].x;
    s16 landmarkY = plan->entries[landmarkIndex].y;
    s16 x;
    s16 y;

    for(y = landmarkY - 3; y <= landmarkY + 3; ++y)
    {
        for(x = landmarkX - 3; x <= landmarkX + 3; ++x)
        {
            s16 deltaX = x - landmarkX;
            s16 deltaY = y - landmarkY;
            s16 distance = (deltaX < 0 ? -deltaX : deltaX) + (deltaY < 0 ? -deltaY : deltaY);
            u32 score;

            if(distance < 2 || distance > 4
                || !IsAdventureSurfaceDecorationCandidate(x, y)
                || !IsAdventureDecorationSeparated(plan, x, y, ADVENTURE_SURFACE_DECORATION))
                continue;

            score = GetAdventureIslandCoordHash(x, y, 0x47524F55 ^ ordinal);
            if(!foundCandidate || score > bestScore)
            {
                foundCandidate = TRUE;
                bestScore = score;
                bestX = x;
                bestY = y;
            }
        }
    }

    if(!foundCandidate)
        return FALSE;

    plan->entries[plan->count].x = bestX;
    plan->entries[plan->count].y = bestY;
    plan->entries[plan->count].kind = ADVENTURE_SURFACE_DECORATION;
    plan->entries[plan->count].type = ((GetAdventureIslandCoordHash(0, 0, 0x53555246) % ADVENTURE_DECORATION_TYPE_COUNT) + ordinal * 5) % ADVENTURE_DECORATION_TYPE_COUNT;
    ++plan->count;
    return TRUE;
}

static void BuildAdventureDecorationPlan(struct AdventureDecorationPlan *plan)
{
    u8 landmarkTarget = ADVENTURE_LANDMARK_MIN_COUNT
        + (GetAdventureIslandCoordHash(0, 0, 0x4C4D434F) % (ADVENTURE_LANDMARK_MAX_COUNT - ADVENTURE_LANDMARK_MIN_COUNT + 1));
    u8 surfaceTarget = ADVENTURE_SURFACE_MIN_COUNT
        + (GetAdventureIslandCoordHash(0, 0, 0x5346434F) % (ADVENTURE_SURFACE_MAX_COUNT - ADVENTURE_SURFACE_MIN_COUNT + 1));
    u8 edgeTarget = ADVENTURE_EDGE_MIN_COUNT
        + (GetAdventureIslandCoordHash(0, 0, 0x4544434F) % (ADVENTURE_EDGE_MAX_COUNT - ADVENTURE_EDGE_MIN_COUNT + 1));
    u8 landmarkCount = 0;
    u8 surfaceCount = 0;
    u8 edgeCount = 0;
    u8 band;
    u8 i;

    memset(plan, 0, sizeof(*plan));

    for(band = 0; band < 4 && landmarkCount < landmarkTarget; ++band)
    {
        if(TryAddAdventureDecoration(plan, ADVENTURE_LANDMARK_DECORATION, landmarkCount,
            band * ADVENTURE_PATHS_MAP_HEIGHT / 4,
            (band + 1) * ADVENTURE_PATHS_MAP_HEIGHT / 4))
            ++landmarkCount;
    }
    while(landmarkCount < landmarkTarget
        && TryAddAdventureDecoration(plan, ADVENTURE_LANDMARK_DECORATION, landmarkCount, 0, ADVENTURE_PATHS_MAP_HEIGHT))
        ++landmarkCount;

    // Compose formations by giving each landmark one nearby quiet detail
    // before adding any independent surface variation.
    for(i = 0; i < landmarkCount && surfaceCount < surfaceTarget; ++i)
    {
        if(TryAddSurfaceDecorationNearLandmark(plan, i, surfaceCount))
            ++surfaceCount;
    }

    for(band = 0; band < 4 && surfaceCount < surfaceTarget; ++band)
    {
        if(TryAddAdventureDecoration(plan, ADVENTURE_SURFACE_DECORATION, surfaceCount,
            band * ADVENTURE_PATHS_MAP_HEIGHT / 4,
            (band + 1) * ADVENTURE_PATHS_MAP_HEIGHT / 4))
            ++surfaceCount;
    }
    while(surfaceCount < surfaceTarget
        && TryAddAdventureDecoration(plan, ADVENTURE_SURFACE_DECORATION, surfaceCount, 0, ADVENTURE_PATHS_MAP_HEIGHT))
        ++surfaceCount;

    while(edgeCount < edgeTarget
        && TryAddAdventureDecoration(plan, ADVENTURE_EDGE_DECORATION, edgeCount, 0, ADVENTURE_PATHS_MAP_HEIGHT))
        ++edgeCount;
}

static bool8 GetAdventureDecorationPart(const struct AdventureDecorationPlan *plan, s16 x, s16 y, u8 *type, u8 *kind, bool8 *bottom)
{
    u8 i;

    for(i = 0; i < plan->count; ++i)
    {
        if(x != plan->entries[i].x)
            continue;
        if(y == plan->entries[i].y)
            *bottom = TRUE;
        else if(plan->entries[i].kind == ADVENTURE_LANDMARK_DECORATION && y == plan->entries[i].y - 1)
            *bottom = FALSE;
        else
            continue;

        *type = plan->entries[i].type;
        *kind = plan->entries[i].kind;
        return TRUE;
    }
    return FALSE;
}

static u16 GetAdventureDecorationMetatile(u8 type, u8 kind, bool8 bottom)
{
    if(kind == ADVENTURE_SURFACE_DECORATION)
        return METATILE_AdventurePaths_Decor_ShardCyan + type;
    if(kind == ADVENTURE_EDGE_DECORATION)
        return METATILE_AdventurePaths_Decor_EdgeCrystalCyan_North + type;
    return METATILE_AdventurePaths_Decor_CrystalCyan_Top + type * 2 + (bottom ? 1 : 0);
}

#endif

static u16 GetAdventureSurfaceTemplateMetatile(s16 x, s16 y)
{
    if(!IsIslandCoordInBounds(x, y))
        return METATILE_AdventurePaths_Island_Interior0;
    return GetAdventureIslandVisualSettings()->surfaceTemplate[y * ADVENTURE_PATHS_MAP_WIDTH + x]
        & MAPGRID_METATILE_ID_MASK;
}

static u16 GetAdventureOffsetSurfaceTemplateMetatile(s16 x, s16 y)
{
    struct AdventureIslandVisualSettings *settings = GetAdventureIslandVisualSettings();

    return GetAdventureSurfaceTemplateMetatile(
        x + settings->templateOffsetX,
        y + settings->templateOffsetY
    );
}

static bool8 IsAdventureFormationMetatile(u16 metatile)
{
    return (metatile >= METATILE_AdventurePaths_FormationA_TopLeft
            && metatile <= METATILE_AdventurePaths_FormationA_BottomRight)
        || (metatile >= METATILE_AdventurePaths_FormationB_TopLeft
            && metatile <= METATILE_AdventurePaths_FormationB_BottomRight);
}

static bool8 GetAdventureFormationAnchor(s16 x, s16 y, s16 *anchorX, s16 *anchorY, u16 *baseMetatile)
{
    u16 metatile = GetAdventureSurfaceTemplateMetatile(x, y);
    u16 offset;

    if(metatile >= METATILE_AdventurePaths_FormationA_TopLeft
        && metatile <= METATILE_AdventurePaths_FormationA_BottomRight)
    {
        *baseMetatile = METATILE_AdventurePaths_FormationA_TopLeft;
    }
    else if(metatile >= METATILE_AdventurePaths_FormationB_TopLeft
        && metatile <= METATILE_AdventurePaths_FormationB_BottomRight)
    {
        *baseMetatile = METATILE_AdventurePaths_FormationB_TopLeft;
    }
    else
    {
        return FALSE;
    }

    offset = metatile - *baseMetatile;
    *anchorX = x - (offset & 1);
    *anchorY = y - (offset / 2);
    return TRUE;
}

static bool8 IsAdventureFormationValid(s16 anchorX, s16 anchorY, u16 baseMetatile)
{
    u8 level = GetAdventureIslandLevel(anchorX, anchorY);
    s16 offsetX;
    s16 offsetY;

    if(level == 0)
        return FALSE;

    // Match the original tree cleanup: the complete authored footprint must
    // survive on one uninterrupted plane without crossing a trail.
    for(offsetY = 0; offsetY < 3; ++offsetY)
    {
        for(offsetX = 0; offsetX < 2; ++offsetX)
        {
            s16 x = anchorX + offsetX;
            s16 y = anchorY + offsetY;

            if(GetAdventureIslandLevel(x, y) != level)
                return FALSE;
            if(HasIslandMaskFlag(x, y, ISLAND_MASK_TRAIL))
                return FALSE;
            if(IsAdventureTerraceFaceCell(x, y))
                return FALSE;
        }
    }

    for(offsetY = 0; offsetY < 3; ++offsetY)
    {
        for(offsetX = 0; offsetX < 2; ++offsetX)
        {
            if(GetAdventureSurfaceTemplateMetatile(anchorX + offsetX, anchorY + offsetY)
                != baseMetatile + offsetY * 2 + offsetX)
                return FALSE;
        }
    }
    return TRUE;
}

static u8 GetAdventureFormationBackgroundIndex(u16 metatile)
{
    if(metatile >= METATILE_AdventurePaths_Island_Interior0
        && metatile <= METATILE_AdventurePaths_Island_Interior2)
        return metatile - METATILE_AdventurePaths_Island_Interior0;
    if(metatile == METATILE_AdventurePaths_Island_Edge_North)
        return 3;
    if(metatile == METATILE_AdventurePaths_Island_Edge_East)
        return 4;
    if(metatile == METATILE_AdventurePaths_Island_Edge_South
        || metatile == METATILE_AdventurePaths_Island_Edge_South1
        || metatile == METATILE_AdventurePaths_Island_Edge_South2)
        return 5;
    if(metatile == METATILE_AdventurePaths_Island_Edge_West)
        return 6;
    if(metatile >= METATILE_AdventurePaths_Island_Corner_NorthEast
        && metatile <= METATILE_AdventurePaths_Island_Corner_NorthWest)
        return 7 + metatile - METATILE_AdventurePaths_Island_Corner_NorthEast;
    if(metatile >= METATILE_AdventurePaths_Island_InnerCorner_NorthEast
        && metatile <= METATILE_AdventurePaths_Island_InnerCorner_NorthWest)
        return 11 + metatile - METATILE_AdventurePaths_Island_InnerCorner_NorthEast;
    return 0;
}

static bool8 IsAdventureTemplateAccentMetatile(u16 metatile)
{
    return (metatile >= METATILE_AdventurePaths_Surface_Fractured0
            && metatile <= METATILE_AdventurePaths_Surface_Pebbles1)
        || (metatile >= METATILE_AdventurePaths_Surface_Rock0
            && metatile <= METATILE_AdventurePaths_Surface_MineralViolet)
        || (metatile >= METATILE_AdventurePaths_Surface_CrystalClusterBlue0
            && metatile <= METATILE_AdventurePaths_Surface_CrystalClusterBlue1);
}

static bool8 IsAdventureInteriorPlaneCell(s16 x, s16 y)
{
    u8 level = GetAdventureIslandLevel(x, y);

    return level != 0
        && GetAdventureIslandLevel(x, y - 1) == level
        && GetAdventureIslandLevel(x + 1, y) == level
        && GetAdventureIslandLevel(x, y + 1) == level
        && GetAdventureIslandLevel(x - 1, y) == level
        && !IsAdventureTerraceFaceCell(x, y);
}

static u16 GetAdventureTemplateFallbackMetatile(s16 x, s16 y)
{
    return METATILE_AdventurePaths_Island_Interior0
        + (GetAdventureIslandCoordHash(x, y, 0x544D504C) % 3);
}

static u16 GetAdventureTemplateMetatile(s16 x, s16 y)
{
    u16 metatile = GetAdventureSurfaceTemplateMetatile(x, y);

    if(IsAdventureFormationMetatile(metatile))
    {
        s16 anchorX;
        s16 anchorY;
        u16 baseMetatile;

        if(GetAdventureFormationAnchor(x, y, &anchorX, &anchorY, &baseMetatile)
            && IsAdventureFormationValid(anchorX, anchorY, baseMetatile))
        {
            u16 overlayBase = baseMetatile == METATILE_AdventurePaths_FormationA_TopLeft
                ? METATILE_AdventurePaths_FormationOverlayA_Base
                : METATILE_AdventurePaths_FormationOverlayB_Base;
            u8 part = metatile - baseMetatile;
            u8 background = GetAdventureFormationBackgroundIndex(GetAdventureIslandMetatile(x, y));

            return overlayBase + part * ADVENTURE_FORMATION_BACKGROUND_COUNT + background;
        }
        return GetAdventureTemplateFallbackMetatile(x, y);
    }

    // Large formations retain their carefully composed anchors. Shift the
    // one-cell surface details, which can move safely without splitting art.
    metatile = GetAdventureOffsetSurfaceTemplateMetatile(x, y);
    if(IsAdventureTemplateAccentMetatile(metatile))
    {
        if(IsAdventureInteriorPlaneCell(x, y))
            return metatile;
        return GetAdventureTemplateFallbackMetatile(x, y);
    }

    if(metatile >= METATILE_AdventurePaths_Island_Interior0
        && metatile <= METATILE_AdventurePaths_Island_Interior2)
        return metatile;
    return GetAdventureTemplateFallbackMetatile(x, y);
}

static u16 GetAdventureTrailMetatile(u16 x, u16 y)
{
    u8 connections = 0;

    if(HasIslandMaskFlag(x, y - 1, ISLAND_MASK_TRAIL))
        connections |= 1 << 0;
    if(HasIslandMaskFlag(x + 1, y, ISLAND_MASK_TRAIL))
        connections |= 1 << 1;
    if(HasIslandMaskFlag(x, y + 1, ISLAND_MASK_TRAIL))
        connections |= 1 << 2;
    if(HasIslandMaskFlag(x - 1, y, ISLAND_MASK_TRAIL))
        connections |= 1 << 3;

    if(HasIslandMaskFlag(x, y, ISLAND_MASK_BLOCKED))
    {
        if((connections & ((1 << 1) | (1 << 3))) != 0)
            return METATILE_AdventurePaths_Trail_BlockedHorizontal;
        return METATILE_AdventurePaths_Trail_BlockedVertical;
    }

    switch(connections)
    {
    case 0:
        return METATILE_AdventurePaths_Trail_Centre;
    case (1 << 0):
        return METATILE_AdventurePaths_Trail_EndNorth;
    case (1 << 1):
        return METATILE_AdventurePaths_Trail_EndEast;
    case (1 << 2):
        return METATILE_AdventurePaths_Trail_EndSouth;
    case (1 << 3):
        return METATILE_AdventurePaths_Trail_EndWest;
    case (1 << 0) | (1 << 2):
        return METATILE_AdventurePaths_Trail_Vertical;
    case (1 << 1) | (1 << 3):
        return METATILE_AdventurePaths_Trail_Horizontal;
    case (1 << 0) | (1 << 1):
        return METATILE_AdventurePaths_Trail_Corner_NorthEast;
    case (1 << 1) | (1 << 2):
        return METATILE_AdventurePaths_Trail_Corner_SouthEast;
    case (1 << 2) | (1 << 3):
        return METATILE_AdventurePaths_Trail_Corner_SouthWest;
    case (1 << 3) | (1 << 0):
        return METATILE_AdventurePaths_Trail_Corner_NorthWest;
    case (1 << 1) | (1 << 2) | (1 << 3):
        return METATILE_AdventurePaths_Trail_T_MissingNorth;
    case (1 << 0) | (1 << 2) | (1 << 3):
        return METATILE_AdventurePaths_Trail_T_MissingEast;
    case (1 << 0) | (1 << 1) | (1 << 3):
        return METATILE_AdventurePaths_Trail_T_MissingSouth;
    case (1 << 0) | (1 << 1) | (1 << 2):
        return METATILE_AdventurePaths_Trail_T_MissingWest;
    default:
        return METATILE_AdventurePaths_Trail_Cross;
    }
}

static u16 GetAdventureRenderedMetatile(s16 x, s16 y, bool8 *impassable)
{
    u8 mask = GetIslandMaskCell(x, y);
    u16 peripheralMetatile;

    *impassable = TRUE;
    if((mask & ISLAND_MASK_TRAIL) != 0)
    {
        *impassable = (mask & ISLAND_MASK_BLOCKED) != 0;
        return GetAdventureTrailMetatile(x, y);
    }
    if(IsAdventureTerraceFaceCell(x, y))
        return GetAdventureTerraceFaceMetatile(x, y);
    if(IsAdventureIslandSurfaceCell(x, y))
    {
        u16 metatile = GetAdventureIslandMetatile(x, y);

        if(metatile >= METATILE_AdventurePaths_Island_Interior0
            && metatile <= METATILE_AdventurePaths_Island_Interior2)
            return GetAdventureTemplateMetatile(x, y);
        return metatile;
    }
    if(IsAdventureCliffCell(x, y))
        return GetAdventureCliffMetatile(x, y);
    if(GetAdventureSatelliteMetatile(x, y, &peripheralMetatile)
        || GetAdventureDebrisMetatile(x, y, &peripheralMetatile))
        return peripheralMetatile;
    return GetAdventureVoidMetatile(x, y);
}

static void PaintAdventureIsland(void)
{
    u16 x;
    u16 y;

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            bool8 impassable;
            u16 metatile = GetAdventureRenderedMetatile(x, y, &impassable);

            if(impassable)
                metatile |= MAPGRID_COLLISION_MASK;
            MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, metatile);
        }
    }
}

void RogueAdv_ApplyAdventureMetatiles()
{
    bool8 allTrailCellsInBounds;
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    u32 renderStartClock = RogueDebug_SampleClock();
#endif

    AGB_ASSERT(gMapHeader.mapLayout->width == ADVENTURE_PATHS_MAP_WIDTH);
    AGB_ASSERT(gMapHeader.mapLayout->height == ADVENTURE_PATHS_MAP_HEIGHT);

    if(!AllocAdventureIslandMask())
    {
        AGB_ASSERT(FALSE);
        return;
    }

    allTrailCellsInBounds = BuildAdventureIslandMask(NULL);
    AGB_ASSERT(allTrailCellsInBounds);
    PaintAdventureIsland();
    FreeAdventureIslandMask();
#ifdef DEBUG_FEATURE_FRAME_TIMERS
    DebugPrintf("[Adventure Path] Island rendering: %d us", RogueDebug_ClockToDisplayUnits(RogueDebug_SampleClock() - renderStartClock));
#endif
}

#ifdef ROGUE_DEBUG
static bool8 IsAdventureIslandConnected(void)
{
    bool8 changed;
    bool8 foundStart = FALSE;
    u16 trailCount = 0;
    u16 visitedCount = 0;
    s16 x;
    s16 y;

    // Reuse the blocker bit as a temporary visited bit. The caller rebuilds
    // the deterministic blocker state before hashing or querying it again.
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 cell = GetIslandMaskCell(x, y);

            cell &= ~ISLAND_MASK_BLOCKED;
            if((cell & ISLAND_MASK_TRAIL) != 0)
            {
                ++trailCount;
                if(!foundStart)
                {
                    cell |= ISLAND_MASK_BLOCKED;
                    foundStart = TRUE;
                }
            }
            SetIslandMaskCell(x, y, cell);
        }
    }

    do
    {
        changed = FALSE;
        for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
        {
            for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
            {
                if(!HasIslandMaskFlag(x, y, ISLAND_MASK_TRAIL)
                    || HasIslandMaskFlag(x, y, ISLAND_MASK_BLOCKED))
                    continue;

                if((HasIslandMaskFlag(x, y - 1, ISLAND_MASK_TRAIL) && HasIslandMaskFlag(x, y - 1, ISLAND_MASK_BLOCKED))
                    || (HasIslandMaskFlag(x + 1, y, ISLAND_MASK_TRAIL) && HasIslandMaskFlag(x + 1, y, ISLAND_MASK_BLOCKED))
                    || (HasIslandMaskFlag(x, y + 1, ISLAND_MASK_TRAIL) && HasIslandMaskFlag(x, y + 1, ISLAND_MASK_BLOCKED))
                    || (HasIslandMaskFlag(x - 1, y, ISLAND_MASK_TRAIL) && HasIslandMaskFlag(x - 1, y, ISLAND_MASK_BLOCKED)))
                {
                    SetIslandMaskCell(x, y, GetIslandMaskCell(x, y) | ISLAND_MASK_BLOCKED);
                    changed = TRUE;
                }
            }
        }
    } while(changed);

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            if(HasIslandMaskFlag(x, y, ISLAND_MASK_TRAIL)
                && HasIslandMaskFlag(x, y, ISLAND_MASK_BLOCKED))
            {
                ++visitedCount;
            }
        }
    }
    return trailCount != 0 && trailCount == visitedCount;
}

#if 0
static bool8 IsAdventureDecorationPlanValid(const struct AdventureDecorationPlan *plan)
{
    struct AdventureDecorationPlan prefix = {0};
    u8 i;

    if(plan->count > ADVENTURE_DECORATION_MAX_COUNT)
        return FALSE;

    for(i = 0; i < plan->count; ++i)
    {
        bool8 bottomImpassable;
        bool8 isCandidate;
        u8 edgeType = 0;
        u16 bottomMetatile;

        if(plan->entries[i].kind == ADVENTURE_LANDMARK_DECORATION)
            isCandidate = IsAdventureLandmarkDecorationCandidate(plan->entries[i].x, plan->entries[i].y);
        else if(plan->entries[i].kind == ADVENTURE_SURFACE_DECORATION)
            isCandidate = IsAdventureSurfaceDecorationCandidate(plan->entries[i].x, plan->entries[i].y);
        else if(plan->entries[i].kind == ADVENTURE_EDGE_DECORATION)
            isCandidate = IsAdventureEdgeDecorationCandidate(plan->entries[i].x, plan->entries[i].y, &edgeType);
        else
            return FALSE;

        if(plan->entries[i].type >= ADVENTURE_DECORATION_TYPE_COUNT
            || !isCandidate
            || (plan->entries[i].kind == ADVENTURE_EDGE_DECORATION && edgeType != plan->entries[i].type)
            || !IsAdventureDecorationSeparated(&prefix, plan->entries[i].x, plan->entries[i].y, plan->entries[i].kind))
            return FALSE;

        bottomMetatile = GetAdventureRenderedMetatile(plan, plan->entries[i].x, plan->entries[i].y, &bottomImpassable);
        if(!bottomImpassable
            || bottomMetatile != GetAdventureDecorationMetatile(plan->entries[i].type, plan->entries[i].kind, TRUE))
            return FALSE;

        if(plan->entries[i].kind == ADVENTURE_LANDMARK_DECORATION)
        {
            bool8 topImpassable;
            u16 topMetatile = GetAdventureRenderedMetatile(plan, plan->entries[i].x, plan->entries[i].y - 1, &topImpassable);

            if(!topImpassable
                || topMetatile != GetAdventureDecorationMetatile(plan->entries[i].type, ADVENTURE_LANDMARK_DECORATION, FALSE))
                return FALSE;
        }

        prefix.entries[prefix.count++] = plan->entries[i];
    }
    return TRUE;
}
#endif

bool8 RogueAdv_Debug_ValidateIslandLayout(u32 *layoutHash)
{
    bool8 isValid;
    u32 hash = 2166136261u;
    s16 x;
    s16 y;

    if(!AllocAdventureIslandMask())
        return FALSE;

    isValid = BuildAdventureIslandMask(NULL);
    isValid &= IsAdventureIslandConnected();
    isValid &= BuildAdventureIslandMask(NULL);

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u8 cell = GetIslandMaskCell(x, y);
            u8 level = GetAdventureIslandLevel(x, y);
            bool8 impassable;
            u16 metatile = GetAdventureRenderedMetatile(x, y, &impassable);

            if((cell & ISLAND_MASK_TRAIL) != 0 && level == 0)
                isValid = FALSE;
            if((cell & ISLAND_MASK_TRAIL) != 0 && (cell & ISLAND_MASK_BLOCKED) == 0 && impassable)
                isValid = FALSE;
            if(((cell & ISLAND_MASK_TRAIL) == 0 || (cell & ISLAND_MASK_BLOCKED) != 0) && !impassable)
                isValid = FALSE;
            hash = (hash ^ cell) * 16777619u;
            hash = (hash ^ level) * 16777619u;
            hash = (hash ^ metatile) * 16777619u;
        }
    }

    if(layoutHash != NULL)
        *layoutHash = hash;
    FreeAdventureIslandMask();
    return isValid;
}

bool8 RogueAdv_Debug_ValidateIslandWallStyles(u16 *styleCounts)
{
    bool8 isValid = TRUE;
    s16 x;
    s16 y;

    if(!AllocAdventureIslandMask())
        return FALSE;

    BuildAdventureIslandMask(NULL);
    if(styleCounts != NULL)
        memset(styleCounts, 0, sizeof(u16) * 4);

    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            bool8 isTerrace = IsAdventureTerraceFaceCell(x, y);
            bool8 isCliff = IsAdventureCliffCell(x, y);
            u8 style;
            u16 metatile;

            if(!isTerrace && !isCliff)
                continue;

            style = GetAdventureWallStyle(x, y);
            metatile = isTerrace
                ? GetAdventureTerraceFaceMetatile(x, y)
                : GetAdventureCliffMetatile(x, y);
            if(styleCounts != NULL)
                ++styleCounts[style];

            switch(style)
            {
            case ADVENTURE_WALL_STYLE_LEFT_SLANT:
                isValid &= isTerrace
                    ? metatile >= METATILE_AdventurePaths_Terrace_FaceLeftSlant0
                        && metatile <= METATILE_AdventurePaths_Terrace_FaceLeftSlant2
                    : metatile == METATILE_AdventurePaths_Island_Underside_Left;
                break;
            case ADVENTURE_WALL_STYLE_SHADOW:
                isValid &= isTerrace
                    ? metatile >= METATILE_AdventurePaths_Terrace_FaceShadow0
                        && metatile <= METATILE_AdventurePaths_Terrace_FaceShadow2
                    : metatile >= METATILE_AdventurePaths_Island_Underside_Shadow0
                        && metatile <= METATILE_AdventurePaths_Island_Underside_Shadow2;
                break;
            case ADVENTURE_WALL_STYLE_RIGHT_SLANT:
                isValid &= isTerrace
                    ? metatile >= METATILE_AdventurePaths_Terrace_FaceRightSlant0
                        && metatile <= METATILE_AdventurePaths_Terrace_FaceRightSlant2
                    : metatile == METATILE_AdventurePaths_Island_Underside_Right;
                break;
            default:
                isValid &= isTerrace
                    ? metatile >= METATILE_AdventurePaths_Terrace_Face0
                        && metatile <= METATILE_AdventurePaths_Terrace_Face2
                    : metatile >= METATILE_AdventurePaths_Island_Underside_Middle0
                        && metatile <= METATILE_AdventurePaths_Island_Underside_Middle2;
                break;
            }
        }
    }

    FreeAdventureIslandMask();
    return isValid;
}

bool8 RogueAdv_Debug_GetIslandPeripheralStats(u8 *satelliteCount, u8 *singleDebrisCount, u8 *pairedDebrisCount)
{
    struct AdventureIslandVisualSettings *settings;
    bool8 isValid = TRUE;
    u8 singles = 0;
    u8 pairs = 0;
    u8 i;

    if(!AllocAdventureIslandMask())
        return FALSE;

    BuildAdventureIslandMask(NULL);
    settings = GetAdventureIslandVisualSettings();
    if(settings->satelliteCount > ADVENTURE_SATELLITE_MAX_COUNT
        || settings->debrisCount > ADVENTURE_DEBRIS_MAX_COUNT)
        isValid = FALSE;

    for(i = 0; i < settings->satelliteCount; ++i)
    {
        const struct AdventureSatelliteIsland *satellite = &settings->satellites[i];
        s16 x;
        s16 y;
        u8 j;

        if(satellite->width < ADVENTURE_SATELLITE_MIN_WIDTH
            || satellite->width > ADVENTURE_SATELLITE_MAX_WIDTH
            || satellite->x + satellite->width > ADVENTURE_PATHS_MAP_WIDTH
            || satellite->y + ADVENTURE_SATELLITE_HEIGHT > ADVENTURE_PATHS_MAP_HEIGHT)
            isValid = FALSE;

        for(y = satellite->y; y < satellite->y + ADVENTURE_SATELLITE_HEIGHT; ++y)
        {
            for(x = satellite->x; x < satellite->x + satellite->width; ++x)
            {
                u16 metatile;

                if(IsAdventureIslandSurfaceCell(x, y)
                    || !GetAdventureSatelliteMetatile(x, y, &metatile))
                    isValid = FALSE;
            }
        }

        for(j = i + 1; j < settings->satelliteCount; ++j)
        {
            const struct AdventureSatelliteIsland *other = &settings->satellites[j];

            if(satellite->x < other->x + other->width
                && satellite->x + satellite->width > other->x
                && satellite->y < other->y + ADVENTURE_SATELLITE_HEIGHT
                && satellite->y + ADVENTURE_SATELLITE_HEIGHT > other->y)
                isValid = FALSE;
        }
    }

    for(i = 0; i < settings->debrisCount; ++i)
    {
        const struct AdventureFloatingDebris *debris = &settings->debris[i];
        u16 metatile;

        if(debris->width == 1)
            ++singles;
        else if(debris->width == 2)
            ++pairs;
        else
            isValid = FALSE;
        if(!GetAdventureDebrisMetatile(debris->x, debris->y, &metatile))
            isValid = FALSE;
    }

    if(satelliteCount != NULL)
        *satelliteCount = settings->satelliteCount;
    if(singleDebrisCount != NULL)
        *singleDebrisCount = singles;
    if(pairedDebrisCount != NULL)
        *pairedDebrisCount = pairs;
    FreeAdventureIslandMask();
    return isValid;
}

bool8 RogueAdv_Debug_HasBlockedIslandTrail(void)
{
    bool8 foundBlockedTrail = FALSE;
    s16 x;
    s16 y;

    if(!AllocAdventureIslandMask())
        return FALSE;

    BuildAdventureIslandMask(NULL);
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            if((GetIslandMaskCell(x, y) & (ISLAND_MASK_TRAIL | ISLAND_MASK_BLOCKED))
                == (ISLAND_MASK_TRAIL | ISLAND_MASK_BLOCKED))
            {
                foundBlockedTrail = TRUE;
                break;
            }
        }
        if(foundBlockedTrail)
            break;
    }
    FreeAdventureIslandMask();
    return foundBlockedTrail;
}

bool8 RogueAdv_Debug_GetIslandGeologyStats(u16 *formationCount, u16 *accentCount, u16 *crystalCount, u8 *terraceStage)
{
    u16 formations = 0;
    u16 accents = 0;
    u16 crystals = 0;
    s16 x;
    s16 y;

    if(!AllocAdventureIslandMask())
        return FALSE;

    BuildAdventureIslandMask(NULL);
    for(y = 0; y < ADVENTURE_PATHS_MAP_HEIGHT; ++y)
    {
        for(x = 0; x < ADVENTURE_PATHS_MAP_WIDTH; ++x)
        {
            u16 metatile = GetAdventureTemplateMetatile(x, y);
            u16 sourceMetatile = GetAdventureSurfaceTemplateMetatile(x, y);

            if(sourceMetatile == METATILE_AdventurePaths_FormationA_TopLeft
                || sourceMetatile == METATILE_AdventurePaths_FormationB_TopLeft)
            {
                s16 anchorX;
                s16 anchorY;
                u16 baseMetatile;

                if(GetAdventureFormationAnchor(x, y, &anchorX, &anchorY, &baseMetatile)
                    && IsAdventureFormationValid(anchorX, anchorY, baseMetatile))
                    ++formations;
            }
            else if(IsAdventureTemplateAccentMetatile(metatile)
                && IsAdventureIslandSurfaceCell(x, y))
            {
                ++accents;
                if(metatile >= METATILE_AdventurePaths_Surface_CrystalClusterBlue0
                    && metatile <= METATILE_AdventurePaths_Surface_CrystalClusterBlue1)
                    ++crystals;
            }
        }
    }
    FreeAdventureIslandMask();

    if(formationCount != NULL)
        *formationCount = formations;
    if(accentCount != NULL)
        *accentCount = accents;
    if(crystalCount != NULL)
        *crystalCount = crystals;
    if(terraceStage != NULL)
        *terraceStage = GetAdventureTerraceStage();
    return TRUE;
}

bool8 RogueAdv_Debug_GetIslandVisualStats(s8 *templateOffsetX, s8 *templateOffsetY, u16 *keptInnerFringe, u16 *prunedInnerFringe, u16 *outerFringe, u16 *narrowFringe)
{
    struct AdventureIslandVisualStats visualStats;
    struct AdventureIslandVisualSettings *settings;

    if(!AllocAdventureIslandMask())
        return FALSE;
    if(!BuildAdventureIslandMask(&visualStats))
    {
        FreeAdventureIslandMask();
        return FALSE;
    }
    settings = GetAdventureIslandVisualSettings();

    if(templateOffsetX != NULL)
        *templateOffsetX = settings->templateOffsetX;
    if(templateOffsetY != NULL)
        *templateOffsetY = settings->templateOffsetY;
    if(keptInnerFringe != NULL)
        *keptInnerFringe = visualStats.keptInnerFringe;
    if(prunedInnerFringe != NULL)
        *prunedInnerFringe = visualStats.prunedInnerFringe;
    if(outerFringe != NULL)
        *outerFringe = visualStats.outerFringe;
    if(narrowFringe != NULL)
        *narrowFringe = visualStats.narrowFringe;

    FreeAdventureIslandMask();
    return TRUE;
}
#endif


static void SetBossRoomWarp(u16 trainerNum, struct WarpData* warp)
{
    if(GetPathGenerationDifficulty() == ROGUE_FINAL_CHAMP_DIFFICULTY)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_13);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_13);
    }
    else if(GetPathGenerationDifficulty() == ROGUE_CHAMP_START_DIFFICULTY)
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_12);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_12);
    }
    else if(GetPathGenerationDifficulty() >= ROGUE_ELITE_START_DIFFICULTY)
    {
        Rogue_GetPreferredElite4Map(trainerNum, &warp->mapGroup, &warp->mapNum);
    }
    else
    {
        warp->mapGroup = MAP_GROUP(ROGUE_BOSS_0);
        warp->mapNum = MAP_NUM(ROGUE_BOSS_0);
    }
}

static void ApplyCurrentNodeWarp(struct WarpData *warp)
{
    struct RogueAdvPathRoom* room = &gRogueAdvPath.rooms[gRogueRun.adventureRoomId];

    SeedRogueRng(room->rngSeed);

    switch(room->roomType)
    {
        case ADVPATH_ROOM_BOSS:
            SetBossRoomWarp(room->roomParams.perType.boss.trainerNum, warp);
            break;

        case ADVPATH_ROOM_RESTSTOP:
            warp->mapGroup = gRogueRestStopEncounterInfo.mapTable[room->roomParams.roomIdx].group;
            warp->mapNum = gRogueRestStopEncounterInfo.mapTable[room->roomParams.roomIdx].num;
            break;

        case ADVPATH_ROOM_ROUTE:
            warp->mapGroup = gRogueRouteTable.routes[room->roomParams.roomIdx].map.group;
            warp->mapNum = gRogueRouteTable.routes[room->roomParams.roomIdx].map.num;
            break;

        case ADVPATH_ROOM_LEGENDARY:
            warp->mapGroup = gRogueLegendaryEncounterInfo.mapTable[room->roomParams.roomIdx].group;
            warp->mapNum = gRogueLegendaryEncounterInfo.mapTable[room->roomParams.roomIdx].num;
            break;

        case ADVPATH_ROOM_TEAM_HIDEOUT:
            warp->mapGroup = gRogueTeamEncounterInfo.mapTable[room->roomParams.roomIdx].group;
            warp->mapNum = gRogueTeamEncounterInfo.mapTable[room->roomParams.roomIdx].num;
            break;

        case ADVPATH_ROOM_MINIBOSS:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_MINI_BOSS);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_MINI_BOSS);
            break;

        case ADVPATH_ROOM_WILD_DEN:
        case ADVPATH_ROOM_UNIQUE_DEN:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_DEN);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_DEN);
            break;

        case ADVPATH_ROOM_HONEY_TREE:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_HONEY_TREE);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_HONEY_TREE);
            break;

        case ADVPATH_ROOM_GAMESHOW:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_GAME_SHOW);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_GAME_SHOW);
            break;

        case ADVPATH_ROOM_DARK_DEAL:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_GRAVEYARD);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_GRAVEYARD);
            break;

        case ADVPATH_ROOM_LAB:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_LAB);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_LAB);
            break;

        case ADVPATH_ROOM_SHRINE:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_SHRINE);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_SHRINE);
            break;

        case ADVPATH_ROOM_CATCHING_CONTEST:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_CATCHING_CONTEST);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_CATCHING_CONTEST);
            break;

        case ADVPATH_ROOM_SIGN:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_SIGN);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_SIGN);
            break;

        case ADVPATH_ROOM_BATTLE_SIM:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_BATTLE_SIM);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_BATTLE_SIM);
            break;

        case ADVPATH_ROOM_BATTLE_TOWER:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_BATTLE_TOWER);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_BATTLE_TOWER);
            break;

        case ADVPATH_ROOM_ITEM:
            warp->mapGroup = MAP_GROUP(ROGUE_ENCOUNTER_ITEM_ROOM);
            warp->mapNum = MAP_NUM(ROGUE_ENCOUNTER_ITEM_ROOM);
            break;
    }
}

u8 RogueAdv_OverrideNextWarp(struct WarpData *warp)
{
    // Should already be set correctly for RogueAdv_WarpLastInteractedRoom
    if(!gRogueAdvPath.isOverviewActive)
    {
        bool8 freshPath = RogueAdv_GenerateAdventurePathsIfRequired();

        // Always jump back to overview screen, after a different route
        warp->mapGroup = MAP_GROUP(ROGUE_ADVENTURE_PATHS);
        warp->mapNum = MAP_NUM(ROGUE_ADVENTURE_PATHS);
        warp->warpId = WARP_ID_NONE;

        if(freshPath || gRogueRun.adventureRoomId == ADVPATH_INVALID_ROOM_ID)
        {
            // Warp to initial start line
            // find start/end coords
            u8 i, x, y;
            u8 minY = (u8)-1;
            u8 maxY = 0;

            x = 0;
            y = 0;

            for(i = 0; i < gRogueAdvPath.roomCount; ++i)
            {
                // Count if in first column
                if(gRogueAdvPath.rooms[i].coords.x == gRogueAdvPath.pathLength - 1)
                {
                    // Move coords into world space
                    x = ROOM_TO_WARP_X(gRogueAdvPath.rooms[i].coords.x); 
                    y = ROOM_TO_WARP_Y(gRogueAdvPath.rooms[i].coords.y);

                    minY = min(minY, y);
                    maxY = max(maxY, y);
                }
            }

            warp->x = x - 2;
            warp->y = minY + (maxY - minY) / 2;
        }
        else
        {
            warp->x = ROOM_TO_WARP_X(gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x);
            warp->y = ROOM_TO_WARP_Y(gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.y);
        }

        // Trigger before we wipe the room type
        if(gRogueAdvPath.currentRoomType == ADVPATH_ROOM_ROUTE)
            RogueRouteScenes_OnExitRoute();

        RogueQuest_OnTrigger(QUEST_TRIGGER_EXIT_ENCOUNTER);

        gRogueAdvPath.currentRoomType = ADVPATH_ROOM_NONE;
        return ROGUE_WARP_TO_ADVPATH;
    }
    else
    {
        ApplyCurrentNodeWarp(warp);
        return ROGUE_WARP_TO_ROOM;
    }
}

extern const u8 Rogue_AdventurePaths_InteractRoom[];

void RogueAdv_ModifyObjectEvents(struct MapHeader *mapHeader, struct ObjectEventTemplate *objectEvents, u8* objectEventCount, u8 objectEventCapacity)
{
    u8 i;
    u8 writeIdx;
    u8 x, y;

    writeIdx = 0;

    // Draw room path
    for(i = 0; i < gRogueAdvPath.roomCount; ++i)
    {
        // Move coords into world space
        x = ROOM_TO_OBJECT_EVENT_X(gRogueAdvPath.rooms[i].coords.x);
        y = ROOM_TO_OBJECT_EVENT_Y(gRogueAdvPath.rooms[i].coords.y);

        if(writeIdx < objectEventCapacity)
        {
            if(IsObjectEventVisible(&gRogueAdvPath.rooms[i]))
            {
                if(gRogueAdvPath.rooms[i].roomType == ADVPATH_ROOM_ITEM)
                    FollowMon_SetGraphics(0, SPECIES_SABLEYE, FALSE, 0);

                objectEvents[writeIdx].localId = writeIdx;
                objectEvents[writeIdx].graphicsId = SelectObjectGfxForRoom(&gRogueAdvPath.rooms[i]);
                objectEvents[writeIdx].x = x;
                objectEvents[writeIdx].y = y;
                objectEvents[writeIdx].elevation = 3;
                objectEvents[writeIdx].trainerType = TRAINER_TYPE_NONE;
                objectEvents[writeIdx].movementType = SelectObjectMovementTypeForRoom(&gRogueAdvPath.rooms[i]);

                // Pack node into this var
                objectEvents[writeIdx].trainerRange_berryTreeId = i;
                objectEvents[writeIdx].script = Rogue_AdventurePaths_InteractRoom;

                ++writeIdx;
            }
        }
        else
        {
            DebugPrintf("WARNING: Cannot add adventure path object %d (out of range %d)", writeIdx, objectEventCapacity);
        }
    }

    *objectEventCount = writeIdx;
}

bool8 RogueAdv_CanUseEscapeRope(void)
{
    if(!gRogueAdvPath.isOverviewActive)
    {
        // We are in transition i.e. just started the run
        if(gRogueAdvPath.roomCount == 0)
            return FALSE;
        
        switch(gRogueAdvPath.currentRoomType)
        {
            case ADVPATH_ROOM_BOSS:
                return FALSE;

            default:
                return TRUE;
        }
    }

    return FALSE;
}

u8 Rogue_GetTypeForHintForRoom(struct RogueAdvPathRoom const* room)
{
    return gRogueRouteTable.routes[room->roomParams.roomIdx].wildTypeTable[(room->coords.x + room->coords.y) % ARRAY_COUNT(gRogueRouteTable.routes[0].wildTypeTable)];
}

static u16 SelectObjectGfxForRoom(struct RogueAdvPathRoom* room)
{
    switch(room->roomType)
    {
        case ADVPATH_ROOM_NONE:
            return 0;
            
        case ADVPATH_ROOM_ROUTE:
        {
            switch(Rogue_GetTypeForHintForRoom(room))
            {
                case TYPE_BUG:
                    return OBJ_EVENT_GFX_ROUTE_BUG;
                case TYPE_DARK:
                    return OBJ_EVENT_GFX_ROUTE_DARK;
                case TYPE_DRAGON:
                    return OBJ_EVENT_GFX_ROUTE_DRAGON;
                case TYPE_ELECTRIC:
                    return OBJ_EVENT_GFX_ROUTE_ELECTRIC;
#ifdef ROGUE_EXPANSION
                case TYPE_FAIRY:
                    return OBJ_EVENT_GFX_ROUTE_FAIRY;
#endif
                case TYPE_FIGHTING:
                    return OBJ_EVENT_GFX_ROUTE_FIGHTING;
                case TYPE_FIRE:
                    return OBJ_EVENT_GFX_ROUTE_FIRE;
                case TYPE_FLYING:
                    return OBJ_EVENT_GFX_ROUTE_FLYING;
                case TYPE_GHOST:
                    return OBJ_EVENT_GFX_ROUTE_GHOST;
                case TYPE_GRASS:
                    return OBJ_EVENT_GFX_ROUTE_GRASS;
                case TYPE_GROUND:
                    return OBJ_EVENT_GFX_ROUTE_GROUND;
                case TYPE_ICE:
                    return OBJ_EVENT_GFX_ROUTE_ICE;
                case TYPE_NORMAL:
                    return OBJ_EVENT_GFX_ROUTE_NORMAL;
                case TYPE_POISON:
                    return OBJ_EVENT_GFX_ROUTE_POISON;
                case TYPE_PSYCHIC:
                    return OBJ_EVENT_GFX_ROUTE_PSYCHIC;
                case TYPE_ROCK:
                    return OBJ_EVENT_GFX_ROUTE_ROCK;
                case TYPE_STEEL:
                    return OBJ_EVENT_GFX_ROUTE_STEEL;
                case TYPE_WATER:
                    return OBJ_EVENT_GFX_ROUTE_WATER;

                default:
                //case TYPE_MYSTERY:
                    return OBJ_EVENT_GFX_ROUTE_MYSTERY;
            }
        }

        case ADVPATH_ROOM_RESTSTOP:
            return gRogueRestStopEncounterInfo.mapTable[room->roomParams.roomIdx].encounterId;

        case ADVPATH_ROOM_LEGENDARY:
            if(room->roomParams.perType.legendary.customMonId != 0)
                return OBJ_EVENT_GFX_GOLD_LEGENDARY_STATUE;
            return OBJ_EVENT_GFX_TRICK_HOUSE_STATUE;

        case ADVPATH_ROOM_TEAM_HIDEOUT:
        {
            u8 gender = (room->coords.x + room->coords.y) % 2;

            switch (gRogueRun.teamEncounterNum)
            {
            case TEAM_NUM_KANTO_ROCKET:
            case TEAM_NUM_JOHTO_ROCKET:
                return gender ? OBJ_EVENT_GFX_ROCKET_M : OBJ_EVENT_GFX_ROCKET_F;

            case TEAM_NUM_AQUA:
                return gender ? OBJ_EVENT_GFX_AQUA_MEMBER_M : OBJ_EVENT_GFX_AQUA_MEMBER_F;

            case TEAM_NUM_MAGMA:
                return gender ? OBJ_EVENT_GFX_MAGMA_MEMBER_M : OBJ_EVENT_GFX_MAGMA_MEMBER_F;

            case TEAM_NUM_GALACTIC:
                return gender ? OBJ_EVENT_GFX_TEAM_GALACTIC_GRUNT_M : OBJ_EVENT_GFX_TEAM_GALACTIC_GRUNT_F;

            case TEAM_NUM_PLASMA:
                return gender ? OBJ_EVENT_GFX_TEAM_PLASMA_GRUNT_M : OBJ_EVENT_GFX_TEAM_PLASMA_GRUNT_F;

            case TEAM_NUM_NEOPLASMA:
                return gender ? OBJ_EVENT_GFX_TEAM_NEO_PLASMA_GRUNT_M : OBJ_EVENT_GFX_TEAM_NEO_PLASMA_GRUNT_F;

            case TEAM_NUM_FLARE:
                return gender ? OBJ_EVENT_GFX_TEAM_FLARE_GRUNT_M : OBJ_EVENT_GFX_TEAM_FLARE_GRUNT_F;

            default:
                AGB_ASSERT(FALSE);
                return OBJ_EVENT_GFX_ROCKET_M;
            }
        }

        case ADVPATH_ROOM_MINIBOSS:
            return Rogue_GetTrainerObjectEventGfx(room->roomParams.perType.miniboss.trainerNum);

        case ADVPATH_ROOM_WILD_DEN:
            return OBJ_EVENT_GFX_GRASS_DEFAULT;

        case ADVPATH_ROOM_UNIQUE_DEN:
            return OBJ_EVENT_GFX_UNIQUE_DEN_GRASS;

        case ADVPATH_ROOM_HONEY_TREE:
            return OBJ_EVENT_GFX_GOLD_GRASS;

        case ADVPATH_ROOM_GAMESHOW:
            return OBJ_EVENT_GFX_CONTEST_JUDGE;

        case ADVPATH_ROOM_DARK_DEAL:
            return OBJ_EVENT_GFX_DEVIL_MAN;

        case ADVPATH_ROOM_LAB:
            return OBJ_EVENT_GFX_PC;

        case ADVPATH_ROOM_SHRINE:
            return OBJ_EVENT_GFX_MISC_CHANNELER;

        case ADVPATH_ROOM_CATCHING_CONTEST:
            return OBJ_EVENT_GFX_MISC_BUG_CATCHER;

        case ADVPATH_ROOM_SIGN:
            return OBJ_EVENT_GFX_SMALL_SIGN;

        case ADVPATH_ROOM_BATTLE_SIM:
            return OBJ_EVENT_GFX_YOUNGSTER;

        case ADVPATH_ROOM_BATTLE_TOWER:
            return OBJ_EVENT_GFX_MISC_YOUNG_COUPLE_F;

        case ADVPATH_ROOM_ITEM:
            return OBJ_EVENT_GFX_FOLLOW_MON_0;

        case ADVPATH_ROOM_BOSS:
            return OBJ_EVENT_GFX_BATTLE_STATUE;
    }

    return 0;
}

static u8 SelectObjectMovementTypeForRoom(struct RogueAdvPathRoom* room)
{
    switch(room->roomType)
    {
        case ADVPATH_ROOM_ITEM:
            return MOVEMENT_TYPE_FACE_DOWN;

        case ADVPATH_ROOM_ROUTE:
        {
            switch(room->roomParams.perType.route.difficulty)
            {
                case 1: // ADVPATH_SUBROOM_ROUTE_AVERAGE
                    return MOVEMENT_TYPE_FACE_UP;
                case 2: // ADVPATH_SUBROOM_ROUTE_TOUGH
                    return MOVEMENT_TYPE_FACE_LEFT;
                default: // ADVPATH_SUBROOM_ROUTE_CALM
                    return MOVEMENT_TYPE_NONE;
            };
        }
    }

    return MOVEMENT_TYPE_NONE;
}

static bool8 IsObjectEventVisible(struct RogueAdvPathRoom* room)
{
    if(room->roomType == ADVPATH_ROOM_NONE)
        return FALSE;

    if(gRogueAdvPath.justGenerated)
    {
        // Everything is visible
        return TRUE;
    }
    else
    {
        u8 focusX = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x;
        return room->coords.x < focusX;
    }
}

static bool8 ShouldBlockObjectEvent(struct RogueAdvPathRoom* room)
{
    if(gRogueAdvPath.justGenerated)
    {
        return FALSE;
    }
    else
    {
        u8 focusX = gRogueAdvPath.rooms[gRogueRun.adventureRoomId].coords.x;
        return room->coords.x == focusX;
    }
}

static void BufferTypeAdjective(u8 type)
{
    const u8 gText_AdjNormal[] = _("Typical");
    const u8 gText_AdjFighting[] = _("Mighty");
    const u8 gText_AdjFlying[] = _("Breezy");
    const u8 gText_AdjPoison[] = _("Corrosive");
    const u8 gText_AdjGround[] = _("Coarse");
    const u8 gText_AdjRock[] = _("Rugged");
    const u8 gText_AdjBug[] = _("Swarming");
    const u8 gText_AdjGhost[] = _("Spooky");
    const u8 gText_AdjSteel[] = _("Sharp");
    const u8 gText_AdjFire[] = _("Warm");
    const u8 gText_AdjWater[] = _("Wet");
    const u8 gText_AdjGrass[] = _("Verdant");
    const u8 gText_AdjElectric[] = _("Energetic");
    const u8 gText_AdjPsychic[] = _("Confusing");
    const u8 gText_AdjIce[] = _("Chilly");
    const u8 gText_AdjDragon[] = _("Fierce");
    const u8 gText_AdjDark[] = _("Gloomy");
#ifdef ROGUE_EXPANSION
    const u8 gText_AdjFairy[] = _("Magical");
#endif
    const u8 gText_AdjNone[] = _("???");

    switch(type)
    {
        case TYPE_NORMAL:
            StringCopy(gStringVar1, gText_AdjNormal);
            break;

        case TYPE_FIGHTING:
            StringCopy(gStringVar1, gText_AdjFighting);
            break;

        case TYPE_FLYING:
            StringCopy(gStringVar1, gText_AdjFlying);
            break;

        case TYPE_POISON:
            StringCopy(gStringVar1, gText_AdjPoison);
            break;

        case TYPE_GROUND:
            StringCopy(gStringVar1, gText_AdjGround);
            break;

        case TYPE_ROCK:
            StringCopy(gStringVar1, gText_AdjRock);
            break;

        case TYPE_BUG:
            StringCopy(gStringVar1, gText_AdjBug);
            break;

        case TYPE_GHOST:
            StringCopy(gStringVar1, gText_AdjGhost);
            break;

        case TYPE_STEEL:
            StringCopy(gStringVar1, gText_AdjSteel);
            break;

        case TYPE_FIRE:
            StringCopy(gStringVar1, gText_AdjFire);
            break;

        case TYPE_WATER:
            StringCopy(gStringVar1, gText_AdjWater);
            break;

        case TYPE_GRASS:
            StringCopy(gStringVar1, gText_AdjGrass);
            break;

        case TYPE_ELECTRIC:
            StringCopy(gStringVar1, gText_AdjElectric);
            break;

        case TYPE_PSYCHIC:
            StringCopy(gStringVar1, gText_AdjPsychic);
            break;

        case TYPE_ICE:
            StringCopy(gStringVar1, gText_AdjIce);
            break;

        case TYPE_DRAGON:
            StringCopy(gStringVar1, gText_AdjDragon);
            break;

        case TYPE_DARK:
            StringCopy(gStringVar1, gText_AdjDark);
            break;

#ifdef ROGUE_EXPANSION
        case TYPE_FAIRY:
            StringCopy(gStringVar1, gText_AdjFairy);
            break;
#endif

        default:
            StringCopy(gStringVar1, gText_AdjNone);
            break;
    }
}

static u8 GetRoomIndexFromLastInteracted()
{
    u16 lastTalkedId = VarGet(VAR_LAST_TALKED);

    // We have to lookup into the template as this var gets zeroed when it's not being used in a valid way
    return gSaveBlock1Ptr->objectEventTemplates[lastTalkedId].trainerRange_berryTreeId;

    //u8 objEventId = GetObjectEventIdByLocalIdAndMap(lastTalkedId, gSaveBlock1Ptr->location.mapNum, gSaveBlock1Ptr->location.mapGroup);
    //u8 roomIdx = gObjectEvents[objEventId].trainerRange_berryTreeId;
    //return roomIdx;
}

void RogueAdv_GetLastInteractedRoomParams()
{
    u8 roomIdx = GetRoomIndexFromLastInteracted();

    gSpecialVar_ScriptNodeParam0 = gRogueAdvPath.rooms[roomIdx].roomType;
    gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.roomIdx;

    switch(gRogueAdvPath.rooms[roomIdx].roomType)
    {
        case ADVPATH_ROOM_ROUTE:
            gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.perType.route.difficulty;
            BufferTypeAdjective(Rogue_GetTypeForHintForRoom(&gRogueAdvPath.rooms[roomIdx]));
            break;

        case ADVPATH_ROOM_LEGENDARY:
            gSpecialVar_ScriptNodeParam1 = gRogueAdvPath.rooms[roomIdx].roomParams.perType.legendary.customMonId != 0;
            break;

        case ADVPATH_ROOM_MINIBOSS:
            Rogue_BufferMiniBossPreview(roomIdx);
            break;
    }
}

bool8 Rogue_SafeSmartCheckInternal();

void RogueAdv_WarpLastInteractedRoom()
{
    struct WarpData warp;
    u8 roomIdx = GetRoomIndexFromLastInteracted();

    if(Rogue_SafeSmartCheckInternal())
    {
        // Move to the selected node
        gRogueRun.adventureRoomId = roomIdx;
        gRogueAdvPath.currentRoomType = gRogueAdvPath.rooms[roomIdx].roomType;
        memcpy(&gRogueAdvPath.currentRoomParams, &gRogueAdvPath.rooms[roomIdx].roomParams, sizeof(gRogueAdvPath.currentRoomParams));

        // Fill with dud warp
        warp.mapGroup = MAP_GROUP(ROGUE_HUB_TRANSITION);
        warp.mapNum = MAP_NUM(ROGUE_HUB_TRANSITION);
        warp.warpId = 0;
        warp.x = -1;
        warp.y = -1;
    }
    else
    {
        warp.mapGroup = MAP_GROUP(ROGUE_AREA_ADVENTURE_ENTRANCE);
        warp.mapNum = MAP_NUM(ROGUE_AREA_ADVENTURE_ENTRANCE);
        warp.warpId = 0;
        warp.x = -1;
        warp.y = -1;
    }


    SetWarpDestination(warp.mapGroup, warp.mapNum, warp.warpId, warp.x, warp.y);
    DoWarp();
    ResetInitialPlayerAvatarState();
}
