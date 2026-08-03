#include "global.h"

#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/items.h"
#include "constants/rogue_route_events.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "item.h"
#include "random.h"

#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_route_events.h"

extern const u8 Rogue_RouteEvent_Interact[];

static const u16 sDeliveryRewards[] =
{
    ITEM_NUGGET,
    ITEM_RARE_CANDY,
    ITEM_PP_UP,
};

static const u16 sDeliveryNpcGraphics[] =
{
    OBJ_EVENT_GFX_POKEFAN_F,
    OBJ_EVENT_GFX_GENTLEMAN,
    OBJ_EVENT_GFX_OLD_WOMAN,
    OBJ_EVENT_GFX_SCIENTIST_1,
    OBJ_EVENT_GFX_HIKER,
    OBJ_EVENT_GFX_PICNICKER,
};

void RogueRouteEvents_GenerateRoom(struct RogueAdvPathRoom *room)
{
    RAND_TYPE originalRng = gRngRogueValue;
    u8 senderIdx;
    u8 recipientIdx;

    memset(&room->routeEvent, 0, sizeof(room->routeEvent));

    SeedRogueRng(room->rngSeed ^ 0xA7E1);

    room->routeEvent.eventType = ROGUE_ROUTE_EVENT_DELIVERY;
    room->routeEvent.rewardItem = sDeliveryRewards[RogueRandom() % ARRAY_COUNT(sDeliveryRewards)];

    senderIdx = RogueRandom() % ARRAY_COUNT(sDeliveryNpcGraphics);
    recipientIdx = (senderIdx + 1 + (RogueRandom() % (ARRAY_COUNT(sDeliveryNpcGraphics) - 1))) % ARRAY_COUNT(sDeliveryNpcGraphics);
    room->routeEvent.senderGraphicsId = sDeliveryNpcGraphics[senderIdx];
    room->routeEvent.recipientGraphicsId = sDeliveryNpcGraphics[recipientIdx];

    gRngRogueValue = originalRng;
}

static void RemoveCarriedParcel(void)
{
    if(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) == ROGUE_ROUTE_EVENT_STATE_CARRYING)
        RemoveBagItem(ITEM_PARCEL, 1);
}

void RogueRouteEvents_OnEnterRoute(void)
{
    RemoveCarriedParcel();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
}

void RogueRouteEvents_OnExitRoute(void)
{
    RemoveCarriedParcel();
    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_NOT_STARTED);
}

bool8 RogueRouteEvents_IsMarkerTemplate(const struct ObjectEventTemplate *objectEvent)
{
    return objectEvent->script == Rogue_RouteEvent_Interact
        && objectEvent->trainerType == TRAINER_TYPE_NONE
        && objectEvent->trainerRange_berryTreeId < ROGUE_ROUTE_EVENT_ANCHOR_COUNT;
}

void RogueRouteEvents_ModifyObjectEvents(struct ObjectEventTemplate *objectEvents, u8 objectEventCount)
{
    const struct RogueRouteEventRoomData *eventData;
    u8 i;

    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return;

    eventData = &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeEvent;
    if(eventData->eventType != ROGUE_ROUTE_EVENT_DELIVERY)
        return;

    for(i = 0; i < objectEventCount; ++i)
    {
        if(!RogueRouteEvents_IsMarkerTemplate(&objectEvents[i]))
            continue;

        if(objectEvents[i].trainerRange_berryTreeId == ROGUE_ROUTE_EVENT_ANCHOR_SENDER)
            objectEvents[i].graphicsId = eventData->senderGraphicsId;
        else
            objectEvents[i].graphicsId = eventData->recipientGraphicsId;

        objectEvents[i].movementType = MOVEMENT_TYPE_FACE_DOWN;
        objectEvents[i].movementRangeX = 0;
        objectEvents[i].movementRangeY = 0;
    }
}

void RogueRouteEvents_GetInteractionData(void)
{
    const struct RogueRouteEventRoomData *eventData;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_ANCHOR_SENDER;
    gSpecialVar_0x8004 = ITEM_NONE;

    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return;

    eventData = &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeEvent;
    if(eventData->eventType != ROGUE_ROUTE_EVENT_DELIVERY)
        return;

    if(gSelectedObjectEvent < OBJECT_EVENTS_COUNT)
        gSpecialVar_Result = gObjectEvents[gSelectedObjectEvent].trainerRange_berryTreeId;
    gSpecialVar_0x8004 = eventData->rewardItem;
}

void RogueRouteEvents_TryAcceptDelivery(void)
{
    gSpecialVar_Result = FALSE;

    if(VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    if(!AddBagItem(ITEM_PARCEL, 1))
        return;

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_CARRYING);
    gSpecialVar_Result = TRUE;
}

void RogueRouteEvents_TryCompleteDelivery(void)
{
    const struct RogueRouteEventRoomData *eventData;

    gSpecialVar_Result = FALSE;

    if(gRogueRun.adventureRoomId >= gRogueAdvPath.roomCount)
        return;

    eventData = &gRogueAdvPath.rooms[gRogueRun.adventureRoomId].routeEvent;
    if(eventData->eventType != ROGUE_ROUTE_EVENT_DELIVERY
        || VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) != ROGUE_ROUTE_EVENT_STATE_CARRYING
        || !CheckBagHasItem(ITEM_PARCEL, 1)
        || !CheckBagHasSpace(eventData->rewardItem, 1))
        return;

    if(!RemoveBagItem(ITEM_PARCEL, 1))
        return;

    if(!AddBagItem(eventData->rewardItem, 1))
    {
        AddBagItem(ITEM_PARCEL, 1);
        return;
    }

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = TRUE;
}
