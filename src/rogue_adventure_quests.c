#include "global.h"

#include "constants/abilities.h"
#include "constants/event_objects.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rogue_adventure_quests.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"

#include "event_data.h"
#include "item.h"
#include "string_util.h"
#include "battle_main.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_trainers.h"

struct RogueAdventureQuestNodeDefinition
{
    const u8 *activeDescription;
    const u8 *readyDescription;
    u8 sceneRecipeId;
    u8 nextNodeId;
    u8 progressSignal;
    u8 completionSignal;
    u8 flags;
    u8 routeDelay;
};

struct RogueAdventureQuestDefinition
{
    const u8 *title;
    const struct RogueAdventureQuestNodeDefinition *nodes;
    bool8 (*buildSceneRequest)(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request);
    void (*prepareDescription)(const struct RogueAdventureQuest *quest);
    u16 fixedQuestItem;
    u8 nodeCount;
    u8 initialNodeId;
    u8 cleanupItemSource;
    u8 protectedItemSource;
};

enum
{
    QUEST_ITEM_SOURCE_NONE,
    QUEST_ITEM_SOURCE_FIXED,
    QUEST_ITEM_SOURCE_PAYLOAD_0,
    QUEST_ITEM_SOURCE_PAYLOAD_1,
};

static const u8 sText_StolenTradeCaseTitle[] = _("Stolen Trade Case");
static const u8 sText_FindCamp[] = _("Track down the thieves and recover the Trade Case.\nReward: Large Bundle + ¥5,000");
static const u8 sText_CampReady[] = _("The thieves are camped somewhere along this route.\nReward: Large Bundle + ¥5,000");
static const u8 sText_ReturnCase[] = _("Take the recovered Trade Case back to the merchant.\nReward: Large Bundle + ¥5,000");
static const u8 sText_MerchantReady[] = _("The caravan merchant is waiting along this route.\nReward: Large Bundle + ¥5,000");
static const u8 sText_AnomalousFossilTitle[] = _("Anomalous Fossil");
static const u8 sText_FindFossilResearcher[] = _("Find the researcher who can restore this fossil.\nReward: Rare Unique {STR_VAR_2}");
static const u8 sText_FossilResearcherReady[] = _("The fossil researcher is waiting along this route.\nReward: Rare Unique {STR_VAR_2}");
static const u8 sText_ForbiddenStoneTitle[] = _("The Forbidden Stone");
static const u8 sText_FindEscapedSouls[] = _("Draw the three missing souls back into the Odd Keystone.\nRecovered: {STR_VAR_1}/3");
static const u8 sText_EscapedSoulsReady[] = _("The Odd Keystone is stirring on this route.\nRecovered: {STR_VAR_1}/3");
static const u8 sText_FindSealingGround[] = _("Return the restored Odd Keystone to the Channeler.\nReward: Ability Patch + ¥10,000");
static const u8 sText_SealingGroundReady[] = _("The Channeler is waiting somewhere along this route.\nReward: Ability Patch + ¥10,000");
static const u8 sText_ApricornCraftingTitle[] = _("Apricorn Crafting");
static const u8 sText_FindBallMaker[] = _("Bring the {STR_VAR_1} to a traveling Ball Maker.\nReward: 5 {STR_VAR_2}");
static const u8 sText_BallMakerReady[] = _("A traveling Ball Maker has set up along this route.\nReward: 5 {STR_VAR_2}");
static const u8 sText_MysteryEggCourierTitle[] = _("Mystery Egg Courier");
static const u8 sText_DeliverMysteryEgg[] = _("Deliver the Egg to the Day Care Lady at a rest stop.\nReward: Escape Rope");
static const u8 sText_FieldRepairBenchTitle[] = _("Field Repair Bench");
static const u8 sText_FindMachineParts[] = _("Find the three machine parts and return to the technician.\nRecovered: {STR_VAR_1}/3");
static const u8 sText_AbilityModulatorReady[] = _("The technician is ready to tune one Pokémon's Ability.\nAbility: {STR_VAR_2}");
static const u8 sText_UnknownAbility[] = _("??????????");
static const u8 sText_UnknownQuestTitle[] = _("Adventure Quest");
static const u8 sText_UnknownQuestDescription[] = _("Complete this quest before the adventure ends.");

static void PrepareAnomalousFossilDescription(const struct RogueAdventureQuest *quest)
{
    CopyItemName(quest->payload[0], gStringVar1);
    StringCopy(gStringVar2, RoguePokedex_GetSpeciesName(RogueAdventureQuests_GetFossilSpecies(quest->payload[0])));
}

static void PrepareForbiddenStoneDescription(const struct RogueAdventureQuest *quest)
{
    u8 recovered = ((quest->progress & (1 << 0)) != 0)
        + ((quest->progress & (1 << 1)) != 0)
        + ((quest->progress & (1 << 2)) != 0);

    ConvertIntToDecimalStringN(gStringVar1, recovered, STR_CONV_MODE_LEFT_ALIGN, 1);
}

static void PrepareApricornCraftingDescription(const struct RogueAdventureQuest *quest)
{
    CopyItemName(quest->payload[0], gStringVar1);
    CopyItemName(quest->payload[1], gStringVar2);
}

static void PrepareFieldRepairBenchDescription(const struct RogueAdventureQuest *quest)
{
    u8 recovered = ((quest->progress & (1 << 1)) != 0)
        + ((quest->progress & (1 << 2)) != 0)
        + ((quest->progress & (1 << 3)) != 0);

    ConvertIntToDecimalStringN(gStringVar1, recovered, STR_CONV_MODE_LEFT_ALIGN, 1);
    if(quest->payload[0] < ABILITIES_COUNT)
        StringCopy(gStringVar2, gAbilityNames[quest->payload[0]]);
    else
        StringCopy(gStringVar2, sText_UnknownAbility);
}

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

static bool8 BuildForbiddenStoneScene(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request)
{
    request->requestedItem = ITEM_ODD_KEYSTONE;
    request->rewardItem = ITEM_ABILITY_PATCH;
    request->rewardAmount = ROGUE_FORBIDDEN_STONE_REWARD_MONEY;
    request->trainerNum = quest->payload[0];
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->primaryGraphicsId = quest->nodeId == 0
        ? OBJ_EVENT_GFX_ROUTE_GHOST
        : OBJ_EVENT_GFX_MISC_CHANNELER;
    return TRUE;
}

static bool8 BuildApricornCraftingScene(const struct RogueAdventureQuest *quest, struct RogueRouteSceneRequest *request)
{
    request->requestedItem = quest->payload[0];
    request->rewardItem = quest->payload[1];
    request->primaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
    request->trainerNum = TRAINER_NONE;
    request->rewardAmount = ROGUE_APRICORN_BALL_REWARD_COUNT;
    return TRUE;
}

#include "data/rogue_adventure_quest_definitions.h"

static const struct RogueAdventureQuestDefinition *GetDefinition(u8 definitionId)
{
    if(definitionId == ROGUE_ADVENTURE_QUEST_DEFINITION_NONE
        || definitionId >= ROGUE_ADVENTURE_QUEST_DEFINITION_COUNT
        || sQuestDefinitions[definitionId].nodes == NULL
        || sQuestDefinitions[definitionId].nodeCount > ROGUE_ADVENTURE_QUEST_MAX_NODE_COUNT)
        return NULL;

    return &sQuestDefinitions[definitionId];
}

static const struct RogueAdventureQuestNodeDefinition *GetNode(const struct RogueAdventureQuest *quest)
{
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);

    if(definition == NULL
        || quest->nodeId >= definition->nodeCount
        || definition->nodes[quest->nodeId].routeDelay > ROGUE_ADVENTURE_QUEST_MAX_ROUTE_DELAY)
        return NULL;

    return &definition->nodes[quest->nodeId];
}

static u16 ResolveQuestItem(
    const struct RogueAdventureQuestDefinition *definition,
    const struct RogueAdventureQuest *quest,
    u8 source)
{
    switch(source)
    {
    case QUEST_ITEM_SOURCE_FIXED:
        return definition->fixedQuestItem;
    case QUEST_ITEM_SOURCE_PAYLOAD_0:
        return quest->payload[0];
    case QUEST_ITEM_SOURCE_PAYLOAD_1:
        return quest->payload[1];
    default:
        return ITEM_NONE;
    }
}

static void EnterNode(struct RogueAdventureQuest *quest, u8 nodeId)
{
    const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);

    if(nodeId == ROGUE_ADVENTURE_QUEST_NODE_COMPLETE
        || definition == NULL
        || nodeId >= definition->nodeCount
        || definition->nodes[nodeId].routeDelay > ROGUE_ADVENTURE_QUEST_MAX_ROUTE_DELAY)
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
    u16 item;

    if(definition == NULL)
        return;

    item = ResolveQuestItem(definition, quest, definition->cleanupItemSource);
    if(item != ITEM_NONE)
        RemoveBagItem(item, 1);
}

void RogueAdventureQuests_Clear(void)
{
    u8 i;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
        CleanupQuest(&gRogueRun.adventureQuests[i]);

    memset(gRogueRun.adventureQuests, 0, sizeof(gRogueRun.adventureQuests));
    FlagClear(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
}

void RogueAdventureQuests_Remove(u8 questId)
{
    if(questId >= ROGUE_ADVENTURE_QUEST_CAPACITY)
        return;

    CleanupQuest(&gRogueRun.adventureQuests[questId]);
    memset(&gRogueRun.adventureQuests[questId], 0, sizeof(gRogueRun.adventureQuests[questId]));
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

u8 RogueAdventureQuests_FindByDefinition(u8 definitionId)
{
    u8 i;

    if(GetDefinition(definitionId) == NULL)
        return ROGUE_ADVENTURE_QUEST_INVALID_ID;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
    {
        if(gRogueRun.adventureQuests[i].definitionId == definitionId)
            return i;
    }

    return ROGUE_ADVENTURE_QUEST_INVALID_ID;
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
        const struct RogueAdventureQuestDefinition *definition = GetDefinition(quest->definitionId);

        if(itemId != ITEM_NONE
            && definition != NULL
            && definition->protectedItemSource != QUEST_ITEM_SOURCE_NONE
            && ResolveQuestItem(definition, quest, definition->protectedItemSource) == itemId)
            return TRUE;
    }

    return FALSE;
}

bool8 RogueAdventureQuests_BuildSceneRequest(u8 questId, struct RogueRouteSceneRequest *request)
{
    const struct RogueAdventureQuest *quest;
    const struct RogueAdventureQuestDefinition *definition;
    const struct RogueAdventureQuestNodeDefinition *node;

    if(questId >= ROGUE_ADVENTURE_QUEST_CAPACITY)
        return FALSE;

    quest = &gRogueRun.adventureQuests[questId];
    definition = GetDefinition(quest->definitionId);
    node = GetNode(quest);

    if(definition == NULL || node == NULL || definition->buildSceneRequest == NULL)
        return FALSE;

    request->recipeId = node->sceneRecipeId;
    request->source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE;
    request->ownerQuestId = questId;
    return definition->buildSceneRequest(quest, request);
}

u8 RogueAdventureQuests_CollectSceneRequests(u8 roomId, struct RogueRouteSceneRequest *requests, u8 capacity)
{
    u8 count = 0;
    u8 i;

    if(capacity == 0)
        return 0;

    // Same-room reloads restore the already-bound graph node.
    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY && count < capacity; ++i)
    {
        struct RogueAdventureQuest *quest = &gRogueRun.adventureQuests[i];
        const struct RogueAdventureQuestNodeDefinition *node = GetNode(quest);

        if(node != NULL
            && (node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE) != 0
            && quest->routesUntilScene == 0
            && quest->sceneRoomId == roomId)
        {
            if(RogueAdventureQuests_BuildSceneRequest(i, &requests[count]))
                ++count;
        }
    }

    // A bound node means this is a same-route reload. Rebuild exactly the
    // existing set without advancing cooldowns or binding newly-ready quests.
    if(count != 0)
        return count;

    // Fill the remaining route capacity with ready graph nodes. Nodes which do
    // not fit remain ready for the next route; selected nodes receive a
    // cooldown only if the player leaves their bound route unfinished.
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

        if(quest->routesUntilScene == 0 && count < capacity)
        {
            quest->sceneRoomId = roomId;
            if(RogueAdventureQuests_BuildSceneRequest(i, &requests[count]))
                ++count;
        }
    }

    return count;
}

bool8 RogueAdventureQuests_TryCollectSceneRequest(u8 roomId, struct RogueRouteSceneRequest *request, u16 *priority)
{
    if(RogueAdventureQuests_CollectSceneRequests(roomId, request, 1) == 0)
        return FALSE;

    *priority = 1000 + ROGUE_ADVENTURE_QUEST_CAPACITY - request->ownerQuestId;
    return TRUE;
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

static u8 CountProgressBits(u8 progress)
{
    u8 count = 0;

    while(progress != 0)
    {
        count += progress & 1;
        progress >>= 1;
    }

    return count;
}

static bool8 IsProgressTargetMet(const struct RogueAdventureQuest *quest, const struct RogueAdventureQuestNodeDefinition *node)
{
    if((node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_PROGRESS_SET_BITS) != 0)
        return CountProgressBits(quest->progress) >= quest->target;

    return quest->progress >= quest->target;
}

bool8 RogueAdventureQuests_IsProgressTargetMet(u8 questId)
{
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);
    const struct RogueAdventureQuestNodeDefinition *node;

    if(quest == NULL)
        return FALSE;

    node = GetNode(quest);
    return node != NULL && IsProgressTargetMet(quest, node);
}

bool8 RogueAdventureQuests_EmitSignalForQuest(u8 questId, u8 signal, u16 value)
{
    struct RogueAdventureQuest *quest;
    const struct RogueAdventureQuestNodeDefinition *node;
    bool8 handled = FALSE;

    if(questId >= ROGUE_ADVENTURE_QUEST_CAPACITY
        || signal == ROGUE_ADVENTURE_QUEST_SIGNAL_NONE
        || signal >= ROGUE_ADVENTURE_QUEST_SIGNAL_COUNT)
        return FALSE;

    quest = &gRogueRun.adventureQuests[questId];
    node = GetNode(quest);
    if(node == NULL)
        return FALSE;

    if(node->progressSignal == signal)
    {
        handled = TRUE;
        if((node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_PROGRESS_SET_BITS) != 0)
            quest->progress |= (u8)value;
        else
            quest->progress = min((u16)quest->target, quest->progress + value);

        if((node->flags & ROGUE_ADVENTURE_QUEST_NODE_FLAG_ADVANCE_ON_PROGRESS) != 0
            && IsProgressTargetMet(quest, node))
        {
            EnterNode(quest, node->nextNodeId);
            return TRUE;
        }
    }

    if(node->completionSignal == signal)
    {
        EnterNode(quest, node->nextNodeId);
        return TRUE;
    }

    return handled;
}

void RogueAdventureQuests_EmitSignal(u8 signal, u16 value)
{
    u8 i;

    if(signal == ROGUE_ADVENTURE_QUEST_SIGNAL_NONE
        || signal >= ROGUE_ADVENTURE_QUEST_SIGNAL_COUNT)
        return;

    for(i = 0; i < ROGUE_ADVENTURE_QUEST_CAPACITY; ++i)
        RogueAdventureQuests_EmitSignalForQuest(i, signal, value);
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
    if(node != NULL
        && node->progressSignal == ROGUE_ADVENTURE_QUEST_SIGNAL_NONE
        && node->completionSignal == ROGUE_ADVENTURE_QUEST_SIGNAL_NONE
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
    const struct RogueAdventureQuestDefinition *definition;
    const struct RogueAdventureQuestNodeDefinition *node;
    const u8 *description;

    if(quest == NULL)
    {
        StringCopy(dest, sText_UnknownQuestDescription);
        return;
    }

    definition = GetDefinition(quest->definitionId);
    node = GetNode(quest);
    if(definition == NULL || node == NULL)
    {
        StringCopy(dest, sText_UnknownQuestDescription);
        return;
    }

    if(definition->prepareDescription != NULL)
        definition->prepareDescription(quest);

    description = RogueAdventureQuests_GetState(questId) == ROGUE_ADVENTURE_QUEST_STATE_READY
        ? node->readyDescription
        : node->activeDescription;
    if(description == NULL)
        description = sText_UnknownQuestDescription;
    StringExpandPlaceholders(dest, description);
}
