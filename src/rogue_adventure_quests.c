#include "global.h"

#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rogue_adventure_quests.h"
#include "constants/rogue_route_scenes.h"

#include "event_data.h"
#include "item.h"
#include "string_util.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_trainers.h"

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

static const u8 sText_StolenTradeCaseTitle[] = _("Stolen Trade Case");
static const u8 sText_FindCamp[] = _("Find the thieves' camp and recover the Trade Case.\nReward: Large Bundle + ¥5,000");
static const u8 sText_CampReady[] = _("The thieves' camp is on this route.\nReward: Large Bundle + ¥5,000");
static const u8 sText_ReturnCase[] = _("Return the Trade Case to the caravan merchant.\nReward: Large Bundle + ¥5,000");
static const u8 sText_MerchantReady[] = _("The caravan merchant is on this route.\nReward: Large Bundle + ¥5,000");
static const u8 sText_AnomalousFossilTitle[] = _("Anomalous Fossil");
static const u8 sText_FindFossilResearcher[] = _("Find a fossil researcher who can restore {STR_VAR_1}.\nReward: Rare Unique {STR_VAR_2}");
static const u8 sText_FossilResearcherReady[] = _("A fossil researcher is on this route.\nReward: Rare Unique {STR_VAR_2}");
static const u8 sText_UnknownQuestTitle[] = _("Adventure Quest");
static const u8 sText_UnknownQuestDescription[] = _("Complete this quest before the adventure ends.");

u16 RogueAdventureQuests_GetFossilSpecies(u16 fossilItem)
{
    switch(fossilItem)
    {
    case ITEM_HELIX_FOSSIL:
        return SPECIES_OMANYTE;
    case ITEM_DOME_FOSSIL:
        return SPECIES_KABUTO;
    case ITEM_OLD_AMBER:
        return SPECIES_AERODACTYL;
    case ITEM_ROOT_FOSSIL:
        return SPECIES_LILEEP;
    case ITEM_CLAW_FOSSIL:
        return SPECIES_ANORITH;
#ifdef ROGUE_EXPANSION
    case ITEM_ARMOR_FOSSIL:
        return SPECIES_SHIELDON;
    case ITEM_SKULL_FOSSIL:
        return SPECIES_CRANIDOS;
    case ITEM_COVER_FOSSIL:
        return SPECIES_TIRTOUGA;
    case ITEM_PLUME_FOSSIL:
        return SPECIES_ARCHEN;
    case ITEM_JAW_FOSSIL:
        return SPECIES_TYRUNT;
    case ITEM_SAIL_FOSSIL:
        return SPECIES_AMAURA;
#endif
    default:
        return SPECIES_NONE;
    }
}

static bool8 BuildStolenTradeCaseScene(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request)
{
    request->requestedItem = ITEM_TRADE_CASE;
    request->rewardItem = ITEM_BIG_POKEBLOCK_BUNDLE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;

    if(quest->nodeId == 0)
    {
        request->trainerNum = quest->payload[1];
        request->primaryGraphicsId = Rogue_GetTrainerObjectEventGfx(request->trainerNum);
    }
    else
    {
        request->trainerNum = TRAINER_NONE;
        request->primaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    }

    return TRUE;
}

static bool8 BuildAnomalousFossilScene(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request)
{
    u16 species = RogueAdventureQuests_GetFossilSpecies(quest->payload[0]);

    if(species == SPECIES_NONE)
        return FALSE;

    request->requestedItem = quest->payload[0];
    request->rewardItem = species;
    request->rewardAmount = quest->payload[1];
    request->primaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_2;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_2;
    request->trainerNum = TRAINER_NONE;
    return TRUE;
}

static const struct RogueAdventureQuestNodeDefinition sStolenTradeCaseNodes[] =
{
    {
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP,
        .nextNodeId = 1,
        .listenSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
    {
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .listenSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

static const struct RogueAdventureQuestNodeDefinition sAnomalousFossilNodes[] =
{
    {
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .listenSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

static const struct RogueAdventureQuestDefinition sQuestDefinitions[ROGUE_ADVENTURE_QUEST_DEFINITION_COUNT] =
{
    [ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE] =
    {
        .title = sText_StolenTradeCaseTitle,
        .nodes = sStolenTradeCaseNodes,
        .buildSceneRequest = BuildStolenTradeCaseScene,
        .cleanupItem = ITEM_TRADE_CASE,
        .nodeCount = ARRAY_COUNT(sStolenTradeCaseNodes),
        .initialNodeId = 0,
    },
    [ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL] =
    {
        .title = sText_AnomalousFossilTitle,
        .nodes = sAnomalousFossilNodes,
        .buildSceneRequest = BuildAnomalousFossilScene,
        .cleanupItem = ITEM_NONE,
        .nodeCount = ARRAY_COUNT(sAnomalousFossilNodes),
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

static void CleanupQuest(const struct RogueAdventureQuest *quest)
{
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);

    if(definition != NULL && definition->cleanupItem != ITEM_NONE)
        RemoveBagItem(definition->cleanupItem, 1);
    else if(quest->definitionId == ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL)
        RemoveBagItem(quest->payload[0], 1);
}

void RogueAdventureQuests_Clear(void)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
        CleanupQuest(&gRogueRun.adventureQuests[i]);

    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
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

bool8 RogueAdventureQuests_HasDefinition(u8 definitionId)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        if(gRogueRun.adventureQuests[i].definitionId == definitionId)
            return TRUE;
    }

    return FALSE;
}

bool8 RogueAdventureQuests_IsDefinitionSourceRoom(u8 definitionId, u8 roomId)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        const struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];

        if(quest->definitionId == definitionId
            && quest->routesUntilScene != 0
            && quest->sceneRoomId == roomId)
            return TRUE;
    }

    return FALSE;
}

bool8 RogueAdventureQuests_IsItemProtected(u16 itemId)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        const struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];

        if(quest->definitionId == ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL
            && quest->payload[0] == itemId)
            return TRUE;
    }

    return FALSE;
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

bool8 RogueAdventureQuests_SetProgress(u8 questId, u8 progress)
{
    struct RogueAdventureQuest *quest;

    if(questId >= ROGUE_ADVENTURE_QUEST_CAPACITY)
        return FALSE;

    quest = &gRogueRun.adventureQuests[questId];
    if(GetNode(quest) == NULL)
        return FALSE;

    quest->progress = progress;
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
    case ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE:
        if(quest->nodeId == 0)
            StringExpandPlaceholders(dest, RogueAdventureQuests_GetState(questId) == ROGUE_ADVENTURE_QUEST_STATE_READY ? sText_CampReady : sText_FindCamp);
        else
            StringExpandPlaceholders(dest, RogueAdventureQuests_GetState(questId) == ROGUE_ADVENTURE_QUEST_STATE_READY ? sText_MerchantReady : sText_ReturnCase);
        break;
    case ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL:
        CopyItemName(quest->payload[0], gStringVar1);
        StringCopy(gStringVar2, RoguePokedex_GetSpeciesName(RogueAdventureQuests_GetFossilSpecies(quest->payload[0])));
        StringExpandPlaceholders(dest, RogueAdventureQuests_GetState(questId) == ROGUE_ADVENTURE_QUEST_STATE_READY ? sText_FossilResearcherReady : sText_FindFossilResearcher);
        break;
    default:
        StringCopy(dest, sText_UnknownQuestDescription);
        break;
    }
}
