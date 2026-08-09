#ifndef GUARD_DATA_ROGUE_ROUTE_SCENE_RECIPES_H
#define GUARD_DATA_ROGUE_ROUTE_SCENE_RECIPES_H

// Route scene presentation lives here. Runtime composition and restoration
// both consume these definitions, so an object's script, graphics,
// visibility, and required authored spot type have one source of truth.

#define SCENE_OBJECT(prop, gfx, objectScript) \
    {objectScript, gfx, prop, ROUTE_SCENE_STATE_MASK_ALL, ROUTE_SCENE_OBJECT_FLAG_NONE}
#define SCENE_OBJECT_UNTIL_COMPLETED(prop, gfx, objectScript) \
    {objectScript, gfx, prop, ROUTE_SCENE_STATE_MASK_UNTIL_COMPLETED, ROUTE_SCENE_OBJECT_FLAG_NONE}
#define SCENE_OBJECT_UNTIL_ROLE_COMPLETE(prop, gfx, objectScript) \
    {objectScript, gfx, prop, ROUTE_SCENE_STATE_MASK_ALL, ROUTE_SCENE_OBJECT_FLAG_HIDE_IF_QUEST_ROLE_COMPLETE}
#define SCENE_SPOT_PAIR_ON(spotType, decorSpotType, terrainMask, objectDefs) \
    {objectDefs, ARRAY_COUNT(objectDefs), spotType, decorSpotType, terrainMask}
#define SCENE_SPOT_ON(spotType, terrainMask, objectDefs) \
    SCENE_SPOT_PAIR_ON(spotType, ROGUE_ROUTE_SCENE_SPOT_TYPE_COUNT, terrainMask, objectDefs)
#define SCENE_SPOT_PAIR(spotType, decorSpotType, objectDefs) \
    SCENE_SPOT_PAIR_ON(spotType, decorSpotType, ROGUE_ROUTE_SCENE_TERRAIN_MASK_STANDARD, objectDefs)
#define SCENE_SPOT(spotType, objectDefs) \
    SCENE_SPOT_ON(spotType, ROGUE_ROUTE_SCENE_TERRAIN_MASK_STANDARD, objectDefs)

static const struct RogueRouteSceneObjectDefinition sStolenTradeCaseOfferObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_StolenTradeCaseOffer),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_SUPPLIES, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sStolenTradeCaseCampObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_StolenTradeCaseCamp),
    SCENE_OBJECT_UNTIL_COMPLETED(1, ROUTE_SCENE_GFX_SEMANTIC_SUPPLIES, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sStolenTradeCasePayoffObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_StolenTradeCasePayoff),
    SCENE_OBJECT_UNTIL_COMPLETED(1, ROUTE_SCENE_GFX_SEMANTIC_SUPPLIES, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sHexedShrineObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_HexedShrine),
};

static const struct RogueRouteSceneObjectDefinition sAnomalousFossilOfferObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_AnomalousFossilOffer),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_RELIC, Rogue_RouteEvent_AnomalousFossilProp),
};

static const struct RogueRouteSceneObjectDefinition sAnomalousFossilRestorationObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_AnomalousFossilRestoration),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_WORKBENCH, Rogue_RouteEvent_FossilWorkbench),
};

static const struct RogueRouteSceneObjectDefinition sForbiddenStoneOfferObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ForbiddenStoneOffer),
};

static const struct RogueRouteSceneObjectDefinition sForbiddenStoneSoulObjects[] =
{
    SCENE_OBJECT_UNTIL_ROLE_COMPLETE(0, ROUTE_SCENE_GFX_SEMANTIC_SPIRIT_STONE, Rogue_RouteEvent_ForbiddenStoneSoul),
};

static const struct RogueRouteSceneObjectDefinition sForbiddenStonePayoffObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ForbiddenStonePayoff),
};

static const struct RogueRouteSceneObjectDefinition sApricornGroveObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ApricornTree),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ApricornTree),
    SCENE_OBJECT(2, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ApricornTree),
};

static const struct RogueRouteSceneObjectDefinition sApricornArtisanObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ApricornArtisan),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_WORKBENCH, Rogue_RouteEvent_ApricornProp),
};

static const struct RogueRouteSceneObjectDefinition sUnboundTutorObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_UnboundTutor),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_CAMP, Rogue_RouteEvent_UnboundTutorProp),
};

static const struct RogueRouteSceneObjectDefinition sTravelingMerchantObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_TravelingMerchant),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_MERCHANT_STALL, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sBreedersExchangeObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_BreedersExchange),
    SCENE_OBJECT_UNTIL_COMPLETED(1, ROUTE_SCENE_GFX_OFFERED_MON, Rogue_RouteEvent_BreedersExchangePokemon),
};

static const struct RogueRouteSceneObjectDefinition sBuriedCacheArchaeologistObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_BuriedCacheArchaeologist),
    SCENE_OBJECT(1, ROUTE_SCENE_GFX_SEMANTIC_WORKBENCH, Rogue_RouteEvent_BuriedCacheSupplies),
};

static const struct RogueRouteSceneObjectDefinition sBuriedCacheSiteObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_BuriedCacheSite),
};

static const struct RogueRouteSceneObjectDefinition sTideSalvageObjects[] =
{
    SCENE_OBJECT(0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_TideSalvage),
};

static const struct RogueRouteSceneLotDefinition sStolenTradeCaseOfferLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_STALL_NPC, ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR, sStolenTradeCaseOfferObjects),
};
static const struct RogueRouteSceneLotDefinition sStolenTradeCaseCampLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC, ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR, sStolenTradeCaseCampObjects),
};
static const struct RogueRouteSceneLotDefinition sStolenTradeCasePayoffLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_STALL_NPC, ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR, sStolenTradeCasePayoffObjects),
};
static const struct RogueRouteSceneLotDefinition sHexedShrineLots[] =
{
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC, sHexedShrineObjects),
};
static const struct RogueRouteSceneLotDefinition sAnomalousFossilOfferLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR, sAnomalousFossilOfferObjects),
};
static const struct RogueRouteSceneLotDefinition sAnomalousFossilRestorationLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR, sAnomalousFossilRestorationObjects),
};
static const struct RogueRouteSceneLotDefinition sForbiddenStoneOfferLots[] =
{
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC, sForbiddenStoneOfferObjects),
};
static const struct RogueRouteSceneLotDefinition sForbiddenStoneSoulLots[] =
{
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, sForbiddenStoneSoulObjects),
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, sForbiddenStoneSoulObjects),
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, sForbiddenStoneSoulObjects),
};
static const struct RogueRouteSceneLotDefinition sForbiddenStonePayoffLots[] =
{
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC, sForbiddenStonePayoffObjects),
};
static const struct RogueRouteSceneLotDefinition sApricornGroveLots[] =
{
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH, sApricornGroveObjects),
};
static const struct RogueRouteSceneLotDefinition sApricornGroveAndArtisanLots[] =
{
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_PLANT_PATCH, sApricornGroveObjects),
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR, sApricornArtisanObjects),
};
static const struct RogueRouteSceneLotDefinition sApricornArtisanLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_NPC, ROGUE_ROUTE_SCENE_SPOT_WORKBENCH_DECOR, sApricornArtisanObjects),
};
static const struct RogueRouteSceneLotDefinition sUnboundTutorLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_CAMP_NPC, ROGUE_ROUTE_SCENE_SPOT_CAMP_DECOR, sUnboundTutorObjects),
};
static const struct RogueRouteSceneLotDefinition sTravelingMerchantLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_STALL_NPC, ROGUE_ROUTE_SCENE_SPOT_STALL_DECOR, sTravelingMerchantObjects),
};
static const struct RogueRouteSceneLotDefinition sBreedersExchangeLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_CREATURE_NPC, ROGUE_ROUTE_SCENE_SPOT_CREATURE_DECOR, sBreedersExchangeObjects),
};
static const struct RogueRouteSceneLotDefinition sBuriedCacheLots[] =
{
    SCENE_SPOT_PAIR(ROGUE_ROUTE_SCENE_SPOT_RELIC_NPC, ROGUE_ROUTE_SCENE_SPOT_RELIC_DECOR, sBuriedCacheArchaeologistObjects),
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, sBuriedCacheSiteObjects),
    SCENE_SPOT(ROGUE_ROUTE_SCENE_SPOT_COLLECTABLE, sBuriedCacheSiteObjects),
};
static const struct RogueRouteSceneLotDefinition sTideSalvageLots[] =
{
    SCENE_SPOT_ON(ROGUE_ROUTE_SCENE_SPOT_WATER_NPC, ROGUE_ROUTE_SCENE_TERRAIN_MASK_WATER, sTideSalvageObjects),
};

static const struct RogueRouteRecipeDefinition sRouteRecipes[ROGUE_ROUTE_SCENE_RECIPE_COUNT] =
{
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER] =
    {
        .selectPayload = SelectStolenTradeCaseOfferPayload,
        .expandPayload = ExpandStolenTradeCaseOfferPayload,
        .lots = sStolenTradeCaseOfferLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sStolenTradeCaseOfferLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP] =
    {
        .lots = sStolenTradeCaseCampLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sStolenTradeCaseCampLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
        .resumeBehavior = ROUTE_SCENE_RESUME_REWARD_PENDING_IF_PROGRESS,
        .flags = ROUTE_SCENE_RECIPE_FLAG_EXCLUDE_DYNAMIC_TRAINER,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF] =
    {
        .lots = sStolenTradeCasePayoffLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sStolenTradeCasePayoffLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE,
        .resumeBehavior = ROUTE_SCENE_RESUME_REWARD_PENDING_IF_PROGRESS,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE] =
    {
        .selectPayload = SelectHexedShrinePayload,
        .expandPayload = ExpandHexedShrinePayload,
        .lots = sHexedShrineLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = ARRAY_COUNT(sHexedShrineLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER] =
    {
        .selectPayload = SelectAnomalousFossilOfferPayload,
        .expandPayload = ExpandAnomalousFossilOfferPayload,
        .lots = sAnomalousFossilOfferLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sAnomalousFossilOfferLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION] =
    {
        .lots = sAnomalousFossilRestorationLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sAnomalousFossilRestorationLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER] =
    {
        .selectPayload = SelectForbiddenStoneOfferPayload,
        .expandPayload = ExpandForbiddenStoneOfferPayload,
        .lots = sForbiddenStoneOfferLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sForbiddenStoneOfferLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS] =
    {
        .lots = sForbiddenStoneSoulLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sForbiddenStoneSoulLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
        .resumeBehavior = ROUTE_SCENE_RESUME_COMPLETED_IF_TARGET_MET,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF] =
    {
        .lots = sForbiddenStonePayoffLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sForbiddenStonePayoffLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE,
        .resumeBehavior = ROUTE_SCENE_RESUME_REWARD_PENDING_IF_PROGRESS,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE] =
    {
        .selectPayload = SelectApricornGrovePayload,
        .expandPayload = ExpandApricornGrovePayload,
        .lots = sApricornGroveLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sApricornGroveLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN] =
    {
        .selectPayload = SelectApricornGrovePayload,
        .expandPayload = ExpandApricornGrovePayload,
        .lots = sApricornGroveAndArtisanLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sApricornGroveAndArtisanLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
        .flags = ROUTE_SCENE_RECIPE_FLAG_COMPLETE_LINKED_QUEST_ON_EXIT,
        .completionLotRole = 1,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN] =
    {
        .lots = sApricornArtisanLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sApricornArtisanLots),
        .linkedQuestDefinitionId = ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING,
    },
    [ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR] =
    {
        .selectPayload = SelectUnboundTutorPayload,
        .expandPayload = ExpandUnboundTutorPayload,
        .lots = sUnboundTutorLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = ARRAY_COUNT(sUnboundTutorLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT] =
    {
        .selectPayload = SelectTravelingMerchantPayload,
        .expandPayload = ExpandTravelingMerchantPayload,
        .lots = sTravelingMerchantLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = ARRAY_COUNT(sTravelingMerchantLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE] =
    {
        .selectPayload = SelectBreedersExchangePayload,
        .expandPayload = ExpandBreedersExchangePayload,
        .lots = sBreedersExchangeLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = ARRAY_COUNT(sBreedersExchangeLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE] =
    {
        .selectPayload = SelectBuriedCachePayload,
        .expandPayload = ExpandBuriedCachePayload,
        .lots = sBuriedCacheLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = ARRAY_COUNT(sBuriedCacheLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE] =
    {
        .selectPayload = SelectTideSalvagePayload,
        .expandPayload = ExpandTideSalvagePayload,
        .lots = sTideSalvageLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF,
        .lotCount = ARRAY_COUNT(sTideSalvageLots),
    },
};

#undef SCENE_SPOT_ON
#undef SCENE_SPOT_PAIR_ON
#undef SCENE_SPOT_PAIR
#undef SCENE_SPOT
#undef SCENE_OBJECT_UNTIL_ROLE_COMPLETE
#undef SCENE_OBJECT_UNTIL_COMPLETED
#undef SCENE_OBJECT

#endif // GUARD_DATA_ROGUE_ROUTE_SCENE_RECIPES_H
