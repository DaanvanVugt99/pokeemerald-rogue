#include "global.h"

#include "constants/event_object_movement.h"
#include "constants/event_objects.h"
#include "constants/items.h"
#include "constants/rogue_route_events.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "item.h"
#include "overworld.h"
#include "random.h"
#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_route_events.h"
#include "test/test.h"

extern const u8 Rogue_RouteEvent_Interact[];

static bool8 IsDeliveryReward(u16 itemId)
{
    return itemId == ITEM_NUGGET || itemId == ITEM_RARE_CANDY || itemId == ITEM_PP_UP;
}

TEST("Route events generate deterministic RNG-neutral delivery descriptors")
{
    struct RogueAdvPathRoom roomA = {0};
    struct RogueAdvPathRoom roomB = {0};
    RAND_TYPE originalRng = gRngRogueValue;
    RAND_TYPE rngBefore;

    roomA.rngSeed = 24680;
    roomB.rngSeed = roomA.rngSeed;
    SeedRogueRng(13579);
    rngBefore = gRngRogueValue;

    RogueRouteEvents_GenerateRoom(&roomA);
    EXPECT_EQ(memcmp(&gRngRogueValue, &rngBefore, sizeof(rngBefore)), 0);

    SeedRogueRng(31995);
    RogueRouteEvents_GenerateRoom(&roomB);
    EXPECT_EQ(memcmp(&roomA.routeEvent, &roomB.routeEvent, sizeof(roomA.routeEvent)), 0);
    EXPECT_EQ(roomA.routeEvent.eventType, ROGUE_ROUTE_EVENT_DELIVERY);
    EXPECT(IsDeliveryReward(roomA.routeEvent.rewardItem));
    EXPECT_NE(roomA.routeEvent.senderGraphicsId, roomA.routeEvent.recipientGraphicsId);

    gRngRogueValue = originalRng;
}

TEST("Route events reserve two clear three-by-three anchors on every active route")
{
    u8 routeIdx;

    for(routeIdx = 0; routeIdx < gRogueRouteTable.routeCount; ++routeIdx)
    {
        const struct RogueRouteMap *routeMap = &gRogueRouteTable.routes[routeIdx].map;
        const struct MapHeader *mapHeader = Overworld_GetMapHeaderByGroupAndId(routeMap->group, routeMap->num);
        const struct MapLayout *mapLayout = mapHeader->mapLayout;
        const struct MapEvents *events = mapHeader->events;
        const struct ObjectEventTemplate *markers[ROGUE_ROUTE_EVENT_ANCHOR_COUNT] = {0};
        u8 markerCount = 0;
        u8 objectIdx;

        EXPECT_LT(events->objectEventCount, OBJECT_EVENT_TEMPLATES_COUNT);

        for(objectIdx = 0; objectIdx < events->objectEventCount; ++objectIdx)
        {
            const struct ObjectEventTemplate *object = &events->objectEvents[objectIdx];

            if(RogueRouteEvents_IsMarkerTemplate(object))
            {
                u16 role = object->trainerRange_berryTreeId;

                EXPECT_LT(role, ROGUE_ROUTE_EVENT_ANCHOR_COUNT);
                EXPECT_EQ(markers[role], NULL);
                markers[role] = object;
                ++markerCount;
            }
        }

        EXPECT_EQ(markerCount, ROGUE_ROUTE_EVENT_ANCHOR_COUNT);
        EXPECT_NE(markers[ROGUE_ROUTE_EVENT_ANCHOR_SENDER]->localId, markers[ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT]->localId);

        for(objectIdx = 0; objectIdx < ROGUE_ROUTE_EVENT_ANCHOR_COUNT; ++objectIdx)
        {
            const struct ObjectEventTemplate *marker = markers[objectIdx];
            s16 x;
            s16 y;
            u8 otherIdx;
            u8 warpIdx;

            EXPECT_GT(marker->x, 0);
            EXPECT_GT(marker->y, 0);
            EXPECT_LT(marker->x, mapLayout->width - 1);
            EXPECT_LT(marker->y, mapLayout->height - 1);

            for(y = marker->y - 1; y <= marker->y + 1; ++y)
            {
                for(x = marker->x - 1; x <= marker->x + 1; ++x)
                {
                    u16 block = mapLayout->map[y * mapLayout->width + x];

                    EXPECT_EQ(block & MAPGRID_COLLISION_MASK, 0);
                    EXPECT_EQ((block & MAPGRID_ELEVATION_MASK) >> MAPGRID_ELEVATION_SHIFT, marker->elevation);
                }
            }

            for(otherIdx = 0; otherIdx < events->objectEventCount; ++otherIdx)
            {
                const struct ObjectEventTemplate *other = &events->objectEvents[otherIdx];

                if(other == marker)
                    continue;

                EXPECT(abs(other->x - marker->x) > 1 || abs(other->y - marker->y) > 1);
            }

            for(warpIdx = 0; warpIdx < events->warpCount; ++warpIdx)
            {
                const struct WarpEvent *warp = &events->warps[warpIdx];

                EXPECT(abs(warp->x - marker->x) > 1 || abs(warp->y - marker->y) > 1);
            }
        }
    }
}

TEST("Route event rewriting preserves marker identity and applies generated NPCs")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    u8 originalRoomId = gRogueRun.adventureRoomId;
    struct ObjectEventTemplate markers[ROGUE_ROUTE_EVENT_ANCHOR_COUNT] =
    {
        {
            .localId = 41,
            .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE,
            .x = 12,
            .y = 34,
            .trainerType = TRAINER_TYPE_NONE,
            .trainerRange_berryTreeId = ROGUE_ROUTE_EVENT_ANCHOR_SENDER,
            .script = Rogue_RouteEvent_Interact,
        },
        {
            .localId = 42,
            .graphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE,
            .x = 56,
            .y = 78,
            .trainerType = TRAINER_TYPE_NONE,
            .trainerRange_berryTreeId = ROGUE_ROUTE_EVENT_ANCHOR_RECIPIENT,
            .script = Rogue_RouteEvent_Interact,
        },
    };

    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueAdvPath.roomCount = 1;
    gRogueRun.adventureRoomId = 0;
    gRogueAdvPath.rooms[0].routeEvent.eventType = ROGUE_ROUTE_EVENT_DELIVERY;
    gRogueAdvPath.rooms[0].routeEvent.senderGraphicsId = OBJ_EVENT_GFX_GENTLEMAN;
    gRogueAdvPath.rooms[0].routeEvent.recipientGraphicsId = OBJ_EVENT_GFX_PICNICKER;

    RogueRouteEvents_ModifyObjectEvents(markers, ARRAY_COUNT(markers));

    EXPECT_EQ(markers[0].localId, 41);
    EXPECT_EQ(markers[0].x, 12);
    EXPECT_EQ(markers[0].y, 34);
    EXPECT_EQ(markers[0].graphicsId, OBJ_EVENT_GFX_GENTLEMAN);
    EXPECT_EQ(markers[0].movementType, MOVEMENT_TYPE_FACE_DOWN);
    EXPECT_EQ(markers[1].localId, 42);
    EXPECT_EQ(markers[1].x, 56);
    EXPECT_EQ(markers[1].y, 78);
    EXPECT_EQ(markers[1].graphicsId, OBJ_EVENT_GFX_PICNICKER);
    EXPECT_EQ(markers[1].movementType, MOVEMENT_TYPE_FACE_DOWN);

    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route event deliveries persist, complete atomically, and clean up on exit")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    u8 originalRoomId = gRogueRun.adventureRoomId;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);

    ClearBag();
    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueAdvPath.roomCount = 1;
    gRogueRun.adventureRoomId = 0;
    gRogueAdvPath.rooms[0].routeEvent.eventType = ROGUE_ROUTE_EVENT_DELIVERY;
    gRogueAdvPath.rooms[0].routeEvent.rewardItem = ITEM_NUGGET;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);

    RogueRouteEvents_TryAcceptDelivery();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_CARRYING);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));

    // No route lifecycle hook runs during a same-map quicksave restore.
    RogueRouteEvents_ModifyObjectEvents(NULL, 0);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_CARRYING);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));

    RogueRouteEvents_TryCompleteDelivery();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    EXPECT(!CheckBagHasItem(ITEM_PARCEL, 1));
    EXPECT(CheckBagHasItem(ITEM_NUGGET, 1));

    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    RogueRouteEvents_TryAcceptDelivery();
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));
    RogueRouteEvents_OnExitRoute();
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    EXPECT(!CheckBagHasItem(ITEM_PARCEL, 1));

    EXPECT(AddBagItem(ITEM_PARCEL, 1));
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_CARRYING);
    RogueRouteEvents_OnEnterRoute();
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    EXPECT(!CheckBagHasItem(ITEM_PARCEL, 1));

    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}

TEST("Route event completion waits when the reward pocket is full")
{
    struct RogueAdvPath originalPath = gRogueAdvPath;
    u8 originalRoomId = gRogueRun.adventureRoomId;
    u16 originalState = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);
    u16 itemId;

    ClearBag();
    memset(&gRogueAdvPath, 0, sizeof(gRogueAdvPath));
    gRogueAdvPath.roomCount = 1;
    gRogueRun.adventureRoomId = 0;
    gRogueAdvPath.rooms[0].routeEvent.eventType = ROGUE_ROUTE_EVENT_DELIVERY;
    gRogueAdvPath.rooms[0].routeEvent.rewardItem = ITEM_NUGGET;
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
    RogueRouteEvents_TryAcceptDelivery();
    EXPECT_EQ(gSpecialVar_Result, TRUE);

    for(itemId = ITEM_NONE + 1; itemId < ITEMS_COUNT && GetBagUnreservedFreeSlots() != 0; ++itemId)
    {
        u8 pocket = ItemId_GetPocket(itemId);

        if(pocket != POCKET_NONE
            && pocket != POCKET_KEY_ITEMS
            && itemId != ITEM_PARCEL
            && itemId != ITEM_NUGGET)
            AddBagItem(itemId, 1);
    }

    EXPECT_EQ(GetBagUnreservedFreeSlots(), 0);
    EXPECT(!CheckBagHasSpace(ITEM_NUGGET, 1));
    RogueRouteEvents_TryCompleteDelivery();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    EXPECT_EQ(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE), ROGUE_ROUTE_EVENT_STATE_CARRYING);
    EXPECT(CheckBagHasItem(ITEM_PARCEL, 1));
    EXPECT(!CheckBagHasItem(ITEM_NUGGET, 1));

    ClearBag();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, originalState);
    gRogueAdvPath = originalPath;
    gRogueRun.adventureRoomId = originalRoomId;
}
