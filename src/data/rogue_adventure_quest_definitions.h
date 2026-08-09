#ifndef GUARD_DATA_ROGUE_ADVENTURE_QUEST_DEFINITIONS_H
#define GUARD_DATA_ROGUE_ADVENTURE_QUEST_DEFINITIONS_H

// Adventure Quest content lives here. The runtime consumes the same node
// records for scene scheduling, signals, quest-board copy, and item cleanup.

static const struct RogueAdventureQuestNodeDefinition sStolenTradeCaseNodes[] =
{
    {
        .activeDescription = sText_FindCamp,
        .readyDescription = sText_CampReady,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP,
        .nextNodeId = 1,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
    {
        .activeDescription = sText_ReturnCase,
        .readyDescription = sText_MerchantReady,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

static const struct RogueAdventureQuestNodeDefinition sAnomalousFossilNodes[] =
{
    {
        .activeDescription = sText_FindFossilResearcher,
        .readyDescription = sText_FossilResearcherReady,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

static const struct RogueAdventureQuestNodeDefinition sForbiddenStoneNodes[] =
{
    {
        .activeDescription = sText_FindEscapedSouls,
        .readyDescription = sText_EscapedSoulsReady,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS,
        .nextNodeId = 1,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE | ROGUE_ADVENTURE_QUEST_NODE_FLAG_PROGRESS_SET_BITS,
        .routeDelay = 1,
    },
    {
        .activeDescription = sText_FindSealingGround,
        .readyDescription = sText_SealingGroundReady,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

static const struct RogueAdventureQuestNodeDefinition sApricornCraftingNodes[] =
{
    {
        .activeDescription = sText_FindBallMaker,
        .readyDescription = sText_BallMakerReady,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
        .flags = ROGUE_ADVENTURE_QUEST_NODE_FLAG_ROUTE_SCENE,
        .routeDelay = 1,
    },
};

static const struct RogueAdventureQuestNodeDefinition sMysteryEggCourierNodes[] =
{
    {
        .activeDescription = sText_DeliverMysteryEgg,
        .readyDescription = sText_DeliverMysteryEgg,
        .sceneRecipeId = ROGUE_ROUTE_SCENE_RECIPE_NONE,
        .nextNodeId = ROGUE_ADVENTURE_QUEST_NODE_COMPLETE,
        .progressSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_NONE,
        .completionSignal = ROGUE_ADVENTURE_QUEST_SIGNAL_DAYCARE_DELIVERY,
        .flags = 0,
        .routeDelay = 0,
    },
};

static const struct RogueAdventureQuestDefinition sQuestDefinitions[ROGUE_ADVENTURE_QUEST_DEFINITION_COUNT] =
{
    [ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE] =
    {
        .title = sText_StolenTradeCaseTitle,
        .nodes = sStolenTradeCaseNodes,
        .buildSceneRequest = BuildStolenTradeCaseScene,
        .fixedQuestItem = ITEM_TRADE_CASE,
        .nodeCount = ARRAY_COUNT(sStolenTradeCaseNodes),
        .initialNodeId = 0,
        .cleanupItemSource = QUEST_ITEM_SOURCE_FIXED,
        .protectedItemSource = QUEST_ITEM_SOURCE_NONE,
    },
    [ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL] =
    {
        .title = sText_AnomalousFossilTitle,
        .nodes = sAnomalousFossilNodes,
        .buildSceneRequest = BuildAnomalousFossilScene,
        .prepareDescription = PrepareAnomalousFossilDescription,
        .nodeCount = ARRAY_COUNT(sAnomalousFossilNodes),
        .initialNodeId = 0,
        .cleanupItemSource = QUEST_ITEM_SOURCE_PAYLOAD_0,
        .protectedItemSource = QUEST_ITEM_SOURCE_PAYLOAD_0,
    },
    [ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE] =
    {
        .title = sText_ForbiddenStoneTitle,
        .nodes = sForbiddenStoneNodes,
        .buildSceneRequest = BuildForbiddenStoneScene,
        .prepareDescription = PrepareForbiddenStoneDescription,
        .fixedQuestItem = ITEM_ODD_KEYSTONE,
        .nodeCount = ARRAY_COUNT(sForbiddenStoneNodes),
        .initialNodeId = 0,
        .cleanupItemSource = QUEST_ITEM_SOURCE_FIXED,
        .protectedItemSource = QUEST_ITEM_SOURCE_FIXED,
    },
    [ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING] =
    {
        .title = sText_ApricornCraftingTitle,
        .nodes = sApricornCraftingNodes,
        .buildSceneRequest = BuildApricornCraftingScene,
        .prepareDescription = PrepareApricornCraftingDescription,
        .nodeCount = ARRAY_COUNT(sApricornCraftingNodes),
        .initialNodeId = 0,
        .cleanupItemSource = QUEST_ITEM_SOURCE_PAYLOAD_0,
        .protectedItemSource = QUEST_ITEM_SOURCE_PAYLOAD_0,
    },
    [ROGUE_ADVENTURE_QUEST_DEFINITION_MYSTERY_EGG_COURIER] =
    {
        .title = sText_MysteryEggCourierTitle,
        .nodes = sMysteryEggCourierNodes,
        .nodeCount = ARRAY_COUNT(sMysteryEggCourierNodes),
        .initialNodeId = 0,
        .cleanupItemSource = QUEST_ITEM_SOURCE_NONE,
        .protectedItemSource = QUEST_ITEM_SOURCE_NONE,
    },
};

#endif // GUARD_DATA_ROGUE_ADVENTURE_QUEST_DEFINITIONS_H
