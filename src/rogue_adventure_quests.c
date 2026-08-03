#include "global.h"

#include "constants/items.h"
#include "constants/rogue_adventure_quests.h"
#include "constants/rogue_route_scenes.h"

#include "item.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_controller.h"

struct RogueAdventureQuestNodeDefinition
{
    u8 sceneRecipeId;
    u8 nextNodeId;
    u8 listenSignal;
    u8 flags;
    u8 routeDelay;
};

struct RogueAdventureQuestDefinition
{
    const u8 *title;
    const struct RogueAdventureQuestNodeDefinition *nodes;
    bool8 (*buildSceneRequest)(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request);
    u16 cleanupItem;
    u8 nodeCount;
    u8 initialNodeId;
};

static const u8 sText_ParcelDeliveryTitle[] = _("Parcel Delivery");
static const u8 sText_ParcelDeliveryActive[] = _("Carry the Parcel. Its recipient will appear during a future route.");
static const u8 sText_ParcelDeliveryReady[] = _("The recipient is waiting on this route.\nReward: {STR_VAR_1}");
static const u8 sText_TrainerHuntTitle[] = _("Trainer Hunt");
static const u8 sText_TrainerHuntDescription[] = _("Defeat Trainers during this adventure.\nProgress: {STR_VAR_1}/{STR_VAR_2}");
static const u8 sText_UnknownQuestTitle[] = _("Adventure Quest");
static const u8 sText_UnknownQuestDescription[] = _("Complete this quest before the adventure ends.");

static bool8 BuildParcelDeliveryScene(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request)
{
    request->rewardItem = quest->payload[0];
    request->primaryGraphicsId = quest->payload[1];
    return TRUE;
}

static const struct RogueAdventureQuestNodeDefinition sParcelDeliveryNodes[] =
{
    {
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_DELIVERY_PAYOFF,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .listenSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

// Trainer Hunt is deliberately definition-only for now. It proves that
// passive objectives and scene-backed nodes share the same graph model before
// a quest giver or payoff scene is added.
static const struct RogueAdventureQuestNodeDefinition sTrainerHuntNodes[] =
{
    {
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_NONE,
        .nextNodeId = 1,
        .listenSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_TRAINER_DEFEATED,
    },
    {
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_NONE,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .listenSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
    },
};

static const struct RogueAdventureQuestDefinition sQuestDefinitions[ROGUE_ADVENTURE_QUEST_DEFINITION_COUNT] =
{
    [ROGUE_ADVENTURE_QUEST_DEFINITION_PARCEL_DELIVERY] =
    {
        .title = sText_ParcelDeliveryTitle,
        .nodes = sParcelDeliveryNodes,
        .buildSceneRequest = BuildParcelDeliveryScene,
        .cleanupItem = ITEM_PARCEL,
        .nodeCount = ARRAY_COUNT(sParcelDeliveryNodes),
        .initialNodeId = 0,
    },
    [ROGUE_ADVENTURE_QUEST_DEFINITION_TRAINER_HUNT] =
    {
        .title = sText_TrainerHuntTitle,
        .nodes = sTrainerHuntNodes,
        .nodeCount = ARRAY_COUNT(sTrainerHuntNodes),
        .initialNodeId = 0,
    },
};

static const struct RogueAdventureQuestDefinition *GetDefinition(u8 definitionId)
{
    if(definitionId == ROGUE_ADVENTURE_QUEST_DEFINITION_NONE
        || definitionId >= ROGUE_ADVENTURE_QUEST_DEFINITION_COUNT
        || sQuestDefinitions[definitionId].nodes == NULL)
        return NULL;

    return &sQuestDefinitions[definitionId];
}

static const struct RogueAdventureQuestNodeDefinition *GetNode(const struct RogueAdventureQuest *quest)
{
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);

    if(definition == NULL || quest->nodeId >= definition->nodeCount)
        return NULL;

    return &definition->nodes[quest->nodeId];
}

static void EnterNode(struct RogueAdventureQuest *quest, u8 nodeId)
{
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);

    if(nodeId == ROGUE_ADVENTURE_QUEST_NODE_COMPLETE
        || definition == NULL
        || nodeId >= definition->nodeCount)
    {
        memset(quest, 0, sizeof(*quest));
        return;
    }

    quest->nodeId = nodeId;
    quest->progress = 0;
    quest->routesUntilScene = definition->nodes[nodeId].routeDelay;
    quest->sceneRoomId = ROGUE_ADVENTURE_QUEST_INVALID_ROOM;
}

void RogueAdventureQuests_Clear(void)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        const struct RogueAdventureQuestDefinition *definition = GetDefinition(gRogueRun.adventureQuests[i].definitionId);

        if(definition != NULL && definition->cleanupItem != ITEM_NONE)
            RemoveBagItem(definition->cleanupItem, 1);
    }

    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
}

u8 RogueAdventureQuests_Create(u8 definitionId, const struct RogueAdventureQuestCreateParams *params)
{
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(definitionId);
    u8 i;

    if(definition == NULL)
        return ROGUE_ADVENTURE_QUEST_INVALID_ID;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];

        if(quest->definitionId == ROGUE_ADVENTURE_QUEST_DEFINITION_NONE)
        {
            memset(quest, 0, sizeof(*quest));
            quest->definitionId = definitionId;
            quest->target = params != NULL && params->target != 0 ? params->target : 1;
            quest->sceneRoomId = ROGUE_ADVENTURE_QUEST_INVALID_ROOM;
            if(params != NULL)
                memcpy(quest->payload, params->payload, sizeof(quest->payload));
            EnterNode(quest, definition->initialNodeId);
            if((definition->nodes[definition->initialNodeId].flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE) != 0
                && quest->routesUntilScene != 0)
                quest->sceneRoomId = gRogueRun.adventureRoomId;
            return i;
        }
    }

    return ROGUE_ADVENTURE_QUEST_INVALID_ID;
}

static bool8 BuildQuestSceneRequest(u8 questId, struct RogueRouteSceneRequest *request)
{
    const struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[questId];
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);
    const struct RogueAdventureQuestNodeDefinition *node = GetNode(quest);

    if(definition == NULL || node == NULL || definition->buildSceneRequest == NULL)
        return FALSE;

    request->recipeId = node->sceneRecipeId;
    request->source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE;
    request->ownerQuestId = questId;
    return definition->buildSceneRequest(quest, request);
}

bool8 RogueAdventureQuests_TryCollectSceneRequest(u8 roomId, struct RogueRouteSceneRequest *request, u16 *priority)
{
    u8 i;

    // Same-room reloads restore the already-bound graph node.
    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];
        const struct RogueAdventureQuestNodeDefinition *node = GetNode(quest);

        if(node != NULL
            && (node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE) != 0
            && quest->routesUntilScene == 0
            && quest->sceneRoomId == roomId)
        {
            *priority = 1000 + ROGUE_ADVENTURE_QUEST_CAPACITY - i;
            return BuildQuestSceneRequest(i, request);
        }
    }

    // One graph node owns the route scene. Slot order is stable and a skipped
    // node receives a cooldown so later quests still get an opportunity.
    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];
        const struct RogueAdventureQuestNodeDefinition *node = GetNode(quest);

        if(node == NULL || (node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE) == 0)
            continue;
        if(quest->sceneRoomId == roomId)
            continue;

        if(quest->routesUntilScene != 0)
            --quest->routesUntilScene;

        if(quest->routesUntilScene == 0)
        {
            quest->sceneRoomId = roomId;
            *priority = 1000 + ROGUE_ADVENTURE_QUEST_CAPACITY - i;
            return BuildQuestSceneRequest(i, request);
        }
    }

    return FALSE;
}

void RogueAdventureQuests_LeaveRoute(u8 roomId)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];

        if(quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_NONE
            && quest->routesUntilScene == 0
            && quest->sceneRoomId == roomId)
        {
            quest->routesUntilScene = 2;
            quest->sceneRoomId = ROGUE_ADVENTURE_QUEST_INVALID_ROOM;
        }
    }
}

bool8 RogueAdventureQuests_Advance(u8 questId)
{
    struct RogueAdventureQuest *quest;
    const struct RogueAdventureQuestNodeDefinition *node;

    if(questId >= ROGUE_ADVENTURE_QUEST_CAPACITY)
        return FALSE;

    quest = &gRogueRun.adventureQuests[questId];
    node = GetNode(quest);
    if(node == NULL)
        return FALSE;

    EnterNode(quest, node->nextNodeId);
    return TRUE;
}

void RogueAdventureQuests_EmitSignal(u8 signal, u16 value)
{
    u8 i;

    if(signal == ROGUE_ADVENTURE_QUEST_SIGNAL_NONE || value == 0)
        return;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];
        const struct RogueAdventureQuestNodeDefinition *node = GetNode(quest);

        if(node != NULL && node->listenSignal == signal)
        {
            quest->progress = min(quest->target, quest->progress + value);
            if(quest->progress >= quest->target)
                EnterNode(quest, node->nextNodeId);
        }
    }
}

u8 RogueAdventureQuests_GetCount(void)
{
    u8 count = 0;
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        if(gRogueRun.adventureQuests[i].definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_NONE)
            ++count;
    }

    return count;
}

u8 RogueAdventureQuests_GetQuestIdAt(u8 displayIndex)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        if(gRogueRun.adventureQuests[i].definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_NONE)
        {
            if(displayIndex == 0)
                return i;
            --displayIndex;
        }
    }

    return ROGUE_ADVENTURE_QUEST_INVALID_ID;
}

const struct RogueAdventureQuest *RogueAdventureQuests_Get(u8 questId)
{
    if(questId >= ROGUE_ADVENTURE_QUEST_CAPACITY
        || gRogueRun.adventureQuests[questId].definitionId == ROGUE_ADVENTURE_QUEST_DEFINITION_NONE)
        return NULL;

    return &gRogueRun.adventureQuests[questId];
}

u8 RogueAdventureQuests_GetState(u8 questId)
{
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);
    const struct RogueAdventureQuestNodeDefinition *node;

    if(quest == NULL)
        return ROGUE_ADVENTURE_QUEST_STATE_EMPTY;
    if(quest->routesUntilScene == 0 && quest->sceneRoomId != ROGUE_ADVENTURE_QUEST_INVALID_ROOM)
        return ROGUE_ADVENTURE_QUEST_STATE_READY;

    node = GetNode(quest);
    if(node != NULL && node->listenSignal == ROGUE_ADVENTURE_QUEST_SIGNAL_NONE
        && (node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE) == 0)
        return ROGUE_ADVENTURE_QUEST_STATE_READY;

    return ROGUE_ADVENTURE_QUEST_STATE_ACTIVE;
}

const u8 *RogueAdventureQuests_GetTitle(u8 questId)
{
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);
    const struct RogueAdventureQuestDefinition *definition;

    if(quest == NULL)
        return sText_UnknownQuestTitle;

    definition = GetDefinition(quest->definitionId);
    return definition == NULL ? sText_UnknownQuestTitle : definition->title;
}

void RogueAdventureQuests_BufferDescription(u8 questId, u8 *dest)
{
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);

    if(quest == NULL)
    {
        StringCopy(dest, sText_UnknownQuestDescription);
        return;
    }

    switch(quest->definitionId)
    {
    case ROGUE_ADVENTURE_QUEST_DEFINITION_PARCEL_DELIVERY:
        if(RogueAdventureQuests_GetState(questId) == ROGUE_ADVENTURE_QUEST_STATE_READY)
        {
            StringCopy(gStringVar1, ItemId_GetName(quest->payload[0]));
            StringExpandPlaceholders(dest, sText_ParcelDeliveryReady);
        }
        else
            StringCopy(dest, sText_ParcelDeliveryActive);
        break;
    case ROGUE_ADVENTURE_QUEST_DEFINITION_TRAINER_HUNT:
        ConvertUIntToDecimalStringN(gStringVar1,
            RogueAdventureQuests_GetState(questId) == ROGUE_ADVENTURE_QUEST_STATE_READY ? quest->target : quest->progress,
            STR_CONV_MODE_LEFT_ALIGN, 3);
        ConvertUIntToDecimalStringN(gStringVar2, quest->target, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringExpandPlaceholders(dest, sText_TrainerHuntDescription);
        break;
    default:
        StringCopy(dest, sText_UnknownQuestDescription);
        break;
    }
}
