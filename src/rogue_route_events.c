#include "global.h"

#include "constants/abilities.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/flags.h"
#include "constants/field_weather.h"
#include "constants/items.h"
#include "constants/metatile_labels.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/pokemon.h"
#include "constants/rogue.h"
#include "constants/rogue_route_events.h"
#include "constants/rogue_route_scenes.h"
#include "constants/trainer_types.h"
#include "constants/vars.h"

#include "event_data.h"
#include "event_object_movement.h"
#include "battle_main.h"
#include "characters.h"
#include "daycare.h"
#include "fieldmap.h"
#include "field_screen_effect.h"
#include "field_weather.h"
#include "item.h"
#include "mail.h"
#include "money.h"
#include "move_relearner.h"
#include "overworld.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "pokedex.h"
#include "random.h"
#include "strings.h"
#include "string_util.h"
#include "task.h"

#include "rogue.h"
#include "rogue_adventure_quests.h"
#include "rogue_adventurepaths.h"
#include "rogue_baked.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_event_transactions.h"
#include "rogue_followmon.h"
#include "rogue_gifts.h"
#include "rogue_pokedex.h"
#include "rogue_popup.h"
#include "rogue_route_events.h"
#include "rogue_route_scene_internal.h"
#include "rogue_route_scenes.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "rogue_trials.h"

extern const u8 Rogue_RouteEvent_Interact[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseOffer[];
extern const u8 Rogue_RouteEvent_StolenTradeCaseCamp[];
extern const u8 Rogue_RouteEvent_StolenTradeCasePayoff[];
extern const u8 Rogue_RouteEvent_Prop[];
extern const u8 Rogue_RouteEvent_HexedShrine[];
extern const u8 Rogue_RouteEvent_HexedShrineProp[];
extern const u8 Rogue_RouteEvent_AnomalousFossilOffer[];
extern const u8 Rogue_RouteEvent_AnomalousFossilRestoration[];
extern const u8 Rogue_RouteEvent_AnomalousFossilProp[];
extern const u8 Rogue_RouteEvent_FossilWorkbench[];
extern const u8 Rogue_RouteEvent_ForbiddenStoneOffer[];
extern const u8 Rogue_RouteEvent_ForbiddenStoneSoul[];
extern const u8 Rogue_RouteEvent_ForbiddenStonePayoff[];
extern const u8 Rogue_RouteEvent_ApricornTree[];
extern const u8 Rogue_RouteEvent_ApricornArtisan[];
extern const u8 Rogue_RouteEvent_ApricornProp[];
extern const u8 Rogue_RouteEvent_UnboundTutor[];
extern const u8 Rogue_RouteEvent_UnboundTutorProp[];
extern const u8 Rogue_RouteEvent_CampCook[];
extern const u8 Rogue_RouteEvent_CampCookProp[];
extern const u8 Rogue_RouteEvent_MysteryEggCourier[];
extern const u8 Rogue_RouteEvent_MysteryEggCourierProp[];
extern const u8 Rogue_RouteEvent_FieldRepairBench[];
extern const u8 Rogue_RouteEvent_FieldRepairWorkbench[];
extern const u8 Rogue_RouteEvent_FieldRepairPart[];
extern const u8 Rogue_RouteEvent_TravelingMerchant[];
extern const u8 Rogue_RouteEvent_BreedersExchange[];
extern const u8 Rogue_RouteEvent_BreedersExchangePokemon[];
extern const u8 Rogue_RouteEvent_BuriedCacheArchaeologist[];
extern const u8 Rogue_RouteEvent_BuriedCacheSupplies[];
extern const u8 Rogue_RouteEvent_BuriedCacheSite[];
extern const u8 Rogue_RouteEvent_TideSalvage[];
extern const struct Tileset gTileset_General;
extern const struct Tileset gTileset_GeneralHub;

static u32 GetActiveTeamClassFlag(void)
{
    switch(gRogueRun.teamEncounterNum)
    {
    case TEAM_NUM_KANTO_ROCKET:
    case TEAM_NUM_JOHTO_ROCKET:
        return CLASS_FLAG_TEAM_ROCKET;
    case TEAM_NUM_AQUA:
        return CLASS_FLAG_TEAM_AQUA;
    case TEAM_NUM_MAGMA:
        return CLASS_FLAG_TEAM_MAGMA;
    case TEAM_NUM_GALACTIC:
        return CLASS_FLAG_TEAM_GALACTIC;
    case TEAM_NUM_PLASMA:
        return CLASS_FLAG_TEAM_PLASMA;
    case TEAM_NUM_NEOPLASMA:
        return CLASS_FLAG_TEAM_NEOPLASMA;
    case TEAM_NUM_FLARE:
        return CLASS_FLAG_TEAM_FLARE;
    }

    return CLASS_FLAG_TEAM_ROCKET;
}

static u16 SelectEvilTeamTrainer(struct RogueRouteSceneRng *rng)
{
    u32 teamClassFlag = GetActiveTeamClassFlag();
    u16 eligibleCount = 0;
    u16 selectedIdx;
    u16 trainerNum;

    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer *trainer = &gRogueTrainers[trainerNum];

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM) != 0
            && (trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM_BOSS) == 0
            && (trainer->classFlags & teamClassFlag) != 0)
            ++eligibleCount;
    }

    AGB_ASSERT(eligibleCount != 0);
    if(eligibleCount == 0)
        return TRAINER_NONE;

    selectedIdx = RogueRouteSceneRng_Next(rng) % eligibleCount;
    for(trainerNum = 0; trainerNum < gRogueTrainerCount; ++trainerNum)
    {
        const struct RogueTrainer *trainer = &gRogueTrainers[trainerNum];

        if((trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM) != 0
            && (trainer->trainerFlags & TRAINER_FLAG_CLASS_TEAM_BOSS) == 0
            && (trainer->classFlags & teamClassFlag) != 0
            && selectedIdx-- == 0)
            return trainerNum;
    }

    return TRAINER_NONE;
}

static bool8 CanShowStolenTradeCaseOffer(u8 roomId)
{
    (void)roomId;
    if(FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED))
        return FALSE;

    return !RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE)
        && Rogue_GetCurrentDifficulty() < ROGUE_CHAMP_START_DIFFICULTY;
}

static bool8 CanShowHexedShrine(u8 roomId)
{
    if(Rogue_SelectDarkDealCurseItem(0) == ITEM_NONE)
        return FALSE;

    if(gRogueRun.temporaryDarkDealCurseItem == ITEM_NONE)
        return TRUE;

    // Rebuild the accepted scene on a same-route quickload. Once the route is
    // left, the active temporary Curse suppresses future shrine offers.
    return gRogueRun.routeSceneRoomId == roomId
        && (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_HEXED_SHRINE_ACCEPTED) != 0;
}

static const u16 sAnomalousFossilItems[] =
{
    ITEM_HELIX_FOSSIL,
    ITEM_DOME_FOSSIL,
    ITEM_OLD_AMBER,
    ITEM_ROOT_FOSSIL,
    ITEM_CLAW_FOSSIL,
#ifdef ROGUE_EXPANSION
    ITEM_ARMOR_FOSSIL,
    ITEM_SKULL_FOSSIL,
    ITEM_COVER_FOSSIL,
    ITEM_PLUME_FOSSIL,
    ITEM_JAW_FOSSIL,
    ITEM_SAIL_FOSSIL,
#endif
};

static u16 CountEligibleAnomalousFossils(void)
{
    u16 count = 0;
    u16 i;

    for(i = 0; i < ARRAY_COUNT(sAnomalousFossilItems); ++i)
    {
        u16 species = RogueAdventureQuests_GetFossilSpecies(sAnomalousFossilItems[i]);

        if(species != SPECIES_NONE && RoguePokedex_IsSpeciesEnabled(species))
            ++count;
    }

    return count;
}

static bool8 CanShowAnomalousFossilOffer(u8 roomId)
{
    (void)roomId;

    return !RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL)
        && Rogue_GetCurrentDifficulty() < ROGUE_CHAMP_START_DIFFICULTY
        && CountEligibleAnomalousFossils() != 0;
}

static bool8 CanShowForbiddenStoneOffer(u8 roomId)
{
    (void)roomId;

    return !RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE)
        && RoguePokedex_IsSpeciesEnabled(SPECIES_SPIRITOMB);
}

static bool8 CanShowApricornGrove(u8 roomId)
{
    (void)roomId;

    return !RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
}

static bool8 CanShowUnboundTutor(u8 roomId)
{
    (void)roomId;
    return gPlayerPartyCount != 0;
}

static bool8 CanShowCampCook(u8 roomId)
{
    (void)roomId;
    return TRUE;
}

static bool8 CanUseMysteryEggCourierSpecies(u16 eggSpecies, bool8 partyHasRoom);

static bool8 CanSelectMysteryEggCourierSpecies(void)
{
    u8 encounterCount = Rogue_GetCurrentWildEncounterCount();
    bool8 partyHasRoom = CalculatePlayerPartyCount() < Rogue_GetMaxPartySize();
    u8 i;

    for(i = 0; i < encounterCount; ++i)
    {
        u16 species = Rogue_GetCurrentWildEncounterSpecies(i);
        u16 eggSpecies = Rogue_GetEggSpecies(species);

        if(CanUseMysteryEggCourierSpecies(eggSpecies, partyHasRoom))
            return TRUE;
    }

    return FALSE;
}

static bool8 CanUseMysteryEggCourierSpecies(u16 eggSpecies, bool8 partyHasRoom)
{
    if(eggSpecies == SPECIES_NONE
        || eggSpecies == SPECIES_EGG
        || !RoguePokedex_IsSpeciesEnabled(eggSpecies))
        return FALSE;

    // Do not offer an Egg that the Species Clause will reject while the party
    // still has room. A full party can replace the conflicting team member.
    return !Rogue_IsSpeciesClauseActive()
        || !partyHasRoom
        || !Rogue_PartyContainsSpeciesChain(eggSpecies, PARTY_SIZE, PARTY_SIZE);
}

static bool8 CanShowMysteryEggCourier(u8 roomId)
{
    (void)roomId;

    return !RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_MYSTERY_EGG_COURIER)
        && Rogue_GetCurrentDifficulty() < ROGUE_CHAMP_START_DIFFICULTY
        && CanSelectMysteryEggCourierSpecies();
}

static bool8 CanShowFieldRepairBench(u8 roomId)
{
    (void)roomId;
    return !RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FIELD_REPAIR_BENCH)
        && gPlayerPartyCount != 0;
}

static bool8 CanShowTravelingMerchant(u8 roomId)
{
    (void)roomId;
    return TRUE;
}

#define BREEDERS_EXCHANGE_BST_TOLERANCE 80
#define BREEDERS_EXCHANGE_PAYLOAD_SPECIES_MASK 0x7FF
#define BREEDERS_EXCHANGE_PAYLOAD_SEED_SHIFT 11
#define BREEDERS_EXCHANGE_PAYLOAD_SEED_MASK 0x1FF
#define BREEDERS_EXCHANGE_OT_ID 0x00BEEFED

static const u8 sBreedersExchangeOtName[] = _("BREEDER");

static bool8 IsBreedersExchangeOfferCandidate(
    u16 species,
    u16 requestedSpecies,
    u16 requestedBst,
    u8 requestedEvolutionCount)
{
    u16 bst;

    if(species == SPECIES_NONE
        || species >= NUM_SPECIES
        || species == requestedSpecies
        || Rogue_GetEggSpecies(species) == Rogue_GetEggSpecies(requestedSpecies)
        || !RoguePokedex_IsSpeciesEnabled(species)
        || RoguePokedex_IsSpeciesLegendary(species)
        || !RogueTrial_IsSpeciesLegal(species, BREEDERS_EXCHANGE_OT_ID)
        || gRoguePokemonProfiles[species].competitiveSetCount == 0
        || Rogue_GetActiveEvolutionCount(species) != requestedEvolutionCount)
        return FALSE;

#ifdef ROGUE_EXPANSION
    // Alternate and battle-only forms should not be handed out as ordinary
    // Breeder stock. Their base form remains eligible in its own right.
    if(GET_BASE_SPECIES_ID(species) != species)
        return FALSE;
#endif

    bst = RoguePokedex_GetSpeciesBST(species);
    return bst + BREEDERS_EXCHANGE_BST_TOLERANCE >= requestedBst
        && requestedBst + BREEDERS_EXCHANGE_BST_TOLERANCE >= bst;
}

static u16 SelectBreedersExchangeOffer(u16 requestedSpecies, u16 seed)
{
    struct RogueRouteSceneRng rng;
    u16 requestedBst = RoguePokedex_GetSpeciesBST(requestedSpecies);
    u8 requestedEvolutionCount = Rogue_GetActiveEvolutionCount(requestedSpecies);
    u16 speciesSpan = NUM_SPECIES - 1;
    u16 startIndex;
    u16 offset;
    u16 species;

    RogueRouteSceneRng_Seed(&rng, seed ^ requestedSpecies ^ 0xBEE5);
    startIndex = RogueRouteSceneRng_Next(&rng) % speciesSpan;
    for(offset = 0; offset < speciesSpan; ++offset)
    {
        species = SPECIES_NONE + 1 + ((startIndex + offset) % speciesSpan);
        if(IsBreedersExchangeOfferCandidate(
            species,
            requestedSpecies,
            requestedBst,
            requestedEvolutionCount))
            return species;
    }

    return SPECIES_NONE;
}

static bool8 CanShowBreedersExchange(u8 roomId)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    u8 encounterCount = Rogue_GetCurrentWildEncounterCount();

    (void)roomId;
    if(RogueTrial_IsActive() && trial != NULL && trial->disableGifts)
        return FALSE;

    // Payload selection performs the more expensive comparable-offer search.
    // Keeping eligibility cheap avoids scanning the full species table once
    // per fallback candidate during a single route-planning pass.
    return encounterCount != 0;
}

static bool8 CanShowBuriedCache(u8 roomId)
{
    (void)roomId;
    return !CheckBagHasItem(ITEM_FIELD_SHOVEL, 1);
}

static bool8 CanShowTideSalvage(u8 roomId)
{
    (void)roomId;
    return TRUE;
}

static const u16 sApricornItems[] =
{
    ITEM_RED_APRICORN,
    ITEM_BLUE_APRICORN,
    ITEM_YELLOW_APRICORN,
    ITEM_GREEN_APRICORN,
    ITEM_PINK_APRICORN,
    ITEM_WHITE_APRICORN,
    ITEM_BLACK_APRICORN,
};

static const u16 sApricornBalls[] =
{
    ITEM_LEVEL_BALL,
    ITEM_LURE_BALL,
    ITEM_MOON_BALL,
    ITEM_FRIEND_BALL,
    ITEM_LOVE_BALL,
    ITEM_FAST_BALL,
    ITEM_HEAVY_BALL,
};

static const u16 sCampCookTypedResistBerries[] =
{
    ITEM_CHILAN_BERRY,
    ITEM_OCCA_BERRY,
    ITEM_PASSHO_BERRY,
    ITEM_WACAN_BERRY,
    ITEM_RINDO_BERRY,
    ITEM_YACHE_BERRY,
    ITEM_CHOPLE_BERRY,
    ITEM_KEBIA_BERRY,
    ITEM_SHUCA_BERRY,
    ITEM_COBA_BERRY,
    ITEM_PAYAPA_BERRY,
    ITEM_TANGA_BERRY,
    ITEM_CHARTI_BERRY,
    ITEM_KASIB_BERRY,
    ITEM_HABAN_BERRY,
    ITEM_COLBUR_BERRY,
    ITEM_BABIRI_BERRY,
    ITEM_ROSELI_BERRY,
};

#include "data/rogue_unbound_tutor_moves.h"

static u16 GetApricornBall(u16 apricorn)
{
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sApricornItems); ++i)
    {
        if(sApricornItems[i] == apricorn)
            return sApricornBalls[i];
    }

    return ITEM_NONE;
}

static bool8 SelectStolenTradeCaseOfferPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    (void)request;
    *payload = SelectEvilTeamTrainer(rng);
    return *payload != TRAINER_NONE;
}

static void ExpandStolenTradeCaseOfferPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->requestedItem = ITEM_TRADE_CASE;
    request->rewardItem = ITEM_BIG_POKEBLOCK_BUNDLE;
    request->trainerNum = payload;
}

#define HEXED_SHRINE_PAYLOAD_ITEM_MASK 0xFFF
#define HEXED_SHRINE_PAYLOAD_REWARD_SHIFT 12

static bool8 SelectHexedShrinePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u16 curseItem;
    u16 rewardAmount;

    (void)request;
    if(gRogueRun.temporaryDarkDealCurseItem != ITEM_NONE
        && (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_HEXED_SHRINE_ACCEPTED) != 0)
        curseItem = gRogueRun.temporaryDarkDealCurseItem;
    else
        curseItem = Rogue_SelectDarkDealCurseItem(RogueRouteSceneRng_Next(rng));

    rewardAmount = min(
        ROGUE_HEXED_SHRINE_REWARD_MAX,
        ROGUE_HEXED_SHRINE_REWARD_BASE + ROGUE_HEXED_SHRINE_REWARD_PER_DIFFICULTY * Rogue_GetCurrentDifficulty());
    if(curseItem == ITEM_NONE
        || curseItem > HEXED_SHRINE_PAYLOAD_ITEM_MASK
        || rewardAmount % 1000 != 0)
        return FALSE;

    *payload = curseItem | ((u32)(rewardAmount / 1000) << HEXED_SHRINE_PAYLOAD_REWARD_SHIFT);
    return TRUE;
}

static void ExpandHexedShrinePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_DEVIL_MAN;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_DEVIL_MAN;
    request->requestedItem = payload & HEXED_SHRINE_PAYLOAD_ITEM_MASK;
    request->rewardAmount = (payload >> HEXED_SHRINE_PAYLOAD_REWARD_SHIFT) * 1000;
}

#define FOSSIL_PAYLOAD_INDEX_MASK 0xF
#define FOSSIL_PAYLOAD_SEED_SHIFT 4

static bool8 SelectAnomalousFossilOfferPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u16 eligibleCount = CountEligibleAnomalousFossils();
    u16 selected = eligibleCount == 0 ? 0 : RogueRouteSceneRng_Next(rng) % eligibleCount;
    u16 rewardSeed = RogueRouteSceneRng_Next(rng);
    u16 i;

    (void)request;
    if(eligibleCount == 0)
        return FALSE;

    for(i = 0; i < ARRAY_COUNT(sAnomalousFossilItems); ++i)
    {
        u16 species = RogueAdventureQuests_GetFossilSpecies(sAnomalousFossilItems[i]);

        if(species != SPECIES_NONE
            && RoguePokedex_IsSpeciesEnabled(species)
            && selected-- == 0)
        {
            AGB_ASSERT(i <= FOSSIL_PAYLOAD_INDEX_MASK);
            *payload = i | ((u32)rewardSeed << FOSSIL_PAYLOAD_SEED_SHIFT);
            return i <= FOSSIL_PAYLOAD_INDEX_MASK;
        }
    }

    return FALSE;
}

static void ExpandAnomalousFossilOfferPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    u8 fossilIndex = payload & FOSSIL_PAYLOAD_INDEX_MASK;

    if(fossilIndex >= ARRAY_COUNT(sAnomalousFossilItems))
        return;

    request->primaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_1;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_SCIENTIST_2;
    request->requestedItem = sAnomalousFossilItems[fossilIndex];
    request->rewardItem = RogueAdventureQuests_GetFossilSpecies(request->requestedItem);
    request->rewardAmount = payload >> FOSSIL_PAYLOAD_SEED_SHIFT;
}

static bool8 SelectForbiddenStoneOfferPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    (void)request;
    *payload = RogueRouteSceneRng_Next(rng);
    return TRUE;
}

static void ExpandForbiddenStoneOfferPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MISC_CHANNELER;
    request->requestedItem = ITEM_ODD_KEYSTONE;
    request->rewardItem = ITEM_ABILITY_PATCH;
    request->rewardAmount = payload;
}

#define APRICORN_PAYLOAD_CHOICE_BITS 3
#define APRICORN_PAYLOAD_CHOICE_MASK 0x7

static bool8 SelectApricornGrovePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u8 choices[ARRAY_COUNT(sApricornItems)];
    u8 i;

    (void)request;
    for(i = 0; i < ARRAY_COUNT(choices); ++i)
        choices[i] = i;
    for(i = 0; i < ROGUE_APRICORN_CHOICE_COUNT; ++i)
    {
        u8 selected = i + RogueRouteSceneRng_Next(rng) % (ARRAY_COUNT(choices) - i);
        u8 temp = choices[i];

        choices[i] = choices[selected];
        choices[selected] = temp;
    }

    *payload = choices[0]
        | ((u32)choices[1] << APRICORN_PAYLOAD_CHOICE_BITS)
        | ((u32)choices[2] << (APRICORN_PAYLOAD_CHOICE_BITS * 2));
    return TRUE;
}

static void ExpandApricornGrovePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    if(request->recipeId == ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN
        && request->lotRole == 1)
    {
        u8 questId = RogueAdventureQuests_FindByDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
        const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);

        request->primaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
        request->secondaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
        request->rewardAmount = ROGUE_APRICORN_BALL_REWARD_COUNT;
        if(quest != NULL)
        {
            request->requestedItem = quest->payload[0];
            request->rewardItem = quest->payload[1];
        }
        return;
    }

    request->primaryGraphicsId = OBJ_EVENT_GFX_ROUTE_PROP_APRICORN_TREE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN;
    request->requestedItem = sApricornItems[payload & APRICORN_PAYLOAD_CHOICE_MASK];
    request->rewardItem = sApricornItems[(payload >> APRICORN_PAYLOAD_CHOICE_BITS) & APRICORN_PAYLOAD_CHOICE_MASK];
    request->trainerNum = sApricornItems[(payload >> (APRICORN_PAYLOAD_CHOICE_BITS * 2)) & APRICORN_PAYLOAD_CHOICE_MASK];
}

#define UNBOUND_TUTOR_PAYLOAD_SEED_MASK 0xFFFF
#define UNBOUND_TUTOR_PAYLOAD_TIER_SHIFT 16
#define UNBOUND_TUTOR_PAYLOAD_TIER_MASK 0x3

static void GenerateUnboundTutorMoves(u32 payload, u16 moves[UNBOUND_TUTOR_ROLE_COUNT])
{
    struct RogueRouteSceneRng rng;
    u8 tier = (payload >> UNBOUND_TUTOR_PAYLOAD_TIER_SHIFT) & UNBOUND_TUTOR_PAYLOAD_TIER_MASK;
    u8 role;

    if(tier >= UNBOUND_TUTOR_TIER_COUNT)
    {
        memset(moves, 0, sizeof(u16) * UNBOUND_TUTOR_ROLE_COUNT);
        return;
    }

    RogueRouteSceneRng_Seed(&rng, (payload & UNBOUND_TUTOR_PAYLOAD_SEED_MASK) ^ 0xA17E);
    for(role = 0; role < UNBOUND_TUTOR_ROLE_COUNT; ++role)
    {
        const struct RogueUnboundTutorMovePool *pool = &sUnboundTutorMovePools[tier][role];

        moves[role] = pool->moves[RogueRouteSceneRng_Next(&rng) % pool->count];
    }
}

static bool8 IsUnboundTutorOfferUseful(const u16 moves[UNBOUND_TUTOR_ROLE_COUNT])
{
    u8 moveIdx;

    for(moveIdx = 0; moveIdx < UNBOUND_TUTOR_ROLE_COUNT; ++moveIdx)
    {
        bool8 hasRecipient = FALSE;
        u8 partyIdx;

        for(partyIdx = 0; partyIdx < gPlayerPartyCount; ++partyIdx)
        {
            struct Pokemon *mon = &gPlayerParty[partyIdx];

            if(!GetMonData(mon, MON_DATA_IS_EGG) && !MonKnowsMove(mon, moves[moveIdx]))
            {
                hasRecipient = TRUE;
                break;
            }
        }

        if(!hasRecipient)
            return FALSE;
    }

    return TRUE;
}

static bool8 SelectUnboundTutorPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u8 tier = min(Rogue_GetCurrentDifficulty() / 4, UNBOUND_TUTOR_TIER_LATE);
    u8 attempt;

    (void)request;
    for(attempt = 0; attempt < 16; ++attempt)
    {
        u16 moves[UNBOUND_TUTOR_ROLE_COUNT];

        *payload = RogueRouteSceneRng_Next(rng)
            | ((u32)tier << UNBOUND_TUTOR_PAYLOAD_TIER_SHIFT);
        GenerateUnboundTutorMoves(*payload, moves);
        if(IsUnboundTutorOfferUseful(moves))
            return TRUE;
    }

    return FALSE;
}

static void ExpandUnboundTutorPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    u16 moves[UNBOUND_TUTOR_ROLE_COUNT];

    GenerateUnboundTutorMoves(payload, moves);
    request->primaryGraphicsId = OBJ_EVENT_GFX_MISC_NPC_TUTOR;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MISC_NPC_TUTOR;
    request->requestedItem = moves[0];
    request->rewardItem = moves[1];
    request->trainerNum = moves[2];
}

static bool8 SelectCampCookPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    (void)request;
    *payload = RogueRouteSceneRng_Next(rng);
    return TRUE;
}

static u16 SelectCampCookBerryReward(struct RogueRouteSceneRng *rng)
{
    u8 attempts;

    for(attempts = 0; attempts < 8; ++attempts)
    {
        u16 item;
        u8 roll = RogueRouteSceneRng_Next(rng) % 3;

        if(roll == 0)
            item = ITEM_LUM_BERRY;
        else if(roll == 1)
            item = ITEM_SITRUS_BERRY;
        else
            item = sCampCookTypedResistBerries[RogueRouteSceneRng_Next(rng) % ARRAY_COUNT(sCampCookTypedResistBerries)];

        if(Rogue_IsItemEnabled(item))
            return item;
    }

    return ITEM_NONE;
}

static void ExpandCampCookPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_COOK;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_DECOR_CAULDRON;
    request->rewardAmount = payload;
}

static bool8 SelectMysteryEggCourierPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u8 encounterCount = Rogue_GetCurrentWildEncounterCount();
    bool8 partyHasRoom = CalculatePlayerPartyCount() < Rogue_GetMaxPartySize();
    u8 startIndex;
    u8 offset;

    (void)request;
    if(encounterCount == 0)
        return FALSE;

    startIndex = RogueRouteSceneRng_Next(rng) % encounterCount;
    for(offset = 0; offset < encounterCount; ++offset)
    {
        u16 species = Rogue_GetCurrentWildEncounterSpecies((startIndex + offset) % encounterCount);
        u16 eggSpecies = Rogue_GetEggSpecies(species);

        if(CanUseMysteryEggCourierSpecies(eggSpecies, partyHasRoom))
        {
            *payload = eggSpecies;
            return TRUE;
        }
    }

    return FALSE;
}

static void ExpandMysteryEggCourierPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_OLD_MAN_2;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_BIRCHS_BAG;
    request->requestedItem = payload;
    request->rewardItem = ITEM_ESCAPE_ROPE;
}

static const u8 sTravelingMerchantShopCategories[] =
{
    ROGUE_SHOP_GENERAL,
    ROGUE_SHOP_BALLS,
    ROGUE_SHOP_TMS,
    ROGUE_SHOP_BATTLE_ENHANCERS,
    ROGUE_SHOP_HELD_ITEMS,
#ifdef ROGUE_EXPANSION
    ROGUE_SHOP_RARE_HELD_ITEMS,
#endif
};

static bool8 SelectTravelingMerchantPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    (void)request;
    *payload = sTravelingMerchantShopCategories[
        RogueRouteSceneRng_Next(rng) % ARRAY_COUNT(sTravelingMerchantShopCategories)];
    return TRUE;
}

static void ExpandTravelingMerchantPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    request->primaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MART_EMPLOYEE;
    request->rewardAmount = ROGUE_SHOP_FLAG_TRAVELING_MERCHANT | payload;
}

struct TideSalvageReward
{
    u16 itemId;
    u8 count;
};

static const struct TideSalvageReward sTideSalvageRewards[] =
{
    {ITEM_PEARL, 2},
    {ITEM_BIG_PEARL, 1},
    {ITEM_STARDUST, 2},
    {ITEM_HEART_SCALE, 2},
    {ITEM_DIVE_BALL, 5},
};

static bool8 SelectTideSalvagePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u8 eligible[ARRAY_COUNT(sTideSalvageRewards)];
    u8 count = 0;
    u8 i;

    (void)request;
    for(i = 0; i < ARRAY_COUNT(sTideSalvageRewards); ++i)
    {
        if(Rogue_IsItemEnabled(sTideSalvageRewards[i].itemId))
            eligible[count++] = i;
    }

    if(count == 0)
        return FALSE;

    *payload = eligible[RogueRouteSceneRng_Next(rng) % count];
    return TRUE;
}

static void ExpandTideSalvagePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    if(payload >= ARRAY_COUNT(sTideSalvageRewards))
        return;

    request->primaryGraphicsId = OBJ_EVENT_GFX_SWIMMER_M;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_SWIMMER_M;
    request->rewardItem = sTideSalvageRewards[payload].itemId;
    request->rewardAmount = sTideSalvageRewards[payload].count;
}

static bool8 SelectFieldRepairBenchPayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u16 groupCount = RogueGift_GetStandardAbilityGroupCount();
    u16 startIndex;
    u16 offset;

    (void)request;
    if(groupCount == 0)
        return FALSE;

    startIndex = RogueRouteSceneRng_Next(rng) % groupCount;
    for(offset = 0; offset < groupCount; ++offset)
    {
        u16 ability = RogueGift_GetStandardAbilityByGroupIndex((startIndex + offset) % groupCount);

        ability = RogueGift_GetStandardAbilityFlavor(ability, RogueRouteSceneRng_Next(rng));
        if(ability != ABILITY_NONE && RogueGift_IsStandardAbilityEligible(ability))
        {
            *payload = ability;
            return TRUE;
        }
    }

    return FALSE;
}

static void ExpandFieldRepairBenchPayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    if(payload == ABILITY_NONE || payload >= ABILITIES_COUNT)
        return;

    request->primaryGraphicsId = OBJ_EVENT_GFX_DEVON_EMPLOYEE;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_WORK_TABLE;
    request->rewardItem = payload;
    request->trainerNum = TRAINER_NONE;
}

static bool8 SelectBreedersExchangePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    u8 encounterCount = Rogue_GetCurrentWildEncounterCount();
    u16 seed = RogueRouteSceneRng_Next(rng) & BREEDERS_EXCHANGE_PAYLOAD_SEED_MASK;
    u8 startIndex;
    u8 offset;

    (void)request;
    if(encounterCount == 0)
        return FALSE;

    startIndex = RogueRouteSceneRng_Next(rng) % encounterCount;
    for(offset = 0; offset < encounterCount; ++offset)
    {
        u16 requestedSpecies = Rogue_GetCurrentWildEncounterSpecies((startIndex + offset) % encounterCount);

        if(requestedSpecies <= BREEDERS_EXCHANGE_PAYLOAD_SPECIES_MASK
            && SelectBreedersExchangeOffer(requestedSpecies, seed) != SPECIES_NONE)
        {
            *payload = requestedSpecies | ((u32)seed << BREEDERS_EXCHANGE_PAYLOAD_SEED_SHIFT);
            return TRUE;
        }
    }

    return FALSE;
}

static void ExpandBreedersExchangePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    u16 requestedSpecies = payload & BREEDERS_EXCHANGE_PAYLOAD_SPECIES_MASK;
    u16 seed = (payload >> BREEDERS_EXCHANGE_PAYLOAD_SEED_SHIFT) & BREEDERS_EXCHANGE_PAYLOAD_SEED_MASK;
    u16 offeredSpecies = SelectBreedersExchangeOffer(requestedSpecies, seed);

    request->primaryGraphicsId = OBJ_EVENT_GFX_GIRL_1;
    request->secondaryGraphicsId = OBJ_EVENT_GFX_FOLLOW_MON_1;
    request->requestedItem = requestedSpecies;
    request->rewardItem = offeredSpecies;
    request->rewardAmount = seed;
    if(offeredSpecies != SPECIES_NONE)
        request->trainerNum = seed % gRoguePokemonProfiles[offeredSpecies].competitiveSetCount;
}

enum
{
    BURIED_CACHE_TRAIT_LANDMARK,
    BURIED_CACHE_TRAIT_MARKING,
    BURIED_CACHE_TRAIT_GROUND,
    BURIED_CACHE_TRAIT_COUNT,
};

struct BuriedCacheSiteData
{
    u8 landmark;
    u8 marking;
    u8 ground;
};

struct BuriedCacheData
{
    struct BuriedCacheSiteData sites[2];
    u16 rewardItem;
    u16 secondaryRewardItem;
    u16 ambushSpecies;
    u16 moneyReward;
    u8 correctSite;
    u8 clueTraitA;
    u8 clueTraitB;
    u8 cacheType;
};

static const u16 sBuriedCacheEvolutionItems[] =
{
    ITEM_FIRE_STONE,
    ITEM_WATER_STONE,
    ITEM_THUNDER_STONE,
    ITEM_LEAF_STONE,
    ITEM_ICE_STONE,
    ITEM_SUN_STONE,
    ITEM_MOON_STONE,
    ITEM_SHINY_STONE,
    ITEM_DUSK_STONE,
    ITEM_DAWN_STONE,
};

static const u16 sBuriedCacheTmMoves[] =
{
    MOVE_PSYCHIC,
    MOVE_EARTHQUAKE,
    MOVE_ICE_BEAM,
    MOVE_THUNDERBOLT,
    MOVE_FLAMETHROWER,
    MOVE_SHADOW_BALL,
    MOVE_SLUDGE_BOMB,
    MOVE_BRICK_BREAK,
    MOVE_AERIAL_ACE,
};

#define ROGUE_BURIED_CACHE_AMBUSH_SPECIES_PER_ENVIRONMENT 6

static const u16 sBuriedCacheAmbushSpecies[ROGUE_ROUTE_ENVIRONMENT_COUNT][ROGUE_BURIED_CACHE_AMBUSH_SPECIES_PER_ENVIRONMENT] =
{
    [ROGUE_ROUTE_ENVIRONMENT_FIELD] =
    {
        SPECIES_SANDSHREW,
        SPECIES_DIGLETT,
        SPECIES_TRAPINCH,
        SPECIES_BALTOY,
        SPECIES_DUNSPARCE,
        SPECIES_NINCADA,
    },
    [ROGUE_ROUTE_ENVIRONMENT_FOREST] =
    {
        SPECIES_NINCADA,
        SPECIES_PARAS,
        SPECIES_SHROOMISH,
        SPECIES_PINECO,
        SPECIES_SPINARAK,
        SPECIES_DUNSPARCE,
    },
    [ROGUE_ROUTE_ENVIRONMENT_CAVE] =
    {
        SPECIES_GEODUDE,
        SPECIES_ONIX,
        SPECIES_DUNSPARCE,
        SPECIES_ZUBAT,
        SPECIES_WHISMUR,
        SPECIES_MAWILE,
    },
    [ROGUE_ROUTE_ENVIRONMENT_MOUNTAIN] =
    {
        SPECIES_GEODUDE,
        SPECIES_NOSEPASS,
        SPECIES_ARON,
        SPECIES_MACHOP,
        SPECIES_SANDSHREW,
        SPECIES_TRAPINCH,
    },
    [ROGUE_ROUTE_ENVIRONMENT_WATERFRONT] =
    {
        SPECIES_KRABBY,
        SPECIES_CORPHISH,
        SPECIES_CLAMPERL,
        SPECIES_WOOPER,
        SPECIES_LOTAD,
        SPECIES_PSYDUCK,
    },
    [ROGUE_ROUTE_ENVIRONMENT_URBAN] =
    {
        SPECIES_RATTATA,
        SPECIES_MEOWTH,
        SPECIES_ZIGZAGOON,
        SPECIES_VOLTORB,
        SPECIES_GRIMER,
        SPECIES_KOFFING,
    },
};

static u16 SelectEnabledItemFromList(
    const u16 *items,
    u16 itemCount,
    struct RogueRouteSceneRng *rng)
{
    u16 eligibleCount = 0;
    u16 selected;
    u16 i;

    for(i = 0; i < itemCount; ++i)
    {
        if(Rogue_IsItemEnabled(items[i]))
            ++eligibleCount;
    }
    if(eligibleCount == 0)
        return ITEM_NONE;

    selected = RogueRouteSceneRng_Next(rng) % eligibleCount;
    for(i = 0; i < itemCount; ++i)
    {
        if(Rogue_IsItemEnabled(items[i]) && selected-- == 0)
            return items[i];
    }

    return ITEM_NONE;
}

static u16 SelectBuriedCacheTm(struct RogueRouteSceneRng *rng)
{
    u16 items[ARRAY_COUNT(sBuriedCacheTmMoves)];
    u8 count = 0;
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sBuriedCacheTmMoves); ++i)
    {
        u16 item = BattleMoveIdToItemId(sBuriedCacheTmMoves[i]);

        if(item != ITEM_NONE && Rogue_IsItemEnabled(item))
            items[count++] = item;
    }

    return count == 0 ? ITEM_NONE : items[RogueRouteSceneRng_Next(rng) % count];
}

static u16 SelectBuriedCacheRelic(struct RogueRouteSceneRng *rng)
{
#ifdef ROGUE_EXPANSION
    u16 eligibleCount = 0;
    u16 selected;
    u16 item;

    for(item = ITEM_VENUSAURITE; item < ITEMS_COUNT; ++item)
    {
        bool8 isMegaStone = IS_MEGA_STONE_ITEM(item) && IsMegaEvolutionEnabled();
        bool8 isZCrystal = item >= ITEM_NORMALIUM_Z
            && item <= ITEM_ULTRANECROZIUM_Z
            && IsZMovesEnabled();

        if((isMegaStone || isZCrystal) && Rogue_IsItemEnabled(item))
            ++eligibleCount;
    }
    if(eligibleCount == 0)
        return ITEM_NONE;

    selected = RogueRouteSceneRng_Next(rng) % eligibleCount;
    for(item = ITEM_VENUSAURITE; item < ITEMS_COUNT; ++item)
    {
        bool8 isMegaStone = IS_MEGA_STONE_ITEM(item) && IsMegaEvolutionEnabled();
        bool8 isZCrystal = item >= ITEM_NORMALIUM_Z
            && item <= ITEM_ULTRANECROZIUM_Z
            && IsZMovesEnabled();

        if((isMegaStone || isZCrystal)
            && Rogue_IsItemEnabled(item)
            && selected-- == 0)
            return item;
    }
#else
    (void)rng;
#endif

    return ITEM_NONE;
}

static u16 SelectBuriedCacheCharm(struct RogueRouteSceneRng *rng)
{
    u16 eligibleCount = 0;
    u16 selected;
    u16 item;

    for(item = FIRST_ITEM_CHARM; item <= LAST_ITEM_CHARM; ++item)
    {
        if(Rogue_IsItemEnabled(item))
            ++eligibleCount;
    }
    if(eligibleCount == 0)
        return ITEM_NONE;

    selected = RogueRouteSceneRng_Next(rng) % eligibleCount;
    for(item = FIRST_ITEM_CHARM; item <= LAST_ITEM_CHARM; ++item)
    {
        if(Rogue_IsItemEnabled(item) && selected-- == 0)
            return item;
    }

    return ITEM_NONE;
}

static u16 SelectBuriedCacheAmbush(u8 environment, struct RogueRouteSceneRng *rng)
{
    u16 eligible[ROGUE_BURIED_CACHE_AMBUSH_SPECIES_PER_ENVIRONMENT];
    u8 count = 0;
    u8 i;

    if(environment >= ROGUE_ROUTE_ENVIRONMENT_COUNT)
        environment = ROGUE_ROUTE_ENVIRONMENT_FIELD;
    for(i = 0; i < ARRAY_COUNT(eligible); ++i)
    {
        u16 species = sBuriedCacheAmbushSpecies[environment][i];

        if(RoguePokedex_GetSpeciesCurrentNum(species) != 0)
            eligible[count++] = species;
    }

    return count == 0 ? SPECIES_NONE : eligible[RogueRouteSceneRng_Next(rng) % count];
}

static void GenerateBuriedCacheData(u16 seed, u8 environment, struct BuriedCacheData *data)
{
    struct RogueRouteSceneRng rng;
    struct BuriedCacheSiteData *correct;
    struct BuriedCacheSiteData *decoy;
    u8 sharedTrait;
    u8 rewardRoll;

    memset(data, 0, sizeof(*data));
    RogueRouteSceneRng_Seed(&rng, seed ^ 0xD16A);
    data->correctSite = RogueRouteSceneRng_Next(&rng) % 2;
    correct = &data->sites[data->correctSite];
    decoy = &data->sites[data->correctSite ^ 1];

    correct->landmark = RogueRouteSceneRng_Next(&rng) % 3;
    correct->marking = RogueRouteSceneRng_Next(&rng) % 4;
    correct->ground = RogueRouteSceneRng_Next(&rng) % 3;
    decoy->landmark = (correct->landmark + 1 + RogueRouteSceneRng_Next(&rng) % 2) % 3;
    decoy->marking = (correct->marking + 1 + RogueRouteSceneRng_Next(&rng) % 3) % 4;
    decoy->ground = (correct->ground + 1 + RogueRouteSceneRng_Next(&rng) % 2) % 3;

    data->clueTraitA = RogueRouteSceneRng_Next(&rng) % BURIED_CACHE_TRAIT_COUNT;
    data->clueTraitB = (data->clueTraitA + 1 + RogueRouteSceneRng_Next(&rng) % 2)
        % BURIED_CACHE_TRAIT_COUNT;
    if(data->clueTraitA > data->clueTraitB)
    {
        u8 temp = data->clueTraitA;
        data->clueTraitA = data->clueTraitB;
        data->clueTraitB = temp;
    }
    sharedTrait = RogueRouteSceneRng_Next(&rng) % 2 == 0
        ? data->clueTraitA
        : data->clueTraitB;
    if(sharedTrait == BURIED_CACHE_TRAIT_LANDMARK)
        decoy->landmark = correct->landmark;
    else if(sharedTrait == BURIED_CACHE_TRAIT_MARKING)
        decoy->marking = correct->marking;
    else
        decoy->ground = correct->ground;

    rewardRoll = RogueRouteSceneRng_Next(&rng) % 100;
    data->cacheType = rewardRoll < 35 ? ROGUE_BURIED_CACHE_ANCIENT
        : rewardRoll < 65 ? ROGUE_BURIED_CACHE_TRAINER
        : rewardRoll < 90 ? ROGUE_BURIED_CACHE_RELIC
        : ROGUE_BURIED_CACHE_JACKPOT;
    switch(data->cacheType)
    {
    case ROGUE_BURIED_CACHE_TRAINER:
        data->rewardItem = SelectBuriedCacheTm(&rng);
        data->secondaryRewardItem = ITEM_PP_MAX;
        if(data->rewardItem != ITEM_NONE)
            break;
        data->cacheType = ROGUE_BURIED_CACHE_ANCIENT;
        // fall through
    case ROGUE_BURIED_CACHE_ANCIENT:
        data->rewardItem = SelectEnabledItemFromList(
            sBuriedCacheEvolutionItems,
            ARRAY_COUNT(sBuriedCacheEvolutionItems),
            &rng);
        data->moneyReward = min(
            ROGUE_BURIED_CACHE_MONEY_MAX,
            ROGUE_BURIED_CACHE_MONEY_BASE
                + ROGUE_BURIED_CACHE_MONEY_PER_DIFFICULTY * Rogue_GetCurrentDifficulty());
        break;
    case ROGUE_BURIED_CACHE_RELIC:
        data->rewardItem = SelectBuriedCacheRelic(&rng);
        if(data->rewardItem != ITEM_NONE)
            break;
        data->cacheType = ROGUE_BURIED_CACHE_ANCIENT;
        data->rewardItem = SelectEnabledItemFromList(
            sBuriedCacheEvolutionItems,
            ARRAY_COUNT(sBuriedCacheEvolutionItems),
            &rng);
        data->moneyReward = min(
            ROGUE_BURIED_CACHE_MONEY_MAX,
            ROGUE_BURIED_CACHE_MONEY_BASE
                + ROGUE_BURIED_CACHE_MONEY_PER_DIFFICULTY * Rogue_GetCurrentDifficulty());
        break;
    case ROGUE_BURIED_CACHE_JACKPOT:
        data->rewardItem = SelectBuriedCacheCharm(&rng);
        if(data->rewardItem == ITEM_NONE)
        {
            data->cacheType = ROGUE_BURIED_CACHE_ANCIENT;
            data->rewardItem = SelectEnabledItemFromList(
                sBuriedCacheEvolutionItems,
                ARRAY_COUNT(sBuriedCacheEvolutionItems),
                &rng);
            data->moneyReward = min(
                ROGUE_BURIED_CACHE_MONEY_MAX,
                ROGUE_BURIED_CACHE_MONEY_BASE
                    + ROGUE_BURIED_CACHE_MONEY_PER_DIFFICULTY * Rogue_GetCurrentDifficulty());
        }
        break;
    }

    data->ambushSpecies = SelectBuriedCacheAmbush(environment, &rng);
}

static bool8 SelectBuriedCachePayload(const struct RogueRouteSceneRequest *request, struct RogueRouteSceneRng *rng, u32 *payload)
{
    struct BuriedCacheData data;
    u16 seed = RogueRouteSceneRng_Next(rng);

    GenerateBuriedCacheData(seed, request->environment, &data);
    if(data.rewardItem == ITEM_NONE || data.ambushSpecies == SPECIES_NONE)
        return FALSE;

    *payload = seed;
    return TRUE;
}

static void ExpandBuriedCachePayload(struct RogueRouteSceneRequest *request, u32 payload)
{
    struct BuriedCacheData data;

    GenerateBuriedCacheData(payload, request->environment, &data);
    request->primaryGraphicsId = request->lotRole == 0
        ? OBJ_EVENT_GFX_MISC_RUIN_MANIAC
        : ROUTE_SCENE_GFX_SEMANTIC(data.sites[request->lotRole - 1].landmark
            + ROUTE_SCENE_SEMANTIC_PROP_LANDMARK_0);
    request->secondaryGraphicsId = OBJ_EVENT_GFX_MISC_RUIN_MANIAC;
    request->requestedItem = data.secondaryRewardItem;
    request->rewardItem = data.rewardItem;
    request->trainerNum = data.ambushSpecies;
    request->rewardAmount = payload;
}

#include "data/rogue_route_scene_recipes.h"
#include "data/rogue_route_event_definitions.h"

STATIC_ASSERT(ROGUE_ROUTE_FAMILY_COUNT <= ROGUE_ROUTE_FAMILY_HISTORY_PER_VAR * 2, RogueRouteEventFamilyHistoryCapacity);

const struct RogueRouteRecipeDefinition *RogueRouteEvents_GetRecipeDefinition(u8 recipeId)
{
    if(recipeId == ROGUE_ROUTE_SCENE_RECIPE_NONE
        || recipeId >= ROGUE_ROUTE_SCENE_RECIPE_COUNT
        || sRouteRecipes[recipeId].lotCount == 0)
        return NULL;

    return &sRouteRecipes[recipeId];
}

const struct RogueRouteFallbackDefinition *RogueRouteEvents_GetFallbackDefinition(u8 fallbackId)
{
    if(fallbackId >= ARRAY_COUNT(sRouteFallbacks))
        return NULL;

    return &sRouteFallbacks[fallbackId];
}

u8 RogueRouteEvents_GetFallbackCount(void)
{
    return ARRAY_COUNT(sRouteFallbacks);
}

static bool8 GetFamilyHistoryLocation(u8 familyId, bool8 completed, u16 *varId, u16 *bit)
{
    if(familyId >= ROGUE_ROUTE_FAMILY_COUNT || familyId >= ROGUE_ROUTE_FAMILY_HISTORY_PER_VAR * 2)
        return FALSE;

    *varId = familyId < ROGUE_ROUTE_FAMILY_HISTORY_PER_VAR
        ? VAR_ROGUE_ROUTE_EVENT_HISTORY
        : VAR_ROGUE_ROUTE_EVENT_HISTORY_2;
    familyId %= ROGUE_ROUTE_FAMILY_HISTORY_PER_VAR;
    *bit = 1 << (familyId + (completed ? ROGUE_ROUTE_FAMILY_HISTORY_COMPLETED_SHIFT : 0));
    return TRUE;
}

bool8 RogueRouteEvents_HasEncounteredFamily(u8 familyId)
{
    u16 varId;
    u16 bit;

    return GetFamilyHistoryLocation(familyId, FALSE, &varId, &bit)
        && (VarGet(varId) & bit) != 0;
}

bool8 RogueRouteEvents_HasCompletedFamily(u8 familyId)
{
    u16 varId;
    u16 bit;

    return GetFamilyHistoryLocation(familyId, TRUE, &varId, &bit)
        && (VarGet(varId) & bit) != 0;
}

static u8 GetSceneFamily(const struct RogueRouteSceneRequest *scene);

void RogueRouteEvents_MarkFamilyEncountered(u8 familyId)
{
    u16 varId;
    u16 bit;

    if(GetFamilyHistoryLocation(familyId, FALSE, &varId, &bit))
        VarSet(varId, VarGet(varId) | bit);
}

void RogueRouteEvents_MarkRecipeFamilyEncountered(u8 recipeId)
{
    struct RogueRouteSceneRequest scene = {.recipeId = recipeId};
    u8 familyId = GetSceneFamily(&scene);

    if(familyId < ROGUE_ROUTE_FAMILY_COUNT)
        RogueRouteEvents_MarkFamilyEncountered(familyId);
}

static u8 GetSceneFamily(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueRouteRecipeDefinition *sceneDefinition;
    u8 i;

    if(scene == NULL)
        return ROGUE_ROUTE_FAMILY_COUNT;

    sceneDefinition = RogueRouteEvents_GetRecipeDefinition(scene->recipeId);
    for(i = 0; i < ARRAY_COUNT(sRouteFallbacks); ++i)
    {
        const struct RogueRouteFallbackDefinition *fallback = &sRouteFallbacks[i];
        const struct RogueRouteRecipeDefinition *fallbackDefinition =
            RogueRouteEvents_GetRecipeDefinition(fallback->recipeId);

        if(fallback->recipeId == scene->recipeId
            || (sceneDefinition != NULL
                && fallbackDefinition != NULL
                && sceneDefinition->linkedQuestDefinitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_NONE
                && sceneDefinition->linkedQuestDefinitionId == fallbackDefinition->linkedQuestDefinitionId))
            return fallback->familyId;
    }

    return ROGUE_ROUTE_FAMILY_COUNT;
}

void RogueRouteEvents_MarkSceneFamilyCompleted(const struct RogueRouteSceneRequest *scene)
{
    u8 familyId = GetSceneFamily(scene);
    u16 varId;
    u16 bit;

    if(!GetFamilyHistoryLocation(familyId, TRUE, &varId, &bit))
        return;

    VarSet(varId, VarGet(varId) | bit);
}

void RogueRouteEvents_OnEnterScene(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueRouteRecipeDefinition *definition;
    const struct RogueAdventureQuest *quest;

    if(scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
        return;

    definition = RogueRouteEvents_GetRecipeDefinition(scene->recipeId);
    quest = RogueAdventureQuests_Get(scene->ownerQuestId);
    if(definition == NULL
        || quest == NULL
        || quest->definitionId != definition->linkedQuestDefinitionId)
        return;

    if(definition->resumeBehavior == ROUTE_SCENE_RESUME_REWARD_PENDING_IF_PROGRESS
        && quest->progress != 0
        && RogueRouteScenes_GetState(scene->sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        RogueRouteScenes_SetState(scene->sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    else if(definition->resumeBehavior == ROUTE_SCENE_RESUME_COMPLETED_IF_TARGET_MET
        && RogueAdventureQuests_IsProgressTargetMet(scene->ownerQuestId))
        RogueRouteScenes_SetState(scene->sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
}

void RogueRouteEvents_PrepareSceneTrainers(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueRouteRecipeDefinition *definition = RogueRouteEvents_GetRecipeDefinition(scene->recipeId);
    u8 trainerIdx;

    if(definition == NULL
        || (definition->flags & ROUTE_SCENE_RECIPE_FLAG_EXCLUDE_DYNAMIC_TRAINER) == 0)
        return;

    for(trainerIdx = 0; trainerIdx < ROGUE_MAX_ACTIVE_TRAINER_COUNT; ++trainerIdx)
    {
        if(Rogue_GetDynamicTrainer(trainerIdx) == scene->trainerNum)
            Rogue_SetDynamicTrainer(trainerIdx, TRAINER_NONE);
    }
}

u8 RogueRouteEvents_OnExitScene(const struct RogueRouteSceneRequest *scene)
{
    const struct RogueRouteRecipeDefinition *definition = RogueRouteEvents_GetRecipeDefinition(scene->recipeId);

    if(scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE && scene->lotRole == 0)
        RemoveBagItem(ITEM_FIELD_SHOVEL, 1);

    if(definition != NULL
        && (definition->flags & ROUTE_SCENE_RECIPE_FLAG_COMPLETE_LINKED_QUEST_ON_EXIT) != 0
        && scene->lotRole == definition->completionLotRole
        && RogueRouteScenes_GetState(scene->sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return RogueAdventureQuests_FindByDefinition(definition->linkedQuestDefinitionId);

    return ROGUE_ADVENTURE_QUEST_INVALID_ID;
}

void RogueRouteEvents_GetInteractionData(void)
{
    struct RogueRouteSceneRequest scene;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_STATE_NOT_STARTED;
    gSpecialVar_0x8004 = ITEM_NONE;
    gSpecialVar_0x8005 = ITEM_NONE;
    gSpecialVar_0x8006 = TRAINER_NONE;
    gSpecialVar_0x8007 = 0;
    gSpecialVar_0x8008 = ROGUE_ROUTE_EVENT_STATE_NOT_STARTED;
    gSpecialVar_0x8009 = 0;

    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;

    gSpecialVar_Result = RogueRouteScenes_GetState(scene.sceneSlot);
    gSpecialVar_0x8004 = scene.rewardItem;
    gSpecialVar_0x8005 = scene.requestedItem;
    gSpecialVar_0x8006 = scene.trainerNum;
    gSpecialVar_0x8007 = scene.rewardAmount;
    gSpecialVar_0x8008 = gSpecialVar_Result;
    gSpecialVar_0x8009 = scene.lotRole;
}

void RogueRouteEvents_TryAcceptStolenTradeCaseQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || FlagGet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED)
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE)
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    params.payload[1] = scene.trainerNum;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_STOLEN_TRADE_CASE, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return;

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_BeginStolenTradeCaseBattle(void)
{
    struct RogueRouteSceneRequest scene;
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED && state != ROGUE_ROUTE_EVENT_STATE_ACTIVE))
        return;

    VarSet(VAR_ROGUE_SPECIAL_ENCOUNTER_DATA, scene.trainerNum);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishStolenTradeCaseBattle(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};
    bool8 alreadyHasCase;
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_CAMP
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_ACTIVE && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    if(state == ROGUE_ROUTE_EVENT_STATE_ACTIVE
        && !RogueAdventureQuests_EmitSignalForQuest(
            scene.ownerQuestId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
            1))
        return;

    alreadyHasCase = CheckBagHasItem(scene.requestedItem, 1);
    transaction.rewards[0].itemId = scene.requestedItem;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    if(!alreadyHasCase)
    {
        gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
        if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        {
            RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
            return;
        }
    }

    if(!alreadyHasCase)
        Rogue_PushPopup_AddItem(scene.requestedItem, 1);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryClaimStolenTradeCaseReward(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};
    u16 state;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;
    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if(scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_STOLEN_TRADE_CASE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || (state != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
            && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING))
        return;

    transaction.costs[0].itemId = scene.requestedItem;
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = scene.rewardItem;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    transaction.flags = ROGUE_EVENT_TRANSACTION_FLAG_ALLOW_COST_SLOTS_FOR_REWARDS;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        return;
    }

    FlagSet(FLAG_ROGUE_STOLEN_TRADE_CASE_COMPLETED);
    Rogue_PushPopup_AddItem(scene.rewardItem, 1);
    Rogue_PushPopup_AddMoney(ROGUE_STOLEN_TRADE_CASE_REWARD_MONEY);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptHexedShrine(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_HEXED_SHRINE
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || !Rogue_CanActivateTemporaryDarkDealCurse(scene.requestedItem))
        return;

    transaction.rewards[0].itemId = scene.requestedItem;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = scene.rewardAmount;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    if(!Rogue_TryActivateTemporaryDarkDealCurse(scene.requestedItem))
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    Rogue_PushPopup_AddMoney(scene.rewardAmount);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    VarSet(
        VAR_ROGUE_ROUTE_EVENT_STATE,
        VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) | ROUTE_SCENE_HEXED_SHRINE_ACCEPTED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptAnomalousFossilQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueEventTransaction transaction = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL)
        || RogueAdventureQuests_GetFossilSpecies(scene.requestedItem) != scene.rewardItem)
        return;

    transaction.rewards[0].itemId = scene.requestedItem;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    params.payload[0] = scene.requestedItem;
    params.payload[1] = scene.rewardAmount;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_ANOMALOUS_FOSSIL, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    Rogue_PushPopup_AddItem(scene.requestedItem, 1);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u32 GenerateFossilCustomMonId(const struct RogueRouteSceneRequest *scene, u8 restoration)
{
    RAND_TYPE originalRng = gRngValue;
    u32 customMonId;

    SeedRng(scene->rewardAmount ^ (restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0x51A7 : 0xB4E3));
    customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(
        UNIQUE_RARITY_RARE,
        scene->rewardItem,
        restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0 : 100);
    gRngValue = originalRng;
    return customMonId;
}

static void BufferFossilCustomTyping(u8 *dest, u16 species, u32 customMonId)
{
    static const u8 sText_TypeSeparator[] = _("/");
    u8 type1 = RogueGift_GetCustomMonType(customMonId, 0);
    u8 type2 = RogueGift_GetCustomMonType(customMonId, 1);

    if(!IS_STANDARD_TYPE(type1))
        type1 = RoguePokedex_GetSpeciesType(species, 0);
    if(!IS_STANDARD_TYPE(type2))
        type2 = RoguePokedex_GetSpeciesType(species, 1);

    StringCopy(dest, gTypeNames[type1]);
    if(type2 != type1)
    {
        StringAppend(dest, sText_TypeSeparator);
        StringAppend(dest, gTypeNames[type2]);
    }
}

void RogueRouteEvents_BufferFossilRestorationData(void)
{
    struct RogueRouteSceneRequest scene;

    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    gStringVar3[0] = EOS;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE)
        return;

    BufferFossilCustomTyping(
        gStringVar1,
        scene.rewardItem,
        GenerateFossilCustomMonId(&scene, ROGUE_FOSSIL_RESTORATION_STABLE));
    BufferFossilCustomTyping(
        gStringVar2,
        scene.rewardItem,
        GenerateFossilCustomMonId(&scene, ROGUE_FOSSIL_RESTORATION_ADAPTIVE));
    StringCopy(gStringVar3, RoguePokedex_GetSpeciesName(scene.rewardItem));
}

void RogueRouteEvents_TryRestoreAnomalousFossil(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};
    struct Pokemon mon;
    RAND_TYPE originalRng;
    bool8 gaveMon;
    u32 customMonId;
    u8 restoration = gSpecialVar_0x8004;
    u8 replacementSlot = gSpecialVar_0x8006;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_ANOMALOUS_FOSSIL_RESTORATION
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || restoration > ROGUE_FOSSIL_RESTORATION_ADAPTIVE)
        return;

    if(CalculatePlayerPartyCount() >= Rogue_GetMaxPartySize()
        && replacementSlot == PARTY_NOTHING_CHOSEN)
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_PARTY_FULL;
        return;
    }

    originalRng = gRngValue;
    SeedRng(scene.rewardAmount ^ (restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0x51A7 : 0xB4E3));
    customMonId = RogueGift_CreateDynamicMonIdRawWithTypingChance(
        UNIQUE_RARITY_RARE,
        scene.rewardItem,
        restoration == ROGUE_FOSSIL_RESTORATION_STABLE ? 0 : 100);
    RogueGift_CreateMon(customMonId, &mon, scene.rewardItem, 1, USE_RANDOM_IVS);
    gRngValue = originalRng;

    transaction.costs[0].itemId = scene.requestedItem;
    transaction.costs[0].count = 1;
    transaction.costCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    gaveMon = RogueGift_TryGiveMonToParty(&mon, replacementSlot);
    gRngValue = originalRng;
    if(!gaveMon)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        return;
    }

    GetSetPokedexSpeciesFlag(scene.rewardItem, FLAG_SET_CAUGHT);
    Rogue_PushPopup_AddPokemon(scene.rewardItem, TRUE, FALSE);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u8 CountForbiddenStoneSouls(u8 progress)
{
    u8 count = 0;
    u8 i;

    for(i = 0; i < ROGUE_FORBIDDEN_STONE_SOUL_COUNT; ++i)
    {
        if((progress & (1 << i)) != 0)
            ++count;
    }

    return count;
}

void RogueRouteEvents_TryAcceptForbiddenStoneQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueEventTransaction transaction = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_OFFER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE))
        return;

    transaction.rewards[0].itemId = ITEM_ODD_KEYSTONE;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    params.payload[0] = scene.rewardAmount;
    params.target = ROGUE_FORBIDDEN_STONE_SOUL_COUNT;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    Rogue_PushPopup_AddItem(ITEM_ODD_KEYSTONE, 1);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_CollectForbiddenStoneSoul(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    u8 progress;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    gSpecialVar_0x8007 = 0;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_SOULS
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || scene.lotRole >= ROGUE_FORBIDDEN_STONE_SOUL_COUNT
        || !CheckBagHasItem(ITEM_ODD_KEYSTONE, 1))
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE
        || quest->nodeId != 0)
        return;

    if(!RogueAdventureQuests_EmitSignalForQuest(
        scene.ownerQuestId,
        ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
        1 << scene.lotRole))
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL)
        return;
    progress = quest->progress;
    gSpecialVar_0x8007 = CountForbiddenStoneSouls(progress);
    if(gSpecialVar_0x8007 == ROGUE_FORBIDDEN_STONE_SOUL_COUNT)
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    else
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_PrepareForbiddenStoneBattle(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    struct RoguePokemonCompetitiveSetRules rules = {0};
    RAND_TYPE originalRng;
    u16 presetCount;
    u32 temp;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || !CheckBagHasItem(ITEM_ODD_KEYSTONE, 1))
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE
        || quest->nodeId != 1
        || quest->progress != 0)
        return;

    originalRng = gRngValue;
    SeedRng(scene.trainerNum ^ 0x108);
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], SPECIES_SPIRITOMB, Rogue_CalculateBossMonLvl(), USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    presetCount = gRoguePokemonProfiles[SPECIES_SPIRITOMB].competitiveSetCount;
    if(presetCount != 0)
    {
        const struct RoguePokemonCompetitiveSet *preset = &gRoguePokemonProfiles[SPECIES_SPIRITOMB].competitiveSets[Random() % presetCount];

        Rogue_ApplyMonCompetitiveSet(&gEnemyParty[0], Rogue_CalculateBossMonLvl(), preset, &rules);
    }
    temp = FALSE;
    SetMonData(&gEnemyParty[0], MON_DATA_IS_SHINY, &temp);
    CalculateMonStats(&gEnemyParty[0]);
    temp = GetMonData(&gEnemyParty[0], MON_DATA_MAX_HP);
    SetMonData(&gEnemyParty[0], MON_DATA_HP, &temp);
    gRngValue = originalRng;

    Rogue_ActivateUncatchableWildBattle();
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishForbiddenStoneBattle(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    struct RogueEventTransaction transaction = {0};

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FORBIDDEN_STONE_PAYOFF
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    quest = RogueAdventureQuests_Get(scene.ownerQuestId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_FORBIDDEN_STONE
        || quest->nodeId != 1)
        return;

    // Calling this after the battle records the win before attempting the
    // atomic payoff. A full Bag or wallet therefore never repeats the boss.
    if(quest->progress == 0
        && !RogueAdventureQuests_EmitSignalForQuest(
            scene.ownerQuestId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
            1))
        return;

    transaction.costs[0].itemId = ITEM_ODD_KEYSTONE;
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = ITEM_ABILITY_PATCH;
    transaction.rewards[0].count = 1;
    transaction.moneyReward = ROGUE_FORBIDDEN_STONE_REWARD_MONEY;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        return;
    }

    Rogue_PushPopup_AddItem(ITEM_ABILITY_PATCH, 1);
    Rogue_PushPopup_AddMoney(ROGUE_FORBIDDEN_STONE_REWARD_MONEY);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u16 GetApricornChoice(const struct RogueRouteSceneRequest *scene, u8 choice)
{
    switch(choice)
    {
    case 0:
        return scene->requestedItem;
    case 1:
        return scene->rewardItem;
    case 2:
        return scene->trainerNum;
    default:
        return ITEM_NONE;
    }
}

void RogueRouteEvents_BufferApricornTreeData(void)
{
    struct RogueRouteSceneRequest scene;
    u16 apricorn = ITEM_NONE;
    u16 ball = ITEM_NONE;

    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE
            && scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN)
        || scene.lotRole != 0)
        return;

    {
        u8 questId = RogueAdventureQuests_FindByDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
        const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);

        if(quest != NULL)
        {
            apricorn = quest->payload[0];
            ball = quest->payload[1];
        }
        else if(gSelectedObjectEvent < OBJECT_EVENTS_COUNT)
        {
            u8 choice = RogueRouteScenes_GetSelectedPropId();

            apricorn = GetApricornChoice(&scene, choice);
            ball = GetApricornBall(apricorn);
        }
    }

    if(apricorn != ITEM_NONE)
        CopyItemName(apricorn, gStringVar1);
    if(ball != ITEM_NONE)
        CopyItemName(ball, gStringVar2);
}

void RogueRouteEvents_TryChooseApricorn(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    struct RogueEventTransaction transaction = {0};
    u16 apricorn;
    u16 ball;
    u8 choice;
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE
            && scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN)
        || scene.lotRole != 0
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING)
        || gSelectedObjectEvent >= OBJECT_EVENTS_COUNT)
        return;

    choice = RogueRouteScenes_GetSelectedPropId();
    apricorn = GetApricornChoice(&scene, choice);
    ball = GetApricornBall(apricorn);
    if(apricorn == ITEM_NONE || ball == ITEM_NONE)
        return;

    transaction.rewards[0].itemId = apricorn;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    params.payload[0] = apricorn;
    params.payload[1] = ball;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
    {
        RogueEventTransaction_Rollback(&transaction);
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
        return;
    }

    Rogue_PushPopup_AddItem(apricorn, 1);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCraftApricornBalls(void)
{
    struct RogueRouteSceneRequest scene;
    const struct RogueAdventureQuest *quest;
    struct RogueEventTransaction transaction = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_ARTISAN
            && (scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_APRICORN_GROVE_AND_ARTISAN || scene.lotRole != 1))
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    questId = scene.source == ROGUE_ROUTE_SCENE_SOURCE_QUEST_NODE
        ? scene.ownerQuestId
        : RogueAdventureQuests_FindByDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING);
    quest = RogueAdventureQuests_Get(questId);
    if(quest == NULL
        || quest->definitionId != ROGUE_ADVENTURE_QUEST_DEFINITION_APRICORN_CRAFTING
        || GetApricornBall(quest->payload[0]) != quest->payload[1])
        return;

    transaction.costs[0].itemId = quest->payload[0];
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = quest->payload[1];
    transaction.rewards[0].count = ROGUE_APRICORN_BALL_REWARD_COUNT;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    Rogue_PushPopup_AddItem(quest->payload[1], ROGUE_APRICORN_BALL_REWARD_COUNT);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_PrepareUnboundTutor(void)
{
    struct RogueRouteSceneRequest scene;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    gSpecialVar_0x8007 = scene.requestedItem;
    gSpecialVar_0x8008 = scene.rewardItem;
    gSpecialVar_0x8009 = scene.trainerNum;
    StringCopy(gStringVar1, gMoveNames[gSpecialVar_0x8007]);
    StringCopy(gStringVar2, gMoveNames[gSpecialVar_0x8008]);
    StringCopy(gStringVar3, gMoveNames[gSpecialVar_0x8009]);
    TeachMoveSetContextUnbound();
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishUnboundTutor(void)
{
    struct RogueRouteSceneRequest scene;
    bool8 taughtMove = gSpecialVar_0x8006 == TRUE;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!taughtMove
        || !RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_UNBOUND_TUTOR
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static bool8 GetAvailableCampCookScene(struct RogueRouteSceneRequest *scene)
{
    return RogueRouteScenes_GetCurrentInteractionRequest(scene)
        && scene->recipeId == ROGUE_ROUTE_SCENE_RECIPE_CAMP_COOK
        && scene->source == ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        && RogueRouteScenes_GetState(scene->sceneSlot) != ROGUE_ROUTE_EVENT_STATE_COMPLETED;
}

static void CompleteCampCookScene(const struct RogueRouteSceneRequest *scene)
{
    RogueRouteScenes_SetState(scene->sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(scene);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCampCookHealParty(void)
{
    struct RogueRouteSceneRequest scene;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetAvailableCampCookScene(&scene))
        return;

    CompleteCampCookScene(&scene);
}

void RogueRouteEvents_TryCampCookGiveBerries(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueRouteSceneRng rng;
    u16 rewards[ROGUE_CAMP_COOK_BERRY_REWARD_COUNT];
    u8 rewardCount = 0;
    u8 addedCount = 0;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!GetAvailableCampCookScene(&scene))
        return;

    RogueRouteSceneRng_Seed(&rng, scene.rewardAmount ^ 0xC00C);
    while(rewardCount < ARRAY_COUNT(rewards))
    {
        u16 item = SelectCampCookBerryReward(&rng);

        if(item == ITEM_NONE)
            return;

        rewards[rewardCount++] = item;
    }

    while(addedCount < ARRAY_COUNT(rewards))
    {
        if(!AddBagItem(rewards[addedCount], 1))
        {
            while(addedCount != 0)
            {
                --addedCount;
                RemoveBagItem(rewards[addedCount], 1);
            }

            gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
            return;
        }

        ++addedCount;
    }

    for(addedCount = 0; addedCount < ARRAY_COUNT(rewards); ++addedCount)
        Rogue_PushPopup_AddItem(rewards[addedCount], 1);

    CompleteCampCookScene(&scene);
}

void RogueRouteEvents_TryCampCookMaxPp(void)
{
    struct RogueRouteSceneRequest scene;
    struct Pokemon *mon;
    u8 ppBonuses = 0;
    u8 moveIdx;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(gSpecialVar_0x8004 >= PARTY_SIZE
        || !GetAvailableCampCookScene(&scene))
        return;

    mon = &gPlayerParty[gSpecialVar_0x8004];
    if(GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE
        || GetMonData(mon, MON_DATA_IS_EGG))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_WRONG_MON;
        return;
    }

    ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
    for(moveIdx = 0; moveIdx < MAX_MON_MOVES; ++moveIdx)
    {
        u16 move = GetMonData(mon, MON_DATA_MOVE1 + moveIdx);

        if(move != MOVE_NONE)
        {
            u8 maxPp;

            ppBonuses &= gPPUpClearMask[moveIdx];
            ppBonuses += gPPUpAddValues[moveIdx] * 3;
            maxPp = CalculatePPWithBonus(move, ppBonuses, moveIdx);
            SetMonData(mon, MON_DATA_PP1 + moveIdx, &maxPp);
        }
    }
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);
    CalculateMonStats(mon);

    CompleteCampCookScene(&scene);
}

static u8 FindMysteryEggCourierQuest(void)
{
    return RogueAdventureQuests_FindByDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_MYSTERY_EGG_COURIER);
}

bool8 RogueRouteEvents_IsMysteryEggCourierEgg(struct Pokemon *mon)
{
    u8 questId;
    const struct RogueAdventureQuest *quest;

    if(!GetMonData(mon, MON_DATA_IS_EGG)
        || !GetMonData(mon, MON_DATA_MODERN_FATEFUL_ENCOUNTER))
        return FALSE;

    questId = FindMysteryEggCourierQuest();
    quest = RogueAdventureQuests_Get(questId);
    return quest != NULL && GetMonData(mon, MON_DATA_SPECIES) == quest->payload[0];
}

static u8 FindMysteryEggCourierPartySlot(u16 eggSpecies)
{
    u8 i;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        if(GetMonData(&gPlayerParty[i], MON_DATA_IS_EGG)
            && GetMonData(&gPlayerParty[i], MON_DATA_MODERN_FATEFUL_ENCOUNTER)
            && Rogue_GetEggSpecies(GetMonData(&gPlayerParty[i], MON_DATA_SPECIES)) == eggSpecies)
            return i;
    }

    return PARTY_NOTHING_CHOSEN;
}

static u8 TryGiveMysteryEggToParty(u16 species, u8 replacementSlot)
{
    struct Pokemon egg;
    bool8 isEgg = TRUE;
    bool8 isCourierEgg = TRUE;
    bool8 needsReplacement;
    u8 destinationSlot;

    if(species == SPECIES_NONE || species == SPECIES_EGG || !RogueTrial_CanReceiveGift())
        return ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;

    CreateEgg(&egg, Rogue_GetEggSpecies(species), FALSE);
    SetMonData(&egg, MON_DATA_IS_EGG, &isEgg);
    SetMonData(&egg, MON_DATA_MODERN_FATEFUL_ENCOUNTER, &isCourierEgg);
    needsReplacement = CalculatePlayerPartyCount() >= Rogue_GetMaxPartySize();

    if(needsReplacement)
    {
        if(replacementSlot == PARTY_NOTHING_CHOSEN)
            return ROGUE_ROUTE_EVENT_RESULT_PARTY_FULL;
        if(replacementSlot >= gPlayerPartyCount)
            return ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        if(!Rogue_CanReleasePartyMonForCaughtMon(&egg, replacementSlot))
        {
            if(!Rogue_CaughtMonFitsSpeciesClauseAfterRelease(&egg, replacementSlot))
                return ROGUE_ROUTE_EVENT_RESULT_SPECIES_CLAUSE;
            return ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        }
        if(!Rogue_TryRemoveDuplicateHeldItemForParty(&egg, replacementSlot, PARTY_SIZE))
            return ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
    }
    else
    {
        if(!Rogue_CanAddCaughtMonToParty(&egg)
            || !Rogue_TryRemoveDuplicateHeldItemForParty(&egg, PARTY_SIZE, PARTY_SIZE))
        {
            if(Rogue_PartyHasDuplicateSpecies(&egg, PARTY_SIZE, PARTY_SIZE))
                return ROGUE_ROUTE_EVENT_RESULT_SPECIES_CLAUSE;
            return ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        }
    }

    if(needsReplacement)
    {
        RemoveMonAtSlot(replacementSlot, TRUE, FALSE);
        destinationSlot = replacementSlot;
    }
    else
    {
        destinationSlot = gPlayerPartyCount;
    }

    CopyMon(&gPlayerParty[destinationSlot], &egg, sizeof(egg));
    CalculatePlayerPartyCount();
    RogueTrial_OnMonGiven(&gPlayerParty[destinationSlot]);
    return ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryAcceptMysteryEggCourierQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_MYSTERY_EGG_COURIER
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_MYSTERY_EGG_COURIER)
        || scene.requestedItem == SPECIES_NONE
        || scene.rewardItem != ITEM_ESCAPE_ROPE)
        return;

    params.payload[0] = scene.requestedItem;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_MYSTERY_EGG_COURIER, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return;

    gSpecialVar_Result = TryGiveMysteryEggToParty(scene.requestedItem, gSpecialVar_0x8006);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
    {
        RogueAdventureQuests_Remove(questId);
        return;
    }

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
}

void RogueRouteEvents_BufferMysteryEggDelivery(void)
{
    u8 questId = FindMysteryEggCourierQuest();
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    gSpecialVar_0x8004 = PARTY_NOTHING_CHOSEN;
    if(quest == NULL)
        return;

    gSpecialVar_0x8004 = FindMysteryEggCourierPartySlot(quest->payload[0]);
    gSpecialVar_Result = gSpecialVar_0x8004 == PARTY_NOTHING_CHOSEN
        ? ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM
        : ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryCompleteMysteryEggDelivery(void)
{
    struct RogueRouteSceneRequest scene = {0};
    u8 questId = FindMysteryEggCourierQuest();
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);
    u8 partySlot;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(quest == NULL)
        return;

    partySlot = FindMysteryEggCourierPartySlot(quest->payload[0]);
    gSpecialVar_0x8004 = partySlot;
    if(partySlot == PARTY_NOTHING_CHOSEN)
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }
    if(!CheckBagHasSpace(ITEM_ESCAPE_ROPE, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }
    if(!AddBagItem(ITEM_ESCAPE_ROPE, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        return;
    }
    if(!RogueAdventureQuests_EmitSignalForQuest(
            questId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_DAYCARE_DELIVERY,
            1))
    {
        RemoveBagItem(ITEM_ESCAPE_ROPE, 1);
        return;
    }

    RemoveMonAtSlot(partySlot, FALSE, TRUE);

    Rogue_PushPopup_AddItem(ITEM_ESCAPE_ROPE, 1);
    scene.recipeId = ROGUE_ROUTE_SCENE_RECIPE_MYSTERY_EGG_COURIER;
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u8 CountFieldRepairParts(u8 progress)
{
    return ((progress & (1 << 1)) != 0)
        + ((progress & (1 << 2)) != 0)
        + ((progress & (1 << 3)) != 0);
}

static u8 FindFieldRepairBenchQuest(void)
{
    return RogueAdventureQuests_FindByDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FIELD_REPAIR_BENCH);
}

static bool8 CanApplyFieldRepairAbilityToMon(struct Pokemon *mon, u16 ability)
{
    u16 species;

    if(mon == NULL
        || ability == ABILITY_NONE
        || !RogueGift_IsStandardAbilityEligible(ability)
        || GetMonData(mon, MON_DATA_SANITY_IS_EGG, NULL))
        return FALSE;

    species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    if(species == SPECIES_NONE
        || GetMonAbility(mon) == ability)
        return FALSE;

    return TRUE;
}

static bool8 CanAnyPartyMonApplyFieldRepairAbility(u16 ability)
{
    u8 i;

    for(i = 0; i < gPlayerPartyCount; ++i)
    {
        if(CanApplyFieldRepairAbilityToMon(&gPlayerParty[i], ability))
            return TRUE;
    }

    return FALSE;
}

void RogueRouteEvents_BufferFieldRepairBenchData(void)
{
    struct RogueRouteSceneRequest scene;
    u8 questId = FindFieldRepairBenchQuest();
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);
    u16 ability = ABILITY_NONE;

    if(quest != NULL)
        ability = quest->payload[0];
    else if(RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        && scene.recipeId == ROGUE_ROUTE_SCENE_RECIPE_FIELD_REPAIR_BENCH)
        ability = scene.rewardItem;

    if(ability < ABILITIES_COUNT)
        StringCopy(gStringVar1, gAbilityNames[ability]);
    else
        StringCopy(gStringVar1, gText_ThreeQuestionMarks);

    gSpecialVar_0x8007 = quest != NULL ? CountFieldRepairParts(quest->progress) : 0;
    gSpecialVar_0x8009 = quest != NULL && CanAnyPartyMonApplyFieldRepairAbility(ability);
}

void RogueRouteEvents_TryAcceptFieldRepairBenchQuest(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FIELD_REPAIR_BENCH
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || scene.lotRole != 0
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || RogueAdventureQuests_HasDefinition(ROGUE_ADVENTURE_QUEST_DEFINITION_FIELD_REPAIR_BENCH)
        || scene.rewardItem == ABILITY_NONE
        || scene.rewardItem >= ABILITIES_COUNT)
        return;

    params.payload[0] = scene.rewardItem;
    params.target = ROGUE_FIELD_REPAIR_PART_COUNT;
    questId = RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FIELD_REPAIR_BENCH, &params);
    if(questId == ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return;

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u8 GetOrCreateFieldRepairBenchQuest(const struct RogueRouteSceneRequest *scene)
{
    struct RogueAdventureQuestCreateParams params = {0};
    u8 questId = FindFieldRepairBenchQuest();

    if(questId != ROGUE_ADVENTURE_QUEST_INVALID_ID)
        return questId;
    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_FIELD_REPAIR_BENCH
        || scene->source != ROGUE_ROUTE_SCENE_SOURCE_QUEST_GENERATOR
        || scene->rewardItem == ABILITY_NONE
        || scene->rewardItem >= ABILITIES_COUNT)
        return ROGUE_ADVENTURE_QUEST_INVALID_ID;

    params.payload[0] = scene->rewardItem;
    params.target = ROGUE_FIELD_REPAIR_PART_COUNT;
    return RogueAdventureQuests_Create(ROGUE_ADVENTURE_QUEST_DEFINITION_FIELD_REPAIR_BENCH, &params);
}

void RogueRouteEvents_CollectFieldRepairPart(void)
{
    struct RogueRouteSceneRequest scene;
    u8 questId;
    const struct RogueAdventureQuest *quest;
    u8 roleBit;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FIELD_REPAIR_BENCH
        || scene.lotRole == 0
        || scene.lotRole >= ROGUE_ROUTE_SCENE_MAX_ROLES
        || RogueRouteScenes_GetState(scene.sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED)
        return;

    questId = GetOrCreateFieldRepairBenchQuest(&scene);
    quest = RogueAdventureQuests_Get(questId);
    roleBit = 1 << scene.lotRole;
    if(quest == NULL)
        return;
    if((quest->progress & roleBit) != 0)
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_ALREADY_DUG;
        return;
    }

    if(!RogueAdventureQuests_EmitSignalForQuest(
            questId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_OBJECTIVE_PROGRESS,
            roleBit))
        return;

    if(RogueAdventureQuests_IsProgressTargetMet(questId))
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
    else
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);

    RogueRouteScenes_HideCurrentInteractionObject();
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_TryApplyFieldRepairAbility(void)
{
    struct RogueRouteSceneRequest scene;
    u8 questId = FindFieldRepairBenchQuest();
    const struct RogueAdventureQuest *quest = RogueAdventureQuests_Get(questId);
    u8 partySlot = gSpecialVar_0x8006;
    u16 ability;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(quest == NULL
        || questId == ROGUE_ADVENTURE_QUEST_INVALID_ID
        || !RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_FIELD_REPAIR_BENCH
        || scene.lotRole != 0
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING
        || !RogueAdventureQuests_IsProgressTargetMet(questId))
        return;

    ability = quest->payload[0];
    if(partySlot >= gPlayerPartyCount
        || !CanApplyFieldRepairAbilityToMon(&gPlayerParty[partySlot], ability))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_WRONG_MON;
        return;
    }

    if(!RogueAdventureQuests_EmitSignalForQuest(
            questId,
            ROGUE_ADVENTURE_QUEST_SIGNAL_SCENE_COMPLETED,
            1))
        return;

    gPlayerParty[partySlot].rogueExtraData.abilityOverride = ability;
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_FinishTravelingMerchant(void)
{
    struct RogueRouteSceneRequest scene;
    bool8 boughtAnything = gSpecialVar_Result == TRUE;

    if(!boughtAnything
        || !RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_TRAVELING_MERCHANT)
        return;

    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
}

void RogueRouteEvents_TryClaimTideSalvage(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_TIDE_SALVAGE
        || scene.source != ROGUE_ROUTE_SCENE_SOURCE_ONE_OFF
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED
        || scene.rewardItem == ITEM_NONE
        || scene.rewardAmount == 0)
        return;

    transaction.rewards[0].itemId = scene.rewardItem;
    transaction.rewards[0].count = scene.rewardAmount;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    Rogue_PushPopup_AddItem(scene.rewardItem, scene.rewardAmount);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
}

static bool8 CreateBreedersExchangeMonForScene(const struct RogueRouteSceneRequest *scene, struct Pokemon *mon)
{
    struct RoguePokemonCompetitiveSetRules rules =
    {
        .skipHeldItem = TRUE,
        .allowMissingMoves = TRUE,
    };
    struct RogueRouteSceneRng rng;
    bool8 perfectStats[NUM_STATS] = {FALSE};
    u32 personality;
    u16 pokeball = ITEM_FRIEND_BALL;
    u8 remainingStats = NUM_STATS;
    u8 perfectCount;
    u8 stat;

    if(scene == NULL
        || mon == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE
        || scene->rewardItem == SPECIES_NONE
        || scene->rewardItem >= NUM_SPECIES
        || scene->trainerNum >= gRoguePokemonProfiles[scene->rewardItem].competitiveSetCount)
        return FALSE;

    RogueRouteSceneRng_Seed(&rng, scene->rewardAmount ^ scene->rewardItem ^ 0x71AD);
    personality = RogueRouteSceneRng_Next(&rng);
    personality |= (u32)RogueRouteSceneRng_Next(&rng) << 16;
    CreateMon(
        mon,
        scene->rewardItem,
        Rogue_CalculatePlayerMonLvl(),
        0,
        TRUE,
        personality,
        OT_ID_PRESET,
        BREEDERS_EXCHANGE_OT_ID);
    SetMonData(mon, MON_DATA_OT_NAME, sBreedersExchangeOtName);
    SetMonData(mon, MON_DATA_POKEBALL, &pokeball);
    Rogue_AssignAutomaticNicknameFromSeed(mon, scene->rewardAmount ^ scene->rewardItem ^ scene->trainerNum);
    Rogue_ApplyMonCompetitiveSet(
        mon,
        Rogue_CalculatePlayerMonLvl(),
        &gRoguePokemonProfiles[scene->rewardItem].competitiveSets[scene->trainerNum],
        &rules);

    // Give the remaining stats ordinary seeded IVs, then choose exactly three
    // different stats to perfect. This keeps the value legible without making
    // every exchange Pokémon generically flawless.
    for(stat = 0; stat < NUM_STATS; ++stat)
    {
        u8 iv = RogueRouteSceneRng_Next(&rng) % 31;
        SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
    }
    for(perfectCount = 0; perfectCount < 3; ++perfectCount)
    {
        u8 selected = RogueRouteSceneRng_Next(&rng) % remainingStats;

        for(stat = 0; stat < NUM_STATS; ++stat)
        {
            if(!perfectStats[stat] && selected-- == 0)
            {
                u8 iv = 31;

                perfectStats[stat] = TRUE;
                --remainingStats;
                SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
                break;
            }
        }
    }

    mon->rogueExtraData.lastPopupLevel = GetMonData(mon, MON_DATA_LEVEL) - 1;
    mon->rogueExtraData.hasPendingEvo = FALSE;
    CalculateMonStats(mon);
    return TRUE;
}

bool8 RogueRouteEvents_CreateBreedersExchangeMon(struct Pokemon *mon)
{
    struct RogueRouteSceneRequest scene;

    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return FALSE;

    return CreateBreedersExchangeMonForScene(&scene, mon);
}

static void Task_ShowBreedersExchangePreview(u8 taskId)
{
    if(gPaletteFade.active)
        return;

    CleanupOverworldWindowsAndTilemaps();
    ShowPokemonSummaryScreen(
        SUMMARY_MODE_NORMAL,
        gEnemyParty,
        0,
        0,
        CB2_ReturnToFieldContinueScriptPlayMapMusic);
    DestroyTask(taskId);
}

void RogueRouteEvents_ShowBreedersExchangePreview(void)
{
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    ZeroEnemyPartyMons();
    if(!RogueRouteEvents_CreateBreedersExchangeMon(&gEnemyParty[0]))
        return;

    FadeScreen(FADE_TO_BLACK, 0);
    CreateTask(Task_ShowBreedersExchangePreview, 10);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static u8 ValidateBreedersExchangeSelection(
    const struct RogueRouteSceneRequest *scene,
    u8 partySlot,
    struct Pokemon *offeredMon)
{
    struct Pokemon generatedMon;
    u16 heldItem;

    if(scene == NULL
        || scene->recipeId != ROGUE_ROUTE_SCENE_RECIPE_BREEDERS_EXCHANGE
        || RogueRouteScenes_GetState(scene->sceneSlot) == ROGUE_ROUTE_EVENT_STATE_COMPLETED
        || partySlot >= gPlayerPartyCount)
        return ROGUE_ROUTE_EVENT_RESULT_FAILED;

    if(GetMonData(&gPlayerParty[partySlot], MON_DATA_IS_EGG)
        || GetMonData(&gPlayerParty[partySlot], MON_DATA_SPECIES) != scene->requestedItem)
        return ROGUE_ROUTE_EVENT_RESULT_WRONG_MON;

    heldItem = GetMonData(&gPlayerParty[partySlot], MON_DATA_HELD_ITEM);
    if(RogueGift_GetCustomMonId(&gPlayerParty[partySlot]) != CUSTOM_MON_NONE
        || RogueAdventureQuests_IsItemProtected(heldItem))
        return ROGUE_ROUTE_EVENT_RESULT_PROTECTED_MON;

    if(heldItem != ITEM_NONE && !CheckBagHasSpace(heldItem, 1))
        return ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;

    if(!CreateBreedersExchangeMonForScene(scene, &generatedMon)
        || !Rogue_CanReleasePartyMonForCaughtMon(&generatedMon, partySlot))
        return ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;

    FollowMon_SetGraphics(1, scene->rewardItem, FALSE, BREEDERS_EXCHANGE_OT_ID);
    if(offeredMon != NULL)
        CopyMon(offeredMon, &generatedMon, sizeof(generatedMon));
    return ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueRouteEvents_ValidateBreedersExchangeSelection(void)
{
    struct RogueRouteSceneRequest scene;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;

    gSpecialVar_Result = ValidateBreedersExchangeSelection(&scene, gSpecialVar_0x8006, NULL);
}

void RogueRouteEvents_TryCompleteBreedersExchange(void)
{
    struct RogueRouteSceneRequest scene;
    struct Pokemon offeredMon;
    struct Pokemon *tradedMon;
    u16 heldItem;
    u8 partySlot = gSpecialVar_0x8006;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene))
        return;

    gSpecialVar_Result = ValidateBreedersExchangeSelection(&scene, partySlot, &offeredMon);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;
    if(!RogueTrial_CanReceiveGift())
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_CANT_GIVE_MON;
        return;
    }

    tradedMon = &gPlayerParty[partySlot];
    heldItem = GetMonData(tradedMon, MON_DATA_HELD_ITEM);
    if(heldItem != ITEM_NONE)
    {
        if(!AddBagItem(heldItem, 1))
        {
            gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
            return;
        }

        Rogue_PushPopup_AddItem(heldItem, 1);
        if(ItemIsMail(heldItem))
            TakeMailFromMon(tradedMon);
        else
        {
            u16 noItem = ITEM_NONE;
            SetMonData(tradedMon, MON_DATA_HELD_ITEM, &noItem);
        }
    }

    RemoveMonAtSlot(partySlot, FALSE, FALSE);
    CopyMon(&gPlayerParty[partySlot], &offeredMon, sizeof(offeredMon));
    CalculatePlayerPartyCount();
    RogueTrial_OnMonGiven(&gPlayerParty[partySlot]);
    Rogue_PushPopup_AddPokemon(scene.rewardItem, FALSE, FALSE);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteScenes_HideProp(scene.sceneSlot, 1);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    gSpecialVar_0x8004 = scene.rewardItem;
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

static const u8 sText_BuriedCacheFieldLandmark0[] = _("old stump");
static const u8 sText_BuriedCacheFieldLandmark1[] = _("boulder");
static const u8 sText_BuriedCacheFieldLandmark2[] = _("abandoned crate");
static const u8 sText_BuriedCacheForestLandmark0[] = _("old stump");
static const u8 sText_BuriedCacheForestLandmark1[] = _("mossy stone");
static const u8 sText_BuriedCacheForestLandmark2[] = _("fallen log");
static const u8 sText_BuriedCacheCaveLandmark0[] = _("cracked boulder");
static const u8 sText_BuriedCacheCaveLandmark1[] = _("rubble pile");
static const u8 sText_BuriedCacheCaveLandmark2[] = _("abandoned crate");
static const u8 sText_BuriedCacheMountainLandmark0[] = _("stone pile");
static const u8 sText_BuriedCacheMountainLandmark1[] = _("split rock");
static const u8 sText_BuriedCacheMountainLandmark2[] = _("weathered supplies");
static const u8 sText_BuriedCacheWaterLandmark0[] = _("driftwood");
static const u8 sText_BuriedCacheWaterLandmark1[] = _("washed-up crate");
static const u8 sText_BuriedCacheWaterLandmark2[] = _("smooth stone");
static const u8 sText_BuriedCacheUrbanLandmark0[] = _("shipping crate");
static const u8 sText_BuriedCacheUrbanLandmark1[] = _("barrel");
static const u8 sText_BuriedCacheUrbanLandmark2[] = _("street marker");

static const u8 *const sBuriedCacheLandmarkNames[ROGUE_ROUTE_ENVIRONMENT_COUNT][3] =
{
    {sText_BuriedCacheFieldLandmark0, sText_BuriedCacheFieldLandmark1, sText_BuriedCacheFieldLandmark2},
    {sText_BuriedCacheForestLandmark0, sText_BuriedCacheForestLandmark1, sText_BuriedCacheForestLandmark2},
    {sText_BuriedCacheCaveLandmark0, sText_BuriedCacheCaveLandmark1, sText_BuriedCacheCaveLandmark2},
    {sText_BuriedCacheMountainLandmark0, sText_BuriedCacheMountainLandmark1, sText_BuriedCacheMountainLandmark2},
    {sText_BuriedCacheWaterLandmark0, sText_BuriedCacheWaterLandmark1, sText_BuriedCacheWaterLandmark2},
    {sText_BuriedCacheUrbanLandmark0, sText_BuriedCacheUrbanLandmark1, sText_BuriedCacheUrbanLandmark2},
};

static const u8 sText_BuriedCacheMarking0[] = _("a crescent mark");
static const u8 sText_BuriedCacheMarking1[] = _("a spiral mark");
static const u8 sText_BuriedCacheMarking2[] = _("crossed lines");
static const u8 sText_BuriedCacheMarking3[] = _("three scratches");
static const u8 *const sBuriedCacheMarkingNames[] =
{
    sText_BuriedCacheMarking0,
    sText_BuriedCacheMarking1,
    sText_BuriedCacheMarking2,
    sText_BuriedCacheMarking3,
};

static const u8 sText_BuriedCacheFieldGround0[] = _("pale soil");
static const u8 sText_BuriedCacheFieldGround1[] = _("red clay");
static const u8 sText_BuriedCacheFieldGround2[] = _("short grass");
static const u8 sText_BuriedCacheForestGround0[] = _("dark earth");
static const u8 sText_BuriedCacheForestGround1[] = _("thick moss");
static const u8 sText_BuriedCacheForestGround2[] = _("tangled roots");
static const u8 sText_BuriedCacheCaveGround0[] = _("pale dust");
static const u8 sText_BuriedCacheCaveGround1[] = _("loose gravel");
static const u8 sText_BuriedCacheCaveGround2[] = _("damp grit");
static const u8 sText_BuriedCacheMountainGround0[] = _("red dust");
static const u8 sText_BuriedCacheMountainGround1[] = _("loose scree");
static const u8 sText_BuriedCacheMountainGround2[] = _("packed earth");
static const u8 sText_BuriedCacheWaterGround0[] = _("damp sand");
static const u8 sText_BuriedCacheWaterGround1[] = _("smooth pebbles");
static const u8 sText_BuriedCacheWaterGround2[] = _("salt-stained soil");
static const u8 sText_BuriedCacheUrbanGround0[] = _("cracked paving");
static const u8 sText_BuriedCacheUrbanGround1[] = _("soot-dark earth");
static const u8 sText_BuriedCacheUrbanGround2[] = _("struggling weeds");

static const u8 *const sBuriedCacheGroundNames[ROGUE_ROUTE_ENVIRONMENT_COUNT][3] =
{
    {sText_BuriedCacheFieldGround0, sText_BuriedCacheFieldGround1, sText_BuriedCacheFieldGround2},
    {sText_BuriedCacheForestGround0, sText_BuriedCacheForestGround1, sText_BuriedCacheForestGround2},
    {sText_BuriedCacheCaveGround0, sText_BuriedCacheCaveGround1, sText_BuriedCacheCaveGround2},
    {sText_BuriedCacheMountainGround0, sText_BuriedCacheMountainGround1, sText_BuriedCacheMountainGround2},
    {sText_BuriedCacheWaterGround0, sText_BuriedCacheWaterGround1, sText_BuriedCacheWaterGround2},
    {sText_BuriedCacheUrbanGround0, sText_BuriedCacheUrbanGround1, sText_BuriedCacheUrbanGround2},
};

static const u8 sText_BuriedCacheTypeAncient[] = _("ancient cache");
static const u8 sText_BuriedCacheTypeTrainer[] = _("Trainer's cache");
static const u8 sText_BuriedCacheTypeRelic[] = _("collector's relic");
static const u8 sText_BuriedCacheTypeJackpot[] = _("exceptional treasure");
static const u8 sText_BuriedCacheLandmarkLabel[] = _("Landmark: ");
static const u8 sText_BuriedCacheMarkLabel[] = _("Mark: ");
static const u8 sText_BuriedCacheGroundLabel[] = _("Ground: ");
static const u8 sText_BuriedCacheNewline[] = _("\n");
static const u8 sText_BuriedCachePeriod[] = _(".");
static const u8 *const sBuriedCacheTypeNames[] =
{
    sText_BuriedCacheTypeAncient,
    sText_BuriedCacheTypeTrainer,
    sText_BuriedCacheTypeRelic,
    sText_BuriedCacheTypeJackpot,
};

static bool8 IsBuriedCacheSiteDug(u8 lotRole)
{
    if(lotRole == 1)
        return (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_BURIED_CACHE_SITE_A_DUG) != 0;
    if(lotRole == 2)
        return (VarGet(VAR_ROGUE_ROUTE_EVENT_STATE) & ROUTE_SCENE_BURIED_CACHE_SITE_B_DUG) != 0;
    return FALSE;
}

static void MarkBuriedCacheSiteDug(u8 lotRole)
{
    u16 state = VarGet(VAR_ROGUE_ROUTE_EVENT_STATE);

    if(lotRole == 1)
        state |= ROUTE_SCENE_BURIED_CACHE_SITE_A_DUG;
    else if(lotRole == 2)
        state |= ROUTE_SCENE_BURIED_CACHE_SITE_B_DUG;

    VarSet(VAR_ROGUE_ROUTE_EVENT_STATE, state);
}

void RogueRouteEvents_BufferBuriedCacheData(void)
{
    struct RogueRouteSceneRequest scene;
    struct BuriedCacheData data;
    const struct BuriedCacheSiteData *site;
    u8 environment;

    gStringVar1[0] = EOS;
    gStringVar2[0] = EOS;
    gStringVar3[0] = EOS;
    gSpecialVar_0x8003 = FALSE;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE)
        return;

    environment = scene.environment < ROGUE_ROUTE_ENVIRONMENT_COUNT
        ? scene.environment
        : ROGUE_ROUTE_ENVIRONMENT_FIELD;
    GenerateBuriedCacheData(scene.rewardAmount, environment, &data);
    gSpecialVar_0x8004 = data.rewardItem;
    gSpecialVar_0x8005 = data.secondaryRewardItem;
    gSpecialVar_0x8006 = data.ambushSpecies;
    gSpecialVar_0x8007 = data.cacheType;
    StringCopy(gStringVar3, sBuriedCacheTypeNames[data.cacheType]);
    {
        const struct BuriedCacheSiteData *correct = &data.sites[data.correctSite];

        if(data.clueTraitA == BURIED_CACHE_TRAIT_LANDMARK
            && data.clueTraitB == BURIED_CACHE_TRAIT_MARKING)
        {
            StringCopy(gStringVar2, sText_BuriedCacheLandmarkLabel);
            StringAppend(gStringVar2, sBuriedCacheLandmarkNames[environment][correct->landmark]);
            StringAppend(gStringVar2, sText_BuriedCachePeriod);
            StringAppend(gStringVar2, sText_BuriedCacheNewline);
            StringAppend(gStringVar2, sText_BuriedCacheMarkLabel);
            StringAppend(gStringVar2, sBuriedCacheMarkingNames[correct->marking]);
            StringAppend(gStringVar2, sText_BuriedCachePeriod);
        }
        else if(data.clueTraitA == BURIED_CACHE_TRAIT_LANDMARK)
        {
            StringCopy(gStringVar2, sText_BuriedCacheLandmarkLabel);
            StringAppend(gStringVar2, sBuriedCacheLandmarkNames[environment][correct->landmark]);
            StringAppend(gStringVar2, sText_BuriedCachePeriod);
            StringAppend(gStringVar2, sText_BuriedCacheNewline);
            StringAppend(gStringVar2, sText_BuriedCacheGroundLabel);
            StringAppend(gStringVar2, sBuriedCacheGroundNames[environment][correct->ground]);
            StringAppend(gStringVar2, sText_BuriedCachePeriod);
        }
        else
        {
            StringCopy(gStringVar2, sText_BuriedCacheMarkLabel);
            StringAppend(gStringVar2, sBuriedCacheMarkingNames[correct->marking]);
            StringAppend(gStringVar2, sText_BuriedCachePeriod);
            StringAppend(gStringVar2, sText_BuriedCacheNewline);
            StringAppend(gStringVar2, sText_BuriedCacheGroundLabel);
            StringAppend(gStringVar2, sBuriedCacheGroundNames[environment][correct->ground]);
            StringAppend(gStringVar2, sText_BuriedCachePeriod);
        }
    }

    if(scene.lotRole == 0)
        return;

    site = &data.sites[scene.lotRole - 1];
    StringCopy(gStringVar1, sText_BuriedCacheLandmarkLabel);
    StringAppend(gStringVar1, sBuriedCacheLandmarkNames[environment][site->landmark]);
    StringAppend(gStringVar1, sText_BuriedCachePeriod);
    StringAppend(gStringVar1, sText_BuriedCacheNewline);
    StringAppend(gStringVar1, sText_BuriedCacheMarkLabel);
    StringAppend(gStringVar1, sBuriedCacheMarkingNames[site->marking]);
    StringAppend(gStringVar1, sText_BuriedCachePeriod);
    StringAppend(gStringVar1, sText_BuriedCacheNewline);
    StringAppend(gStringVar1, sText_BuriedCacheGroundLabel);
    StringAppend(gStringVar1, sBuriedCacheGroundNames[environment][site->ground]);
    StringAppend(gStringVar1, sText_BuriedCachePeriod);
    gSpecialVar_0x8003 = IsBuriedCacheSiteDug(scene.lotRole);
}

void RogueRouteEvents_TryAcceptBuriedCache(void)
{
    struct RogueRouteSceneRequest scene;
    struct RogueEventTransaction transaction = {0};

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE
        || scene.lotRole != 0
        || RogueRouteScenes_GetState(scene.sceneSlot) != ROGUE_ROUTE_EVENT_STATE_NOT_STARTED)
        return;

    transaction.rewards[0].itemId = ITEM_FIELD_SHOVEL;
    transaction.rewards[0].count = 1;
    transaction.rewardCount = 1;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
        return;

    Rogue_PushPopup_AddItem(ITEM_FIELD_SHOVEL, 1);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_ACTIVE);
}

static void PrepareBuriedCacheAmbush(const struct RogueRouteSceneRequest *scene, u16 species)
{
    RAND_TYPE originalRng = gRngValue;
    u8 level = min(MAX_LEVEL, Rogue_CalculatePlayerMonLvl() + 2);

    SeedRng(scene->rewardAmount ^ species ^ 0xA8B5);
    ZeroEnemyPartyMons();
    CreateMon(&gEnemyParty[0], species, level, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    gRngValue = originalRng;
}

void RogueRouteEvents_TryDigBuriedCache(void)
{
    struct RogueRouteSceneRequest scene;
    struct BuriedCacheData data;
    struct RogueEventTransaction transaction = {0};
    u8 state;
    u8 siteIndex;

    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_FAILED;
    if(!RogueRouteScenes_GetCurrentInteractionRequest(&scene)
        || scene.recipeId != ROGUE_ROUTE_SCENE_RECIPE_BURIED_CACHE
        || scene.lotRole < 1
        || scene.lotRole > 2)
        return;

    state = RogueRouteScenes_GetState(scene.sceneSlot);
    if((state != ROGUE_ROUTE_EVENT_STATE_ACTIVE
            && state != ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING)
        || !CheckBagHasItem(ITEM_FIELD_SHOVEL, 1))
    {
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        return;
    }

    GenerateBuriedCacheData(scene.rewardAmount, scene.environment, &data);
    siteIndex = scene.lotRole - 1;
    if(siteIndex != data.correctSite)
    {
        if(IsBuriedCacheSiteDug(scene.lotRole))
        {
            gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_ALREADY_DUG;
            return;
        }

        MarkBuriedCacheSiteDug(scene.lotRole);
        if(data.ambushSpecies != SPECIES_NONE)
            PrepareBuriedCacheAmbush(&scene, data.ambushSpecies);
        gSpecialVar_0x8006 = data.ambushSpecies;
        gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_WRONG_SITE;
        return;
    }

    MarkBuriedCacheSiteDug(scene.lotRole);
    transaction.costs[0].itemId = ITEM_FIELD_SHOVEL;
    transaction.costs[0].count = 1;
    transaction.rewards[0].itemId = data.rewardItem;
    transaction.rewards[0].count = 1;
    transaction.costCount = 1;
    transaction.rewardCount = 1;
    if(data.secondaryRewardItem != ITEM_NONE)
    {
        transaction.rewards[1].itemId = data.secondaryRewardItem;
        transaction.rewards[1].count = 1;
        transaction.rewardCount = 2;
    }
    transaction.moneyReward = data.moneyReward;
    gSpecialVar_Result = RogueEventTransaction_Execute(&transaction);
    if(gSpecialVar_Result != ROGUE_ROUTE_EVENT_RESULT_SUCCESS)
    {
        RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_REWARD_PENDING);
        return;
    }

    Rogue_PushPopup_AddItem(data.rewardItem, 1);
    if(data.secondaryRewardItem != ITEM_NONE)
        Rogue_PushPopup_AddItem(data.secondaryRewardItem, 1);
    if(data.moneyReward != 0)
        Rogue_PushPopup_AddMoney(data.moneyReward);
    RogueRouteScenes_SetState(scene.sceneSlot, ROGUE_ROUTE_EVENT_STATE_COMPLETED);
    RogueRouteEvents_MarkSceneFamilyCompleted(&scene);
    gSpecialVar_Result = ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}
