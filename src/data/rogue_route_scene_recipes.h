#ifndef GUARD_DATA_ROGUE_ROUTE_SCENE_RECIPES_H
#define GUARD_DATA_ROGUE_ROUTE_SCENE_RECIPES_H

// Route scene presentation lives here. Runtime composition and restoration
// both consume these definitions, so an object's placement, script,
// visibility, and accent cells have one source of truth.

#define SCENE_OBJECT(prop, x, y, gfx, objectScript) \
    {objectScript, gfx, x, y, prop, ROUTE_SCENE_STATE_MASK_ALL, ROUTE_SCENE_OBJECT_FLAG_NONE}
#define SCENE_OBJECT_UNTIL_COMPLETED(prop, x, y, gfx, objectScript) \
    {objectScript, gfx, x, y, prop, ROUTE_SCENE_STATE_MASK_UNTIL_COMPLETED, ROUTE_SCENE_OBJECT_FLAG_NONE}
#define SCENE_OBJECT_UNTIL_ROLE_COMPLETE(prop, x, y, gfx, objectScript) \
    {objectScript, gfx, x, y, prop, ROUTE_SCENE_STATE_MASK_ALL, ROUTE_SCENE_OBJECT_FLAG_HIDE_IF_QUEST_ROLE_COMPLETE}
#define SCENE_LOT(size, objectDefs, accentDefs) \
    {objectDefs, accentDefs, ARRAY_COUNT(objectDefs), ARRAY_COUNT(accentDefs), size}

static const struct RogueRouteSceneAccentDefinition sAccentsLeftRight[] =
{
    {-1, 0},
    {1, 0},
};

static const struct RogueRouteSceneAccentDefinition sAccentsLeftRightUp[] =
{
    {-1, 0},
    {1, 0},
    {0, -1},
};

static const struct RogueRouteSceneAccentDefinition sAccentsLeftRightDown[] =
{
    {-1, 0},
    {1, 0},
    {0, 1},
};

static const struct RogueRouteSceneObjectDefinition sStolenTradeCaseOfferObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_StolenTradeCaseOffer),
    SCENE_OBJECT(1, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop),
    SCENE_OBJECT(2, 1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sStolenTradeCaseCampObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_StolenTradeCaseCamp),
    SCENE_OBJECT_UNTIL_COMPLETED(1, 0, 1, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop),
    SCENE_OBJECT(3, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sStolenTradeCasePayoffObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_StolenTradeCasePayoff),
    SCENE_OBJECT_UNTIL_COMPLETED(1, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_Prop),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_Prop),
};

static const struct RogueRouteSceneObjectDefinition sHexedShrineObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_HexedShrine),
    SCENE_OBJECT(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_HexedShrineProp),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp),
    SCENE_OBJECT(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_HexedShrineProp),
};

static const struct RogueRouteSceneObjectDefinition sAnomalousFossilOfferObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_AnomalousFossilOffer),
    SCENE_OBJECT(1, 0, -1, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp),
    SCENE_OBJECT(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_FossilProp),
};

static const struct RogueRouteSceneObjectDefinition sAnomalousFossilRestorationObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_AnomalousFossilRestoration),
    SCENE_OBJECT(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_FossilProp),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_FossilProp),
    SCENE_OBJECT(3, 1, 0, OBJ_EVENT_GFX_FOSSIL, Rogue_RouteEvent_FossilProp),
};

static const struct RogueRouteSceneObjectDefinition sForbiddenStoneOfferObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ForbiddenStoneOffer),
    SCENE_OBJECT(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_ForbiddenStoneProp),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp),
    SCENE_OBJECT(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp),
};

static const struct RogueRouteSceneObjectDefinition sForbiddenStoneSoulObjects[] =
{
    SCENE_OBJECT_UNTIL_ROLE_COMPLETE(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ForbiddenStoneSoul),
};

static const struct RogueRouteSceneObjectDefinition sForbiddenStonePayoffObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ForbiddenStonePayoff),
    SCENE_OBJECT(1, 0, -1, OBJ_EVENT_GFX_BATTLE_STATUE, Rogue_RouteEvent_ForbiddenStoneProp),
    SCENE_OBJECT(2, -1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp),
    SCENE_OBJECT(3, 1, 0, OBJ_EVENT_GFX_BREAKABLE_ROCK, Rogue_RouteEvent_ForbiddenStoneProp),
};

static const struct RogueRouteSceneObjectDefinition sApricornGroveObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ApricornTree),
    SCENE_OBJECT(1, -1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree),
    SCENE_OBJECT(2, 1, 0, OBJ_EVENT_GFX_BERRY_TREE_LATE_STAGES, Rogue_RouteEvent_ApricornTree),
};

static const struct RogueRouteSceneObjectDefinition sApricornArtisanObjects[] =
{
    SCENE_OBJECT(0, 0, 0, ROUTE_SCENE_GFX_PRIMARY, Rogue_RouteEvent_ApricornArtisan),
    SCENE_OBJECT(1, -1, 0, OBJ_EVENT_GFX_BIRCHS_BAG, Rogue_RouteEvent_ApricornProp),
    SCENE_OBJECT(2, 1, 0, OBJ_EVENT_GFX_MOVING_BOX, Rogue_RouteEvent_ApricornProp),
};

static const struct RogueRouteSceneLotDefinition sStolenTradeCaseOfferLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sStolenTradeCaseOfferObjects, sAccentsLeftRight),
};
static const struct RogueRouteSceneLotDefinition sStolenTradeCaseCampLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sStolenTradeCaseCampObjects, sAccentsLeftRightDown),
};
static const struct RogueRouteSceneLotDefinition sStolenTradeCasePayoffLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sStolenTradeCasePayoffObjects, sAccentsLeftRight),
};
static const struct RogueRouteSceneLotDefinition sHexedShrineLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sHexedShrineObjects, sAccentsLeftRightUp),
};
static const struct RogueRouteSceneLotDefinition sAnomalousFossilOfferLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sAnomalousFossilOfferObjects, sAccentsLeftRightUp),
};
static const struct RogueRouteSceneLotDefinition sAnomalousFossilRestorationLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sAnomalousFossilRestorationObjects, sAccentsLeftRightUp),
};
static const struct RogueRouteSceneLotDefinition sForbiddenStoneOfferLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sForbiddenStoneOfferObjects, sAccentsLeftRightUp),
};
static const struct RogueRouteSceneLotDefinition sForbiddenStoneSoulLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_SMALL, sForbiddenStoneSoulObjects, sAccentsLeftRight),
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_SMALL, sForbiddenStoneSoulObjects, sAccentsLeftRight),
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_SMALL, sForbiddenStoneSoulObjects, sAccentsLeftRight),
};
static const struct RogueRouteSceneLotDefinition sForbiddenStonePayoffLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_LARGE, sForbiddenStonePayoffObjects, sAccentsLeftRightUp),
};
static const struct RogueRouteSceneLotDefinition sApricornGroveLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sApricornGroveObjects, sAccentsLeftRight),
};
static const struct RogueRouteSceneLotDefinition sApricornGroveAndArtisanLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sApricornGroveObjects, sAccentsLeftRight),
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sApricornArtisanObjects, sAccentsLeftRight),
};
static const struct RogueRouteSceneLotDefinition sApricornArtisanLots[] =
{
    SCENE_LOT(ROGUE_ROUTE_SCENE_LOT_MEDIUM, sApricornArtisanObjects, sAccentsLeftRight),
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
    },
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP] =
    {
        .lots = sStolenTradeCaseCampLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sStolenTradeCaseCampLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF] =
    {
        .lots = sStolenTradeCasePayoffLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sStolenTradeCasePayoffLots),
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
    },
    [ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION] =
    {
        .lots = sAnomalousFossilRestorationLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sAnomalousFossilRestorationLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER] =
    {
        .selectPayload = SelectForbiddenStoneOfferPayload,
        .expandPayload = ExpandForbiddenStoneOfferPayload,
        .lots = sForbiddenStoneOfferLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sForbiddenStoneOfferLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS] =
    {
        .lots = sForbiddenStoneSoulLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sForbiddenStoneSoulLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF] =
    {
        .lots = sForbiddenStonePayoffLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sForbiddenStonePayoffLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE] =
    {
        .selectPayload = SelectApricornGrovePayload,
        .expandPayload = ExpandApricornGrovePayload,
        .lots = sApricornGroveLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sApricornGroveLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN] =
    {
        .selectPayload = SelectApricornGrovePayload,
        .expandPayload = ExpandApricornGrovePayload,
        .lots = sApricornGroveAndArtisanLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR,
        .lotCount = ARRAY_COUNT(sApricornGroveAndArtisanLots),
    },
    [ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN] =
    {
        .lots = sApricornArtisanLots,
        .source = ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE,
        .lotCount = ARRAY_COUNT(sApricornArtisanLots),
    },
};

#undef SCENE_LOT
#undef SCENE_OBJECT_UNTIL_ROLE_COMPLETE
#undef SCENE_OBJECT_UNTIL_COMPLETED
#undef SCENE_OBJECT

#endif // GUARD_DATA_ROGUE_ROUTE_SCENE_RECIPES_H
