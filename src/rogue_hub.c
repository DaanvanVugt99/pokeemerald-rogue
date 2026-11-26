#include "global.h"
#include "data/park_tunnel_patches.h"
#include "constants/portal_room_tiles.h"
#include "constants/lab_junction_tiles.h"
#include "constants/main_hall_tiles.h"
#include "constants/safari_lab_tiles.h"
#include "constants/berry_lab_tiles.h"
#include "constants/supply_depot_tiles.h"
#include "constants/pokemon_nursery_tiles.h"
#include "constants/event_objects.h"
#include "constants/layouts.h"
#include "constants/metatile_labels.h"

#include "event_data.h"
#include "field_camera.h"
#include "fieldmap.h"
#include "overworld.h"
#include "pokemon.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"

#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_multiplayer.h"
#include "rogue_quest.h"

#define HUB_TOWN_GRID_MIN_X -7
#define HUB_TOWN_GRID_MAX_X 7
#define HUB_TOWN_GRID_MIN_Y -5
#define HUB_TOWN_GRID_MAX_Y 4

static void MetatileSet_Tile(u16 xStart, u16 yStart, u16 tile);
static void MetatileFill_Tile(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u16 tile);
static void RogueHub_UpdateLabsAreaMetatiles();
static void RogueHub_UpdateAdventureEntranceAreaMetatiles();
static void RogueHub_UpdateFarmingAreaMetatiles();
static void RogueHub_UpdateSafariAreaMetatiles();
static void RogueHub_UpdateRideTrainingAreaMetatiles();
static void RogueHub_UpdateMartsAreaMetatiles();
static void RogueHub_UpdateTownSquareAreaMetatiles();
static void RogueHub_UpdateMarketAreaMetatiles();
static void RogueHub_UpdateTrialFrontierAreaMetatiles();
static void RogueHub_UpdateDayCareAreaMetatiles();

static void BuildAtRandomConnectionFrom(u8 fromArea, u8 buildArea);

static void FixupGrassPaths(void);

void RogueHub_Enter()
{
    //AGB_ASSERT(gRogueSaveBlock->hubMap == NULL);
    //gRogueSaveBlock->hubMap = AllocZeroed(sizeof(struct RogueHubMap));
}

void RogueHub_Exit()
{
    //AGB_ASSERT(gRogueSaveBlock->hubMap != NULL);
    //free(gRogueSaveBlock->hubMap);
    //gRogueSaveBlock->hubMap = NULL;
}

static struct RogueHubMap* GetActiveHubMap()
{
    if(RogueMP_IsActive() && !RogueMP_IsHost())
    {
        AGB_ASSERT(gRogueMultiplayer != NULL);
        return &gRogueMultiplayer->gameState.hub.hubMap;
    }

    return &gRogueSaveBlock->hubMap;
}

void RogueHub_ClearProgress()
{
    memset(&gRogueSaveBlock->hubMap, 0, sizeof(gRogueSaveBlock->hubMap));

    // Build default area at away from reserved coords (will recentre in a second)
    RogueHub_BuildArea(HUB_AREA_LABS, 10, 10);

    // Place required areas randomly (Order matters)
    BuildAtRandomConnectionFrom(HUB_AREA_LABS, HUB_AREA_ADVENTURE_ENTRANCE);
    BuildAtRandomConnectionFrom(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_SAFARI_ZONE);
    BuildAtRandomConnectionFrom(HUB_AREA_LABS, HUB_AREA_TOWN_SQUARE);

    // Now recenter so that the adventure enterance is actually at 0,0
    {
        u8 i;
        struct Coords8 offset = gRogueSaveBlock->hubMap.areaCoords[HUB_AREA_ADVENTURE_ENTRANCE];

        for(i = 0; i < HUB_AREA_COUNT; ++i)
        {
            if(RogueHub_HasAreaBuilt(i))
            {
                gRogueSaveBlock->hubMap.areaCoords[i].x -= offset.x;
                gRogueSaveBlock->hubMap.areaCoords[i].y -= offset.y;
            }
        }
    }
}

bool8 RogueHub_HasUpgrade(u16 upgradeId)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(GetActiveHubMap()->upgradeFlags));
    return (GetActiveHubMap()->upgradeFlags[idx] & bitMask) != 0;
}

bool8 RogueHub_HasLocalUpgrade(u16 upgradeId)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(GetActiveHubMap()->upgradeFlags));
    return (gRogueSaveBlock->hubMap.upgradeFlags[idx] & bitMask) != 0;
}

bool8 RogueHub_HasAllLocalUpgrades()
{
    u32 i = 0;

    for(i = 0; i < HUB_UPGRADE_COUNT; ++i)
    {
        // Ignore quest unlocks
        if(gRogueHubUpgrades[i].buildCost == 0)
            continue;

        if(!RogueHub_HasLocalUpgrade(i))
            return FALSE;
    }

    return TRUE;
}

void RogueHub_SetUpgrade(u16 upgradeId, bool8 state)
{
    u16 idx = upgradeId / 8;
    u16 bit = upgradeId % 8;

    u8 bitMask = 1 << bit;
    
    AGB_ASSERT(idx < ARRAY_COUNT(GetActiveHubMap()->upgradeFlags));
    if(state)
    {
        GetActiveHubMap()->upgradeFlags[idx] |= bitMask;
    }
    else
    {
        GetActiveHubMap()->upgradeFlags[idx] &= ~bitMask;
    }
}

bool8 RogueHub_HasUpgradeRequirements(u16 upgradeId)
{
    u8 i;
    u8 check;

    if(upgradeId == HUB_UPGRADE_ADVENTURE_ENTRANCE_TRIAL_CONSOLE
        && !FlagGet(FLAG_SYS_TRIALS_UNLOCKED))
        return FALSE;

    if(!RogueHub_HasAreaBuilt(gRogueHubUpgrades[upgradeId].targetArea))
        return FALSE;

    for(i = 0; i < HUB_UPGRADE_MAX_REQUIREMENTS; ++i)
    {
        check = gRogueHubUpgrades[upgradeId].requiredUpgrades[i];
        if(check == HUB_UPGRADE_NONE)
            break;

        if(!RogueHub_HasUpgrade(check))
            return FALSE;
    }

    return TRUE;
}

bool8 RogueHub_HasAreaBuilt(u8 area)
{
    u16 idx = area / 8;
    u16 bit = area % 8;

    u8 bitMask = 1 << bit;

    AGB_ASSERT(idx < ARRAY_COUNT(GetActiveHubMap()->areaBuiltFlags));
    return (GetActiveHubMap()->areaBuiltFlags[idx] & bitMask) != 0;
}

void RogueHub_BuildArea(u8 area, s8 x, s8 y)
{
    u16 idx = area / 8;
    u16 bit = area % 8;

    u8 bitMask = 1 << bit;
    
    AGB_ASSERT(idx < ARRAY_COUNT(GetActiveHubMap()->areaBuiltFlags));
    
    GetActiveHubMap()->areaBuiltFlags[idx] |= bitMask;
    GetActiveHubMap()->areaCoords[area].x = x;
    GetActiveHubMap()->areaCoords[area].y = y;
}

static void IncrementCoordsByDirection(struct Coords8* coords, u8 dir)
{
    switch (dir)
    {
    case HUB_AREA_CONN_SOUTH:
        --coords->y;
        break;
    case HUB_AREA_CONN_NORTH:
        ++coords->y;
        break;
    case HUB_AREA_CONN_WEST:
        --coords->x;
        break;
    case HUB_AREA_CONN_EAST:
        ++coords->x;
        break;
    }
}

bool8 RogueHub_HasAreaBuildRequirements(u8 area)
{
    u8 i;
    u8 check;

    // Cannot build until post game
    if(area == HUB_AREA_TRIAL_FRONTIER)
    {
        if(!FlagGet(FLAG_ROGUE_MET_POKABBIE))
            return FALSE;
    }

    for(i = 0; i < HUB_UPGRADE_MAX_REQUIREMENTS; ++i)
    {
        check = gRogueHubAreas[area].requiredUpgrades[i];
        if(check == HUB_UPGRADE_NONE)
            break;

        if(!RogueHub_HasUpgrade(check))
            return FALSE;
    }

    return TRUE;
}

u8 RogueHub_FindAreaAtCoord(s8 x, s8 y)
{
    u8 i;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(RogueHub_HasAreaBuilt(i))
        {
            if(GetActiveHubMap()->areaCoords[i].x == x && GetActiveHubMap()->areaCoords[i].y == y)
                return i;
        }
    }

    return HUB_AREA_NONE;
}

// Catch unbuildable coordinates
static bool8 IsReservedCoord(s8 x, s8 y)
{
    // Reserve north of adventure entrance
    if(GetActiveHubMap()->areaCoords[HUB_AREA_ADVENTURE_ENTRANCE].x == x && GetActiveHubMap()->areaCoords[HUB_AREA_ADVENTURE_ENTRANCE].y + 1 == y)
        return TRUE;

    // Reserve north of safari
    if(GetActiveHubMap()->areaCoords[HUB_AREA_SAFARI_ZONE].x == x && GetActiveHubMap()->areaCoords[HUB_AREA_SAFARI_ZONE].y + 1 == y)
        return TRUE;

    return FALSE;
}

bool8 RogueHub_IsCoordInTownGrid(s8 x, s8 y)
{
    return x >= HUB_TOWN_GRID_MIN_X
        && x <= HUB_TOWN_GRID_MAX_X
        && y >= HUB_TOWN_GRID_MIN_Y
        && y <= HUB_TOWN_GRID_MAX_Y;
}

struct Coords8 RogueHub_GetAreaCoords(u8 area)
{
    AGB_ASSERT(RogueHub_HasAreaBuilt(area));
    return GetActiveHubMap()->areaCoords[area];
}

u8 RogueHub_FindAreaInDir(u8 area, u8 connDir)
{
    if(RogueHub_HasAreaBuilt(area))
    {
        struct Coords8 pos;
        pos.x = GetActiveHubMap()->areaCoords[area].x;
        pos.y = GetActiveHubMap()->areaCoords[area].y;
        IncrementCoordsByDirection(&pos, connDir);

        return RogueHub_FindAreaAtCoord(pos.x, pos.y);
    }

    return HUB_AREA_NONE;
}

static bool8 CanAreaConnect(u8 area, u8 dir)
{
    // If both warps aren't set to 0, this is a valid warp
    return gRogueHubAreas[area].connectionWarps[dir][0] != 0 || gRogueHubAreas[area].connectionWarps[dir][1] != 0;
}

static u8 InvertConnDirection(u8 dir)
{
    switch (dir)
    {
    case HUB_AREA_CONN_SOUTH:
        return HUB_AREA_CONN_NORTH;
    case HUB_AREA_CONN_NORTH:
        return HUB_AREA_CONN_SOUTH;
    case HUB_AREA_CONN_WEST:
        return HUB_AREA_CONN_EAST;
    case HUB_AREA_CONN_EAST:
        return HUB_AREA_CONN_WEST;
    }

    return dir;
}

bool8 RogueHub_AreaHasFreeConnection(u8 area, u8 dir)
{
    if(CanAreaConnect(area, dir))
    {
        struct Coords8 pos;
        pos.x = GetActiveHubMap()->areaCoords[area].x;
        pos.y = GetActiveHubMap()->areaCoords[area].y;
        IncrementCoordsByDirection(&pos, dir);

        return !IsReservedCoord(pos.x, pos.y) && RogueHub_FindAreaAtCoord(pos.x, pos.y) == HUB_AREA_NONE;
    }

    return FALSE;
}

static struct Coords8 GetAreaCoordsAfterMove(u8 area, u8 movingArea, s8 x, s8 y)
{
    if(area == movingArea)
    {
        struct Coords8 coords = {x, y};
        return coords;
    }

    return GetActiveHubMap()->areaCoords[area];
}

static bool8 IsReservedCoordAfterMove(s8 x, s8 y, u8 movingArea, s8 movingX, s8 movingY)
{
    struct Coords8 coords = GetAreaCoordsAfterMove(HUB_AREA_ADVENTURE_ENTRANCE, movingArea, movingX, movingY);
    if(coords.x == x && coords.y + 1 == y)
        return TRUE;

    coords = GetAreaCoordsAfterMove(HUB_AREA_SAFARI_ZONE, movingArea, movingX, movingY);
    if(coords.x == x && coords.y + 1 == y)
        return TRUE;

    return FALSE;
}

static bool8 AnyAreaOccupiesReservedCoordAfterMove(u8 movingArea, s8 movingX, s8 movingY)
{
    u8 i;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(RogueHub_HasAreaBuilt(i))
        {
            struct Coords8 coords = GetAreaCoordsAfterMove(i, movingArea, movingX, movingY);
            if(IsReservedCoordAfterMove(coords.x, coords.y, movingArea, movingX, movingY))
                return TRUE;
        }
    }

    return FALSE;
}

static u8 GetDirectionBetweenCoords(struct Coords8 from, struct Coords8 to)
{
    if(from.x == to.x)
    {
        if(from.y - 1 == to.y)
            return HUB_AREA_CONN_SOUTH;
        if(from.y + 1 == to.y)
            return HUB_AREA_CONN_NORTH;
    }
    else if(from.y == to.y)
    {
        if(from.x - 1 == to.x)
            return HUB_AREA_CONN_WEST;
        if(from.x + 1 == to.x)
            return HUB_AREA_CONN_EAST;
    }

    return HUB_AREA_CONN_COUNT;
}

static bool8 CanAreasConnectAfterMove(u8 fromArea, u8 toArea, u8 movingArea, s8 movingX, s8 movingY)
{
    struct Coords8 fromCoords = GetAreaCoordsAfterMove(fromArea, movingArea, movingX, movingY);
    struct Coords8 toCoords = GetAreaCoordsAfterMove(toArea, movingArea, movingX, movingY);
    u8 dir = GetDirectionBetweenCoords(fromCoords, toCoords);

    if(dir == HUB_AREA_CONN_COUNT)
        return FALSE;

    return CanAreaConnect(fromArea, dir) && CanAreaConnect(toArea, InvertConnDirection(dir));
}

static bool8 AreaHasAnyConnectionAfterMove(u8 area, u8 movingArea, s8 movingX, s8 movingY)
{
    u8 i;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(i != area && RogueHub_HasAreaBuilt(i) && CanAreasConnectAfterMove(area, i, movingArea, movingX, movingY))
            return TRUE;
    }

    return FALSE;
}

static bool8 WouldHubRemainConnectedAfterMove(u8 movingArea, s8 movingX, s8 movingY)
{
    bool8 visited[HUB_AREA_COUNT] = {0};
    u8 queue[HUB_AREA_COUNT];
    u8 builtCount = 0;
    u8 visitedCount = 0;
    u8 read = 0;
    u8 write = 0;
    u8 startArea = HUB_AREA_NONE;
    u8 i;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(RogueHub_HasAreaBuilt(i))
        {
            ++builtCount;
            if(startArea == HUB_AREA_NONE)
                startArea = i;
        }
    }

    if(startArea == HUB_AREA_NONE)
        return FALSE;

    visited[startArea] = TRUE;
    queue[write++] = startArea;
    visitedCount = 1;

    while(read < write)
    {
        u8 fromArea = queue[read++];

        for(i = 0; i < HUB_AREA_COUNT; ++i)
        {
            if(!visited[i] && RogueHub_HasAreaBuilt(i) && CanAreasConnectAfterMove(fromArea, i, movingArea, movingX, movingY))
            {
                visited[i] = TRUE;
                queue[write++] = i;
                ++visitedCount;
            }
        }
    }

    return visitedCount == builtCount;
}

static bool8 WouldHubRemainConnectedWithoutArea(u8 removedArea)
{
    bool8 visited[HUB_AREA_COUNT] = {0};
    u8 queue[HUB_AREA_COUNT];
    u8 builtCount = 0;
    u8 visitedCount = 0;
    u8 read = 0;
    u8 write = 0;
    u8 startArea = HUB_AREA_NONE;
    u8 i;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(i != removedArea && RogueHub_HasAreaBuilt(i))
        {
            ++builtCount;
            if(startArea == HUB_AREA_NONE)
                startArea = i;
        }
    }

    if(builtCount <= 1)
        return TRUE;

    visited[startArea] = TRUE;
    queue[write++] = startArea;
    visitedCount = 1;

    while(read < write)
    {
        u8 fromArea = queue[read++];

        for(i = 0; i < HUB_AREA_COUNT; ++i)
        {
            if(i != removedArea && !visited[i] && RogueHub_HasAreaBuilt(i) && CanAreasConnectAfterMove(fromArea, i, HUB_AREA_NONE, 0, 0))
            {
                visited[i] = TRUE;
                queue[write++] = i;
                ++visitedCount;
            }
        }
    }

    return visitedCount == builtCount;
}

bool8 RogueHub_CanMoveArea(u8 area)
{
    if(area >= HUB_AREA_COUNT || !RogueHub_HasAreaBuilt(area))
        return FALSE;

    if(area == HUB_AREA_ADVENTURE_ENTRANCE)
        return FALSE;

    if(area == RogueHub_GetAreaFromCurrentMap())
        return FALSE;

    return WouldHubRemainConnectedWithoutArea(area);
}

bool8 RogueHub_CanBuildAreaAtCoord(u8 area, s8 x, s8 y)
{
    u8 i;

    if(area >= HUB_AREA_COUNT || RogueHub_HasAreaBuilt(area))
        return FALSE;

    if(!RogueHub_IsCoordInTownGrid(x, y))
        return FALSE;

    if(IsReservedCoordAfterMove(x, y, area, x, y))
        return FALSE;

    if(AnyAreaOccupiesReservedCoordAfterMove(area, x, y))
        return FALSE;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(RogueHub_HasAreaBuilt(i))
        {
            struct Coords8 coords = RogueHub_GetAreaCoords(i);
            if(coords.x == x && coords.y == y)
                return FALSE;
        }
    }

    return AreaHasAnyConnectionAfterMove(area, area, x, y);
}

bool8 RogueHub_CanMoveAreaToCoord(u8 area, s8 x, s8 y)
{
    struct Coords8 oldCoords;
    u8 i;

    if(!RogueHub_CanMoveArea(area))
        return FALSE;

    if(!RogueHub_IsCoordInTownGrid(x, y))
        return FALSE;

    oldCoords = RogueHub_GetAreaCoords(area);
    if(oldCoords.x == x && oldCoords.y == y)
        return FALSE;

    if(IsReservedCoordAfterMove(x, y, area, x, y))
        return FALSE;

    if(AnyAreaOccupiesReservedCoordAfterMove(area, x, y))
        return FALSE;

    for(i = 0; i < HUB_AREA_COUNT; ++i)
    {
        if(i != area && RogueHub_HasAreaBuilt(i))
        {
            struct Coords8 coords = RogueHub_GetAreaCoords(i);
            if(coords.x == x && coords.y == y)
                return FALSE;
        }
    }

    if(!AreaHasAnyConnectionAfterMove(area, area, x, y))
        return FALSE;

    return WouldHubRemainConnectedAfterMove(area, x, y);
}

void RogueHub_MoveAreaToCoord(u8 area, s8 x, s8 y)
{
    AGB_ASSERT(RogueHub_CanMoveAreaToCoord(area, x, y));
    GetActiveHubMap()->areaCoords[area].x = x;
    GetActiveHubMap()->areaCoords[area].y = y;
}

u8 RogueHub_GetAreaAtConnection(u8 area, u8 dir)
{
    if(CanAreaConnect(area, dir))
    {
        struct Coords8 pos;
        pos.x = GetActiveHubMap()->areaCoords[area].x;
        pos.y = GetActiveHubMap()->areaCoords[area].y;
        IncrementCoordsByDirection(&pos, dir);

        return RogueHub_FindAreaAtCoord(pos.x, pos.y);
    }

    return HUB_AREA_NONE;
}

static u8 GetAreaForLayout(u16 layout)
{
    u8 area;

    switch (layout)
    {
    case LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL:
        layout = LAYOUT_ROGUE_AREA_SAFARI_ZONE;
        break;
    }

    for(area = HUB_AREA_FIRST; area < HUB_AREA_COUNT; ++area)
    {
        if(gRogueHubAreas[area].primaryMapLayout == layout)
            return area;
    }

    return HUB_AREA_NONE;
}

u8 RogueHub_GetAreaFromCurrentMap()
{
    return GetAreaForLayout(gMapHeader.mapLayoutId);
}

u16 RogueHub_GetWeatherState()
{
    return GetActiveHubMap()->weatherState;
}

void RogueHub_OnNewDayStarted()
{
    //RogueHub_UpdateWeatherState
}

void RogueHub_UpdateWeatherState()
{
    // Ignore this if client
    if(RogueMP_IsActive() && !RogueMP_IsHost())
        return;

    gRogueSaveBlock->hubMap.weatherState = Random();
}

u8 const* RogueHub_GetHubName()
{
    if(RogueMP_IsActive() && !RogueMP_IsHost())
    {
        return RogueMP_GetPlayerHubName(RogueMP_GetRemotePlayerId());
    }

    return gSaveBlock2Ptr->pokemonHubName;
}

u8 RogueHub_GetHubVariantNumber()
{
    if(RogueMP_IsActive() && !RogueMP_IsHost())
    {
        return RogueMP_GetPlayerTrainerId(RogueMP_GetRemotePlayerId())[0];
    }

    return gSaveBlock2Ptr->playerTrainerId[0];
}

struct TrialAreaClosure
{
    u8 direction;
    u8 xStart, yStart, xEnd, yEnd;
    u16 metatile;
};

#include "data/trial_area_closures.h"

static u8 GetConnectionWarpDirection(const struct MapHeader *mapHeader, u8 warpId)
{
    u8 area = GetAreaForLayout(mapHeader->mapLayoutId);

    // The portal room adds upper side lanes and a third south lane.
    // Keep the original six connection indices and return index unchanged.
    if (area == HUB_AREA_ADVENTURE_ENTRANCE)
    {
        if (warpId == 7) warpId = 0;
        else if (warpId == 8) warpId = 4;
        else if (warpId == 9) warpId = 2;
    }

    if ((area == HUB_AREA_BERRY_FIELD || area == HUB_AREA_MARTS || area == HUB_AREA_DAY_CARE) && warpId >= 9 && warpId <= 12)
        warpId = (warpId - 9) * 2; // Appended third lanes, N/E/S/W.

    if (area == HUB_AREA_MARKET && warpId >= 10 && warpId <= 13)
        warpId = (warpId - 10) * 2;

    if (area == HUB_AREA_TRIAL_FRONTIER && warpId >= 11 && warpId <= 14)
        warpId = (warpId - 11) * 2; // Third N/E/S/W lanes.

    if (area == HUB_AREA_LABS)
    {
        if (warpId == 15) warpId = 6;
        else if (warpId == 16) warpId = 2;
        else if (warpId == 17) warpId = 0;
        else if (warpId == 18) warpId = 4;
    }

    if (area == HUB_AREA_TOWN_SQUARE && warpId >= 11 && warpId <= 14)
        warpId = (warpId - 11) * 2; // Third N/E/S/W lanes; keep original indices.

    if (area == HUB_AREA_SAFARI_ZONE)
    {
        if (mapHeader->mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL)
        {
            if (warpId == 6) warpId = 0;
            else if (warpId == 7) warpId = 4;
            else if (warpId == 8) warpId = 2;
        }
        else
        {
            if (warpId == 8) warpId = 0;
            else if (warpId == 9) warpId = 4;
            else if (warpId == 10) warpId = 2;
        }
    }

    if (area != HUB_AREA_NONE)
    {
        u8 dir;
        for (dir = HUB_AREA_CONN_SOUTH; dir <= HUB_AREA_CONN_EAST; ++dir)
            if (CanAreaConnect(area, dir)
                && (gRogueHubAreas[area].connectionWarps[dir][0] == warpId
                    || gRogueHubAreas[area].connectionWarps[dir][1] == warpId))
                return dir;
    }
    return HUB_AREA_CONN_COUNT;
}

bool8 RogueHub_GetWarpArrivalPosition(const struct MapHeader *mapHeader, u8 warpId, s16 *x, s16 *y)
{
    u8 i, dir;
    s16 minX, maxX, minY, maxY;
    switch (mapHeader->mapLayoutId)
    {
    case LAYOUT_ROGUE_AREA_ADVENTURE_ENTRANCE:
    case LAYOUT_ROGUE_AREA_TOWN_SQUARE:
    case LAYOUT_ROGUE_AREA_LABS:
    case LAYOUT_ROGUE_AREA_FARMING_FIELD:
    case LAYOUT_ROGUE_AREA_TRIAL_FRONTIER:
    case LAYOUT_ROGUE_AREA_MARKET:
    case LAYOUT_ROGUE_AREA_MARTS:
    case LAYOUT_ROGUE_AREA_DAY_CARE:
    case LAYOUT_ROGUE_AREA_SAFARI_ZONE:
    case LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL:
        break;
    default:
        return FALSE;
    }
    if (mapHeader->events == NULL || warpId >= mapHeader->events->warpCount)
        return FALSE;
    dir = GetConnectionWarpDirection(mapHeader, warpId);
    if (dir > HUB_AREA_CONN_EAST)
        return FALSE; // Interior doors, portal returns and teleport arrivals.
    minX = maxX = mapHeader->events->warps[warpId].x;
    minY = maxY = mapHeader->events->warps[warpId].y;
    for (i = 0; i < mapHeader->events->warpCount; ++i)
        if (GetConnectionWarpDirection(mapHeader, i) == dir)
        {
            const struct WarpEvent *lane = &mapHeader->events->warps[i];
            if (lane->x < minX) minX = lane->x;
            if (lane->x > maxX) maxX = lane->x;
            if (lane->y < minY) minY = lane->y;
            if (lane->y > maxY) maxY = lane->y;
        }
    *x = (minX + maxX) / 2;
    *y = (minY + maxY) / 2;
    return TRUE;
}

bool8 RogueHub_RecoverHallwayPosition(const struct MapHeader *mapHeader, s16 *x, s16 *y)
{
    u8 i;
    if (mapHeader->events == NULL)
        return FALSE;
    if (mapHeader->mapLayoutId == LAYOUT_ROGUE_AREA_TRIAL_FRONTIER)
    {
        for (i = 0; i < ARRAY_COUNT(sTrialAreaClosures); ++i)
        {
            const struct TrialAreaClosure *closure = &sTrialAreaClosures[i];
            if (*x >= closure->xStart && *x <= closure->xEnd
                && *y >= closure->yStart && *y <= closure->yEnd
                && RogueHub_GetAreaAtConnection(HUB_AREA_TRIAL_FRONTIER, closure->direction) == HUB_AREA_NONE)
            {
                const struct WarpEvent *arrival = &mapHeader->events->warps[10];
                *x = arrival->x;
                *y = arrival->y;
                return TRUE;
            }
        }
        return FALSE;
    }
    // Old saves may be standing beyond a threshold that has moved inward.
    for (i = 0; i < mapHeader->events->warpCount; ++i)
    {
        s16 centerX, centerY;
        u8 dir;
        if (!RogueHub_GetWarpArrivalPosition(mapHeader, i, &centerX, &centerY))
            continue;
        dir = GetConnectionWarpDirection(mapHeader, i);
        if ((dir == HUB_AREA_CONN_WEST && *x < centerX && *y >= centerY - 1 && *y <= centerY + 1)
            || (dir == HUB_AREA_CONN_EAST && *x > centerX && *y >= centerY - 1 && *y <= centerY + 1)
            || (dir == HUB_AREA_CONN_NORTH && *y < centerY && *x >= centerX - 1 && *x <= centerX + 1)
            || (dir == HUB_AREA_CONN_SOUTH && *y > centerY && *x >= centerX - 1 && *x <= centerX + 1))
        {
            *x = centerX;
            *y = centerY;
            return TRUE;
        }
    }
    return FALSE;
}

void RogueHub_ModifyMapWarpEvent(struct MapHeader *mapHeader, u8 warpId, struct WarpEvent *warp)
{
    u8 area = GetAreaForLayout(mapHeader->mapLayoutId);

    if(area != HUB_AREA_NONE)
    {
        u8 dir = GetConnectionWarpDirection(mapHeader, warpId);
        u8 warpArea = HUB_AREA_NONE;
        u8 enterDir = HUB_AREA_CONN_SOUTH;

        // We're trying to warp out into a valid direction
        if(dir <= HUB_AREA_CONN_EAST)
        {
            warpArea = RogueHub_GetAreaAtConnection(area, dir);
            enterDir = InvertConnDirection(dir);
        }

        // Warping into a valid area, so setup a valid warp
        if(warpArea != HUB_AREA_NONE)
        {
            warp->mapGroup = gRogueHubAreas[warpArea].primaryMapGroup;
            warp->mapNum = gRogueHubAreas[warpArea].primaryMapNum;
            warp->warpId = gRogueHubAreas[warpArea].connectionWarps[enterDir][0];

            if(gRogueHubAreas[warpArea].primaryMapLayout == LAYOUT_ROGUE_AREA_SAFARI_ZONE && VarGet(VAR_ROGUE_INTRO_STATE) == ROGUE_INTRO_STATE_CATCH_MON)
            {
                warp->mapGroup = MAP_GROUP(ROGUE_AREA_SAFARI_ZONE_TUTORIAL);
                warp->mapNum = MAP_NUM(ROGUE_AREA_SAFARI_ZONE_TUTORIAL);
            }
        }
        else
        {
            // TODO - We should do safe warp back to main area if we get here
        }
    }
}

bool8 RogueHub_AcceptMapConnection(struct MapHeader *mapHeader, const struct MapConnection *connection)
{
    // Convert from CONNECTION_ to HUB_AREA_CONN_
    u8 area = GetAreaForLayout(mapHeader->mapLayoutId);

    // Park facade maps are permanent terrain. Missing links are sealed by the
    // rock closure patches; hiding the facade would cut the cliffs off at trees.
    if (area == HUB_AREA_RIDE_TRAINING
        && connection->mapGroup == MAP_GROUP(ROGUE_PARK_TUNNEL_NORTH)
        && (connection->mapNum == MAP_NUM(ROGUE_PARK_TUNNEL_NORTH)
            || connection->mapNum == MAP_NUM(ROGUE_PARK_TUNNEL_EAST)
            || connection->mapNum == MAP_NUM(ROGUE_PARK_TUNNEL_SOUTH)
            || connection->mapNum == MAP_NUM(ROGUE_PARK_TUNNEL_WEST)))
        return TRUE;

    if(area != HUB_AREA_NONE)
    {
        u8 connDir = connection->direction - 1;
        
        // Hide any connections, unless we actually have the connection built
        return RogueHub_GetAreaAtConnection(area, connDir) != HUB_AREA_NONE;
    }

    return TRUE;
}

void RogueHub_ApplyMapMetatiles()
{
    bool8 applyCommonFixup = TRUE;

    switch (gMapHeader.mapLayoutId)
    {
    case LAYOUT_ROGUE_AREA_LABS:
        applyCommonFixup = FALSE;
        RogueHub_UpdateLabsAreaMetatiles();
        break;
    case LAYOUT_ROGUE_AREA_ADVENTURE_ENTRANCE:
        applyCommonFixup = FALSE;
        RogueHub_UpdateAdventureEntranceAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_FARMING_FIELD:
        RogueHub_UpdateFarmingAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_SAFARI_ZONE:
    case LAYOUT_ROGUE_AREA_SAFARI_ZONE_TUTORIAL:
        applyCommonFixup = FALSE;
        RogueHub_UpdateSafariAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_RIDE_TRAINING:
        RogueHub_UpdateRideTrainingAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_MARTS:
        RogueHub_UpdateMartsAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_TOWN_SQUARE:
        applyCommonFixup = FALSE;
        RogueHub_UpdateTownSquareAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_MARKET:
        applyCommonFixup = FALSE;
        RogueHub_UpdateMarketAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_TRIAL_FRONTIER:
        applyCommonFixup = FALSE;
        RogueHub_UpdateTrialFrontierAreaMetatiles();
        break;

    case LAYOUT_ROGUE_AREA_DAY_CARE:
        applyCommonFixup = FALSE;
        RogueHub_UpdateDayCareAreaMetatiles();
        break;
    
    default:
        break;
    }

    if(applyCommonFixup)
        FixupGrassPaths();
}

static void UpdateStatueLevel()
{
    if(Rogue_Use200PercEffects())
        gRogueSaveBlock->hubMap.statueLevel = 3;
    else if(Rogue_Use100PercEffects())
        gRogueSaveBlock->hubMap.statueLevel = 1;
    else if(RogueQuest_HasCollectedRewards(QUEST_ID_CHAMPION))
        gRogueSaveBlock->hubMap.statueLevel = 1;
    else
        gRogueSaveBlock->hubMap.statueLevel = 0;
}

void RogueHub_UpdateWarpStates()
{
    UpdateStatueLevel();
}

u8 RogueHub_GetStatueLevel()
{
    UpdateStatueLevel();
    return GetActiveHubMap()->statueLevel;
}

void RogueHub_CheckUniqueLabOpen(void)
{
    gSpecialVar_Result = RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_LAB);
}

static void SetLabJunctionDoor(s16 x, s16 y)
{
    MetatileSet_Tile(x - 1, y - 1, METATILE_LabJunction_FrameTopLeft | MAPGRID_COLLISION_MASK);
    MetatileSet_Tile(x, y - 1, METATILE_LabJunction_FrameTop | MAPGRID_COLLISION_MASK);
    MetatileSet_Tile(x + 1, y - 1, METATILE_LabJunction_FrameTopRight | MAPGRID_COLLISION_MASK);
    MetatileSet_Tile(x - 1, y, METATILE_LabJunction_FrameLeft | MAPGRID_COLLISION_MASK);
    MetatileSet_Tile(x, y, METATILE_LabJunction_Door);
    MetatileSet_Tile(x + 1, y, METATILE_LabJunction_FrameRight | MAPGRID_COLLISION_MASK);
}

static void RogueHub_UpdateLabsAreaMetatiles()
{
    if (RogueHub_GetAreaAtConnection(HUB_AREA_LABS, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(12, 0, 16, 2, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(12, 3, 16, 3, METATILE_LabJunction_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(12, 4, 16, 4, METATILE_LabJunction_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(12, 5, 16, 5, METATILE_LabJunction_FloorShadow);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_LABS, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(25, 6, 27, 13, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(24, 5, 24, 11, METATILE_LabJunction_Pillar | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_LABS, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileSet_Tile(12, 15, METATILE_LabJunction_Floor);
        MetatileSet_Tile(16, 15, METATILE_LabJunction_Floor);
        MetatileFill_Tile(12, 16, 16, 23, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_LABS, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 6, 3, 13, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(4, 5, 4, 11, METATILE_LabJunction_Pillar | MAPGRID_COLLISION_MASK);
    }
    // Hidden wings are blank structural walls until their existing gate opens.
    if (RogueHub_HasUpgrade(HUB_UPGRADE_LAB_UNIQUE_MON_LAB))
    {
        SetLabJunctionDoor(8, 13);
        MetatileSet_Tile(10, 12, METATILE_HubFurnishings_DoorSignTop | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(10, 13, METATILE_HubFurnishings_DoorSignBase | MAPGRID_COLLISION_MASK);
    }
#ifdef ROGUE_DEBUG
    SetLabJunctionDoor(20, 13);
#endif
}

static void RogueHub_UpdateAdventureEntranceAreaMetatiles()
{
    // Close the authored indoor edges using the shared hub tileset aliases.
    if(RogueHub_GetAreaAtConnection(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(17, 8, 19, 15, METATILE_PortalRoom_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(16, 8, 16, 11, METATILE_PortalRoom_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(16, 12, METATILE_PortalRoom_PillarEnd | MAPGRID_COLLISION_MASK);
    }
    if(RogueHub_GetAreaAtConnection(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileSet_Tile(7, 12, METATILE_PortalRoom_Floor);
        MetatileSet_Tile(11, 12, METATILE_PortalRoom_Floor);
        MetatileFill_Tile(7, 13, 11, 15, METATILE_PortalRoom_Void | MAPGRID_COLLISION_MASK);
    }
    if(RogueHub_GetAreaAtConnection(HUB_AREA_ADVENTURE_ENTRANCE, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 8, 1, 15, METATILE_PortalRoom_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(2, 8, 2, 11, METATILE_PortalRoom_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(2, 12, METATILE_PortalRoom_PillarEnd | MAPGRID_COLLISION_MASK);
    }
}

static void RogueHub_UpdateFarmingAreaMetatiles()
{
    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(12, 0, 16, 2, METATILE_BerryLab_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(12, 3, 16, 3, METATILE_BerryLab_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(12, 4, 16, 4, METATILE_BerryLab_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(12, 5, 16, 5, METATILE_BerryLab_FloorShadow);
    }
    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(25, 8, 28, 12, METATILE_BerryLab_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(24, 8, 24, 14, METATILE_BerryLab_Pillar | MAPGRID_COLLISION_MASK);
    }
    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileSet_Tile(12, 24, METATILE_BerryLab_Floor);
        MetatileSet_Tile(16, 24, METATILE_BerryLab_Floor);
        MetatileFill_Tile(12, 25, 16, 27, METATILE_BerryLab_Void | MAPGRID_COLLISION_MASK);
    }
    if(RogueHub_GetAreaAtConnection(HUB_AREA_BERRY_FIELD, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 8, 3, 12, METATILE_BerryLab_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(4, 8, 4, 14, METATILE_BerryLab_Pillar | MAPGRID_COLLISION_MASK);
    }
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD0))
    {
        MetatileFill_Tile(17, 7, 21, 7, METATILE_BerryLab_InactiveBed);
    }
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD1))
    {
        MetatileFill_Tile(17, 22, 21, 22, METATILE_BerryLab_InactiveBed);
    }
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_BERRY_FIELD_EXTRA_FIELD2))
    {
        MetatileFill_Tile(7, 22, 11, 22, METATILE_BerryLab_InactiveBed);
    }
}

static void RogueHub_UpdateSafariAreaMetatiles()
{
    if (RogueHub_GetAreaAtConnection(HUB_AREA_SAFARI_ZONE, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 12, 5, 19, METATILE_SafariLab_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(6, 11, 6, 17, METATILE_SafariLab_Pillar | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_SAFARI_ZONE, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(31, 12, 37, 19, METATILE_SafariLab_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(30, 11, 30, 17, METATILE_SafariLab_Pillar | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_SAFARI_ZONE, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        // Remove the pillar caps on the last floor row and seal the corridor flush.
        MetatileSet_Tile(16, 21, METATILE_SafariLab_Floor);
        MetatileSet_Tile(20, 21, METATILE_SafariLab_Floor);
        MetatileFill_Tile(16, 22, 20, 31, METATILE_SafariLab_Void | MAPGRID_COLLISION_MASK);
    }
    if (gMapHeader.mapLayoutId == LAYOUT_ROGUE_AREA_SAFARI_ZONE
        && RogueHub_HasUpgrade(HUB_UPGRADE_SAFARI_ZONE_LEGENDS_CAVE))
    {
        MetatileSet_Tile(18, 4, METATILE_SafariLab_CaveTop | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(18, 5, METATILE_SafariLab_CaveDoor);
    }
}

static void ApplyParkTunnelClosedPatch(const struct ParkTunnelPatch *patch, u32 count)
{
    u32 i;
    for (i = 0; i < count; ++i)
        MapGridSetMetatileIdAt(patch[i].x + MAP_OFFSET, patch[i].y + MAP_OFFSET, patch[i].tile);
}

static void RogueHub_UpdateRideTrainingAreaMetatiles()
{
    // Disconnected entrances become continuous rock, including their visible border.
    if (RogueHub_GetAreaAtConnection(HUB_AREA_RIDE_TRAINING, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
        ApplyParkTunnelClosedPatch(sParkTunnelClosedNorth, ARRAY_COUNT(sParkTunnelClosedNorth));
    if (RogueHub_GetAreaAtConnection(HUB_AREA_RIDE_TRAINING, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
        ApplyParkTunnelClosedPatch(sParkTunnelClosedEast, ARRAY_COUNT(sParkTunnelClosedEast));
    if (RogueHub_GetAreaAtConnection(HUB_AREA_RIDE_TRAINING, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
        ApplyParkTunnelClosedPatch(sParkTunnelClosedSouth, ARRAY_COUNT(sParkTunnelClosedSouth));
    if (RogueHub_GetAreaAtConnection(HUB_AREA_RIDE_TRAINING, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
        ApplyParkTunnelClosedPatch(sParkTunnelClosedWest, ARRAY_COUNT(sParkTunnelClosedWest));
}

static void RogueHub_UpdateMartsAreaMetatiles()
{
    if(RogueHub_GetAreaAtConnection(HUB_AREA_MARTS, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(16, 0, 20, 2, METATILE_SupplyDepot_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 3, 19, 3, METATILE_SupplyDepot_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 4, 19, 4, METATILE_SupplyDepot_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 5, 19, 5, METATILE_SupplyDepot_FloorShadow);
        MetatileSet_Tile(16, 2, METATILE_SupplyDepot_PillarCap | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(20, 2, METATILE_SupplyDepot_PillarCap | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(16, 5, METATILE_SupplyDepot_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(20, 5, METATILE_SupplyDepot_Pillar | MAPGRID_COLLISION_MASK);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_MARTS, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(33, 12, 36, 19, METATILE_SupplyDepot_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(32, 12, 32, 18, METATILE_SupplyDepot_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(32, 12, METATILE_SupplyDepot_PillarJoin | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(32, 18, METATILE_SupplyDepot_PillarJoin | MAPGRID_COLLISION_MASK);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_MARTS, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(17, 27, 19, 27, METATILE_SupplyDepot_Floor);
        MetatileFill_Tile(16, 28, 20, 30, METATILE_SupplyDepot_Void | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(16, 27, METATILE_SupplyDepot_PillarEnd | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(20, 27, METATILE_SupplyDepot_PillarEnd | MAPGRID_COLLISION_MASK);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_MARTS, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 12, 3, 19, METATILE_SupplyDepot_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(4, 12, 4, 18, METATILE_SupplyDepot_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(4, 12, METATILE_SupplyDepot_PillarJoin | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(4, 18, METATILE_SupplyDepot_PillarJoin | MAPGRID_COLLISION_MASK);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TRAVELER_BATTLE_ENCHANCERS))
    {
        MetatileFill_Tile(25, 25, 26, 26, METATILE_SupplyDepot_Floor);
        MetatileFill_Tile(23, 21, 24, 21, METATILE_SupplyDepot_Floor);
        MetatileSet_Tile(23, 23, METATILE_SupplyDepot_CounterLeft | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(24, 23, METATILE_SupplyDepot_CounterRight | MAPGRID_COLLISION_MASK);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TRAVELER_HELD_ITEMS))
    {
        MetatileFill_Tile(29, 25, 30, 26, METATILE_SupplyDepot_Floor);
        MetatileFill_Tile(28, 21, 29, 21, METATILE_SupplyDepot_Floor);
        MetatileSet_Tile(28, 23, METATILE_SupplyDepot_CounterLeft | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(29, 23, METATILE_SupplyDepot_CounterRight | MAPGRID_COLLISION_MASK);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_GENERAL_STOCK))
    {
        MetatileFill_Tile(6, 24, 7, 26, METATILE_SupplyDepot_Floor);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_POKE_BALLS_STOCK))
    {
        MetatileFill_Tile(13, 9, 14, 11, METATILE_SupplyDepot_Floor);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TMS_STOCK))
    {
        MetatileFill_Tile(29, 9, 30, 11, METATILE_SupplyDepot_Floor);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_POKE_BALLS))
    {
        MetatileFill_Tile(5, 5, 15, 11, METATILE_SupplyDepot_FloorDim40);
        MetatileFill_Tile(8, 4, 9, 4, METATILE_SupplyDepot_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(8, 12, 12, 12, METATILE_SupplyDepot_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(8, 13, 12, 13, METATILE_SupplyDepot_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(8, 14, 12, 14, METATILE_SupplyDepot_FloorShadow);
        MetatileSet_Tile(12, 5, METATILE_SupplyDepot_PlantBaseShadow | MAPGRID_COLLISION_MASK);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TMS))
    {
        MetatileFill_Tile(21, 5, 31, 11, METATILE_SupplyDepot_FloorDim40);
        MetatileFill_Tile(24, 4, 25, 4, METATILE_SupplyDepot_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(24, 12, 28, 12, METATILE_SupplyDepot_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(24, 13, 28, 13, METATILE_SupplyDepot_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(24, 14, 28, 14, METATILE_SupplyDepot_FloorShadow);
        MetatileSet_Tile(28, 5, METATILE_SupplyDepot_PlantBaseShadow | MAPGRID_COLLISION_MASK);
    }

    if(!RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TRAVELER_BATTLE_ENCHANCERS) && !RogueHub_HasUpgrade(HUB_UPGRADE_MARTS_TRAVELER_HELD_ITEMS))
    {
        MetatileFill_Tile(21, 20, 31, 27, METATILE_SupplyDepot_FloorDim40);
        MetatileSet_Tile(24, 17, METATILE_SupplyDepot_Floor);
        MetatileSet_Tile(28, 17, METATILE_SupplyDepot_Floor);
        MetatileFill_Tile(24, 18, 28, 18, METATILE_SupplyDepot_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(24, 19, 28, 19, METATILE_SupplyDepot_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(21, 20, 31, 20, METATILE_SupplyDepot_FloorShadowDim40);
    }
}

static void RogueHub_UpdateTownSquareAreaMetatiles()
{
    if (RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(16, 0, 20, 2, METATILE_MainHall_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(16, 3, 20, 3, METATILE_MainHall_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(16, 4, 20, 4, METATILE_MainHall_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(16, 5, 20, 5, METATILE_MainHall_FloorShadow);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(34, 14, 35, 21, METATILE_MainHall_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(33, 14, 33, 19, METATILE_MainHall_Pillar | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(16, 25, 20, 29, METATILE_MainHall_Void | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_TOWN_SQUARE, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 14, 2, 21, METATILE_MainHall_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(3, 14, 3, 19, METATILE_MainHall_Pillar | MAPGRID_COLLISION_MASK);
    }
    if (!RogueHub_HasUpgrade(HUB_UPGRADE_TOWN_SQUARE_SCHOOL))
        MetatileSet_Tile(9, 4, METATILE_MainHall_ClosedDoor | MAPGRID_COLLISION_MASK);
}

static void RogueHub_UpdateMarketAreaMetatiles()
{
    if (RogueHub_GetAreaAtConnection(HUB_AREA_MARKET, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(17, 0, 19, 2, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 3, 19, 3, METATILE_LabJunction_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 4, 19, 4, METATILE_LabJunction_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 5, 19, 5, METATILE_LabJunction_FloorShadow);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_MARKET, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(34, 13, 36, 16, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(33, 14, 33, 16, METATILE_LabJunction_Pillar | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_MARKET, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(17, 27, 19, 27, METATILE_LabJunction_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 28, 19, 30, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
    }
    if (RogueHub_GetAreaAtConnection(HUB_AREA_MARKET, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 13, 2, 16, METATILE_LabJunction_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(3, 14, 3, 16, METATILE_LabJunction_Pillar | MAPGRID_COLLISION_MASK);
    }

    if (!RogueHub_HasUpgrade(HUB_UPGRADE_MARKET_BANK))
        MetatileSet_Tile(26, 8, METATILE_HubArchitecture_Market_Counter_0_0 | MAPGRID_COLLISION_MASK);
    if (!RogueHub_HasUpgrade(HUB_UPGRADE_MARKET_TREAT_SHOP))
        MetatileSet_Tile(10, 21, METATILE_HubArchitecture_Market_Counter_0_0 | MAPGRID_COLLISION_MASK);
    if (!RogueHub_HasUpgrade(HUB_UPGRADE_MARKET_BAKERY))
    {
        MetatileSet_Tile(25, 19, METATILE_HubArchitecture_Market_KitchenCounter_0_0 | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(26, 19, METATILE_HubArchitecture_Market_PrepCounterMiddle | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(30, 19, METATILE_HubArchitecture_Market_PrepCounterMiddle | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(31, 19, METATILE_HubArchitecture_Market_KitchenCounter_1_0 | MAPGRID_COLLISION_MASK);
    }
}

static void RogueHub_UpdateTrialFrontierAreaMetatiles()
{
    u8 i;
    for (i = 0; i < ARRAY_COUNT(sTrialAreaClosures); ++i)
    {
        const struct TrialAreaClosure *closure = &sTrialAreaClosures[i];
        if (RogueHub_GetAreaAtConnection(HUB_AREA_TRIAL_FRONTIER, closure->direction) == HUB_AREA_NONE)
            MetatileFill_Tile(closure->xStart, closure->yStart, closure->xEnd, closure->yEnd,
                closure->metatile | MAPGRID_COLLISION_MASK);
    }
}

static void RogueHub_UpdateDayCareAreaMetatiles()
{
    if(RogueHub_GetAreaAtConnection(HUB_AREA_DAY_CARE, HUB_AREA_CONN_NORTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(16, 0, 20, 2, METATILE_Nursery_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 3, 19, 3, METATILE_Nursery_Wall | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 4, 19, 4, METATILE_Nursery_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(17, 5, 19, 5, METATILE_Nursery_FloorShadow);
        MetatileSet_Tile(16, 2, METATILE_Nursery_PillarCap | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(20, 2, METATILE_Nursery_PillarCap | MAPGRID_COLLISION_MASK);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_DAY_CARE, HUB_AREA_CONN_EAST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(33, 11, 36, 18, METATILE_Nursery_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(32, 11, 32, 15, METATILE_Nursery_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(32, 11, METATILE_Nursery_PillarJoin | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(32, 15, METATILE_Nursery_PillarEnd | MAPGRID_COLLISION_MASK);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_DAY_CARE, HUB_AREA_CONN_WEST) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(0, 11, 3, 18, METATILE_Nursery_Void | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(4, 11, 4, 17, METATILE_Nursery_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(4, 11, METATILE_Nursery_PillarJoin | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(4, 17, METATILE_Nursery_PillarJoin | MAPGRID_COLLISION_MASK);
    }

    if(RogueHub_GetAreaAtConnection(HUB_AREA_DAY_CARE, HUB_AREA_CONN_SOUTH) == HUB_AREA_NONE)
    {
        MetatileFill_Tile(16, 26, 20, 28, METATILE_Nursery_Void | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(16, 25, METATILE_Nursery_PillarEnd | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(20, 25, METATILE_Nursery_PillarEnd | MAPGRID_COLLISION_MASK);
    }

    if(Rogue_GetCurrentDaycareSlotCount() < 2)
    {
        MetatileFill_Tile(10, 6, 11, 6, METATILE_Nursery_CoffeeMiddleTopDim | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(12, 6, METATILE_Nursery_CoffeeRightTopDim | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(10, 7, 11, 7, METATILE_Nursery_CoffeeMiddleSeatDim | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(12, 7, METATILE_Nursery_CoffeeRightSeatDim | MAPGRID_COLLISION_MASK);
    }
    if(Rogue_GetCurrentDaycareSlotCount() < 3)
    {
        MetatileSet_Tile(12, 9, METATILE_Nursery_CoffeeSideTopDim | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(12, 10, METATILE_Nursery_CoffeeSideMiddleDim | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(12, 11, METATILE_Nursery_CoffeeSideSeatDim | MAPGRID_COLLISION_MASK);
    }
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_BREEDER))
    {
        MetatileFill_Tile(23, 5, 31, 10, METATILE_Nursery_FloorDim40);
        MetatileFill_Tile(23, 5, 31, 5, METATILE_Nursery_FloorShadowDim40);
        MetatileFill_Tile(23, 6, 23, 10, METATILE_Nursery_FloorShadowDim40);
        MetatileFill_Tile(23, 4, 31, 4, METATILE_Nursery_WallBase | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(22, 5, 22, 12, METATILE_Nursery_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(22, 11, METATILE_Nursery_PillarJoin | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(22, 12, METATILE_Nursery_InnerBase | MAPGRID_COLLISION_MASK);
    }
    if(!RogueHub_HasUpgrade(HUB_UPGRADE_DAY_CARE_TEA_SHOP))
    {
        MetatileFill_Tile(5, 19, 11, 25, METATILE_Nursery_FloorDim40);
        MetatileFill_Tile(12, 19, 12, 25, METATILE_Nursery_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(12, 18, METATILE_Nursery_Pillar | MAPGRID_COLLISION_MASK);
        MetatileSet_Tile(12, 25, METATILE_Nursery_PillarEnd | MAPGRID_COLLISION_MASK);
        MetatileFill_Tile(5, 19, 11, 19, METATILE_Nursery_FloorShadowDim40);
        MetatileFill_Tile(5, 20, 5, 25, METATILE_Nursery_FloorShadowDim40);
    }
}

// Metatile util functions
//

static void MetatileSet_Tile(u16 x, u16 y, u16 tile)
{
    MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, tile);
}

static void UNUSED MetatileFill_Tile(u16 xStart, u16 yStart, u16 xEnd, u16 yEnd, u16 tile)
{
    u16 x, y;

    for(x = xStart; x <= xEnd; ++x)
    {
        for(y = yStart; y <= yEnd; ++y)
        {
            MapGridSetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET, tile);
        }
    }
}

static void BuildAtRandomConnectionFrom(u8 fromArea, u8 buildArea)
{
    while (TRUE)
    {
        u8 dir = Random() % HUB_AREA_CONN_COUNT;
        u8 invDir = InvertConnDirection(dir);

        if(RogueHub_AreaHasFreeConnection(fromArea, dir) && CanAreaConnect(buildArea, invDir))
        {
            struct Coords8 pos;
            pos.x = GetActiveHubMap()->areaCoords[fromArea].x;
            pos.y = GetActiveHubMap()->areaCoords[fromArea].y;
            IncrementCoordsByDirection(&pos, dir);

            // We cannot place the lab next to the safari as it will bork up the tutorial sequence
            if(buildArea == HUB_AREA_TOWN_SQUARE)
            {
                if(
                    RogueHub_FindAreaAtCoord(pos.x + 1, pos.y) == HUB_AREA_SAFARI_ZONE ||
                    RogueHub_FindAreaAtCoord(pos.x - 1, pos.y) == HUB_AREA_SAFARI_ZONE ||
                    RogueHub_FindAreaAtCoord(pos.x, pos.y + 1) == HUB_AREA_SAFARI_ZONE ||
                    RogueHub_FindAreaAtCoord(pos.x, pos.y - 1) == HUB_AREA_SAFARI_ZONE
                )
                    continue;
            }

            RogueHub_BuildArea(buildArea, pos.x, pos.y);
            break;
        }
    }
}

static u32 GetCurrentAreaMetatileAt(s32 x, s32 y)
{
    return MapGridGetMetatileIdAt(x + MAP_OFFSET, y + MAP_OFFSET);
}

static bool8 IsGrassPathMetatile(u32 checkTile)
{
    return (
        checkTile == METATILE_GeneralHub_GrassPath_Centre ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_EastWest_North ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_EastWest_South ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_NorthEast ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_NorthSouth_East ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_NorthSouth_West ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_NorthWest ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_SouthEast ||
        checkTile == METATILE_GeneralHub_GrassPath_Conn_SouthWest
    );
}

static void FixupTile_GrassPath_Horizontal(s32 x, s32 y)
{
    bool8 west = IsGrassPathMetatile(GetCurrentAreaMetatileAt(x - 1, y + 0));
    bool8 east = IsGrassPathMetatile(GetCurrentAreaMetatileAt(x + 1, y + 0));

    if(!west && east)
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_NorthSouth_East);
    }
    else if(west && !east)
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_NorthSouth_West);
    }
}

static void FixupTile_GrassPath_Vertical(s32 x, s32 y)
{
    bool8 north = IsGrassPathMetatile(GetCurrentAreaMetatileAt(x + 0, y - 1));
    bool8 south = IsGrassPathMetatile(GetCurrentAreaMetatileAt(x + 0, y + 1));

    if(!north && south)
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_EastWest_South);
    }
    else if(north && !south)
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_EastWest_North);
    }
}

static void FixupTile_GrassPath_Fixup(s32 x, s32 y)
{
    u32 northTile = GetCurrentAreaMetatileAt(x + 0, y - 1);
    u32 eastTile =  GetCurrentAreaMetatileAt(x + 1, y + 0);
    u32 southTile = GetCurrentAreaMetatileAt(x + 0, y + 1);
    u32 westTile =  GetCurrentAreaMetatileAt(x - 1, y + 0);

    // Outside Corners (North)
    if(!IsGrassPathMetatile(northTile) && !IsGrassPathMetatile(westTile))
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_SouthEast);
    }
    else if(!IsGrassPathMetatile(northTile) && !IsGrassPathMetatile(eastTile))
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_SouthWest);
    }
    // Outside Corners (South)
    if(!IsGrassPathMetatile(southTile) && !IsGrassPathMetatile(westTile))
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_NorthEast);
    }
    else if(!IsGrassPathMetatile(southTile) && !IsGrassPathMetatile(eastTile))
    {
        MetatileSet_Tile(x, y, METATILE_GeneralHub_GrassPath_Conn_NorthWest);
    }
}

static void FixupGrassPaths(void)
{
    s32 x, y;
    s32 toX = gMapHeader.mapLayout->width - 2;
    s32 toY = gMapHeader.mapLayout->height - 1;

    for(y = 2; y <= toY; ++y)
    {
        for(x = 2; x <= toX; ++x)
        {
            if(GetCurrentAreaMetatileAt(x, y) == METATILE_GeneralHub_GrassPath_Centre)
                FixupTile_GrassPath_Horizontal(x, y);
        }
    }

    for(x = 2; x <= toX; ++x)
    {
        for(y = 2; y <= toY; ++y)
        {
            u16 metatileId = GetCurrentAreaMetatileAt(x, y);
            if(metatileId == METATILE_GeneralHub_GrassPath_Centre)
                FixupTile_GrassPath_Vertical(x, y);

            if(IsGrassPathMetatile(metatileId))
                FixupTile_GrassPath_Fixup(x, y);
        }
    }
}

void RogueHub_ReloadObjectsAndTiles()
{
    if(GetAreaForLayout(gMapHeader.mapLayoutId) != HUB_AREA_NONE)
    {
        RogueHub_UpdateWarpStates();
        RogueHub_ApplyMapMetatiles();
        DrawWholeMapView();
    }
}
