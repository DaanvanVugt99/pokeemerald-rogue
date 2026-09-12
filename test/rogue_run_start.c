#include "global.h"
#include "constants/flags.h"
#include "constants/game_stat.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"
#include "constants/rogue_pokedex.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "overworld.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_multiplayer.h"
#include "rogue_pokedex.h"
#include "rogue_run_start.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_trials.h"
#include "rogue_quest.h"
#include "test/test.h"
#include "string_util.h"

#define TEST_NET_STATE_ACTIVE (1 << 0)

static void ClearRunReviewDayCare(void)
{
    u8 i;

    for (i = 0; i < DAYCARE_SLOT_COUNT; ++i)
        ZeroBoxMonData(Rogue_GetDaycareBoxMon(i));
}

static void ResetRunReviewTestState(void)
{
    RogueRunStart_Clear();
    gRogueMultiplayer = NULL;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    ClearRunReviewDayCare();
    Rogue_ResetSettingsToDefaults();
    memset(gRogueSaveBlock->bestAscension, ASCENSION_MAX, sizeof(gRogueSaveBlock->bestAscension));
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_GEN9);
    memset(gRogueSaveBlock->adventureReplay, 0, sizeof(gRogueSaveBlock->adventureReplay));
}

static void FinishRunReviewTest(void)
{
    RogueRunStart_Clear();
    gRogueMultiplayer = NULL;
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    FlagClear(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    ClearRunReviewDayCare();
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
}

static void SetRunReviewPartyMon(u8 slot, u16 species)
{
    CreateMon(&gPlayerParty[slot], species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();
}

static void SetRunReviewDayCareMon(u8 slot, u16 species)
{
    struct Pokemon mon;

    CreateMon(&mon, species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CopyMon(Rogue_GetDaycareBoxMon(slot), &mon.box, sizeof(struct BoxPokemon));
}

static void SelectRunReviewTrial(u8 trialId, u8 difficulty, u8 pokedexVariant)
{
    gSpecialVar_0x8004 = trialId;
    gSpecialVar_0x8005 = difficulty;
    gSpecialVar_0x8006 = pokedexVariant;
}

TEST("Run review: local Adventures stage the current configuration")
{
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, BATTLE_FORMAT_DOUBLES);
    Rogue_SetConfigRange(CONFIG_RANGE_ASCENSION, 10);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();

    EXPECT(context != NULL);
    EXPECT_EQ(context->source, RUN_START_SOURCE_NORMAL);
    EXPECT_EQ(context->effectiveConfig.ascension, 10);
    EXPECT_EQ(context->effectiveConfig.battleFormat, BATTLE_FORMAT_DOUBLES);
    EXPECT(Rogue_HasRunStartConfigOverride());
    EXPECT(RogueRunStart_CanStart());
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_NONE);

    FinishRunReviewTest();
}

TEST("Run review: Replay stages its remembered configuration")
{
    const struct RogueRunStartContext *context;
    struct AdventureReplay *replay;

    ResetRunReviewTestState();
    Rogue_SetConfigRange(CONFIG_RANGE_ASCENSION, 0);
    replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];
    memcpy(&replay->adventureConfig, &gRogueSaveBlock->adventureConfig, sizeof(replay->adventureConfig));
    replay->adventureConfig.ascension = 20;
    replay->adventureConfig.battleFormat = BATTLE_FORMAT_MIXED;
    replay->isValid = TRUE;
    replay->rulesVersion = ASCENSION_RULES_VERSION;
    FlagSet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();

    EXPECT(context != NULL);
    EXPECT_EQ(context->source, RUN_START_SOURCE_REPLAY);
    EXPECT(!context->canEdit);
    EXPECT(context->mainQuestsDisabled);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_ASCENSION), 20);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT), BATTLE_FORMAT_MIXED);

    replay->isValid = FALSE;
    FinishRunReviewTest();
}

#ifdef ROGUE_FEATURE_MULTIPLAYER
TEST("Run review: multiplayer guests stage and refresh the host configuration")
{
    struct RogueNetMultiplayer multiplayer;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    memset(&multiplayer, 0, sizeof(multiplayer));
    multiplayer.netCurrentState = TEST_NET_STATE_ACTIVE;
    multiplayer.localPlayerId = 1;
    multiplayer.gameState.adventure.isRunActive = FALSE;
    memcpy(&multiplayer.gameState.hub.adventureConfig,
           &gRogueSaveBlock->adventureConfig,
           sizeof(multiplayer.gameState.hub.adventureConfig));
    multiplayer.gameState.hub.adventureConfig.ascension = 10;
    gRogueMultiplayer = &multiplayer;
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->source, RUN_START_SOURCE_MULTIPLAYER_CLIENT);
    EXPECT(!context->canEdit);
    EXPECT_EQ(context->readiness, RUN_START_WAITING_FOR_HOST);
    EXPECT(!RogueRunStart_CanStart());
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_ASCENSION), 10);

    multiplayer.gameState.adventure.isRunActive = TRUE;
    multiplayer.gameState.hub.adventureConfig.ascension = 20;
    multiplayer.gameState.hub.adventureConfig.pokedexVariant = POKEDEX_VARIANT_HOENN_RSE;
    RogueRunStart_Refresh();
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_ASCENSION), 20);
    EXPECT_EQ(context->pokedexVariant, POKEDEX_VARIANT_HOENN_RSE);

    gRogueMultiplayer = NULL;
    RogueRunStart_Refresh();
    EXPECT_EQ(context->readiness, RUN_START_WAITING_FOR_HOST);
    EXPECT(!RogueRunStart_CanStart());

    FinishRunReviewTest();
}
#endif

#ifdef ROGUE_EXPANSION
TEST("Run review: Trial preview matches the applied configuration")
{
    struct RogueAdventureConfig expected;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    Rogue_CopyAdventureConfig(&expected);
    EXPECT(RogueTrial_BuildSelectionConfig(
        ROGUE_TRIAL_Z_A_ROYALE,
        0,
        POKEDEX_VARIANT_LEGENDS_ZAFULLDLC,
        &expected));

    SelectRunReviewTrial(ROGUE_TRIAL_Z_A_ROYALE, 0, POKEDEX_VARIANT_LEGENDS_ZAFULLDLC);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT(context != NULL);
    EXPECT(memcmp(&context->effectiveConfig, &expected, sizeof(expected)) == 0);

    RogueRunStart_Commit();
    EXPECT(gSpecialVar_Result);
    RogueTrial_ApplyPendingSelection();
    RogueRunStart_Clear();
    EXPECT(memcmp(&gRogueSaveBlock->activeAdventureConfig, &expected, sizeof(expected)) == 0);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER_ORDER), TRAINER_ORDER_RAINBOW);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT), BATTLE_FORMAT_DOUBLES);
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS));

    FinishRunReviewTest();
}
#endif

TEST("Run review: forced Trial settings are present in the preview")
{
    struct RogueAdventureConfig config;

    ResetRunReviewTestState();
    Rogue_CopyAdventureConfig(&config);
    EXPECT(!RogueTrial_BuildSelectionConfig(
        ROGUE_TRIAL_INSANE_MODE,
        0,
        POKEDEX_VARIANT_NATIONAL_GEN9,
        &config));
    EXPECT(RogueTrial_BuildSelectionConfig(
        ROGUE_TRIAL_INSANE_MODE,
        10,
        POKEDEX_VARIANT_NATIONAL_GEN9,
        &config));
    EXPECT_EQ(config.ascension, 10);
    EXPECT_EQ(config.battleFormat, BATTLE_FORMAT_MIXED);
    EXPECT(config.trialFreshStart);

    FinishRunReviewTest();
}

TEST("Run review: readiness distinguishes recovery, Day Care, and fixed parties")
{
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    SetRunReviewPartyMon(0, SPECIES_ZIGZAGOON);
    SelectRunReviewTrial(ROGUE_TRIAL_TYPE_WATER, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY_REQUIRES_PARTNER);
    EXPECT(context->requiresRandomPartner);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_PARTY_ILLEGAL);
    EXPECT_EQ(context->eligibilityReason, ROGUE_TRIAL_ELIGIBILITY_TYPE);
    EXPECT_EQ(context->ineligibleSpecies, SPECIES_ZIGZAGOON);
    EXPECT_EQ(context->eligibilityParam, TYPE_WATER);

    SetRunReviewDayCareMon(0, SPECIES_ZIGZAGOON);
    RogueRunStart_Refresh();
    EXPECT_EQ(context->readiness, RUN_START_BLOCKED_DAY_CARE);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_DAY_CARE_ILLEGAL);
    EXPECT_EQ(context->eligibilityReason, ROGUE_TRIAL_ELIGIBILITY_TYPE);
    EXPECT_EQ(context->ineligibleSpecies, SPECIES_ZIGZAGOON);
    EXPECT_EQ(context->eligibilityParam, TYPE_WATER);
    RogueRunStart_Clear();

    ClearRunReviewDayCare();
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    SelectRunReviewTrial(ROGUE_TRIAL_ORRE_STYLE, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_NONE);
    EXPECT(RogueTrial_PendingHasFixedStartingPartyValue());

    RogueRunStart_Clear();
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    SetRunReviewPartyMon(0, SPECIES_TREECKO);
    SetRunReviewPartyMon(1, SPECIES_TORCHIC);
    SelectRunReviewTrial(ROGUE_TRIAL_IRON_MONO, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY_REQUIRES_PARTNER);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_PARTY_CAPACITY);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_INSANE_MODE, 10, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY_REQUIRES_PARTNER);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_TRIAL_REPLACES_PARTY);

    FinishRunReviewTest();
}

TEST("Run review: species and Legendary clauses block or require Partner recovery")
{
    bool8 hadRandomPartnerUpgrade;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    hadRandomPartnerUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, TRUE);
    SetRunReviewPartyMon(0, SPECIES_ZIGZAGOON);
    SetRunReviewPartyMon(1, SPECIES_ZIGZAGOON);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_BLOCKED_PARTY);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_SPECIES_CLAUSE);
    RogueRunStart_Clear();

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY_REQUIRES_PARTNER);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_SPECIES_CLAUSE);
    RogueRunStart_Clear();

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_LEGENDARY_CLAUSE, TRUE);
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    SetRunReviewPartyMon(0, SPECIES_MEWTWO);
    SetRunReviewPartyMon(1, SPECIES_RAYQUAZA);
    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_BLOCKED_PARTY);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_LEGENDARY_CLAUSE);

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadRandomPartnerUpgrade);
    FinishRunReviewTest();
}

TEST("Run review: Pokédex legality blocks or requires Partner recovery")
{
    bool8 hadRandomPartnerUpgrade;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    hadRandomPartnerUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, FALSE);
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_HOENN_RSE);
    SetRunReviewPartyMon(0, SPECIES_BULBASAUR);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_BLOCKED_PARTY);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_PARTY_ILLEGAL);
    EXPECT_EQ(context->eligibilityReason, ROGUE_TRIAL_ELIGIBILITY_POKEDEX);
    EXPECT_EQ(context->ineligibleSpecies, SPECIES_BULBASAUR);
    RogueRunStart_Clear();

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY_REQUIRES_PARTNER);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_PARTY_ILLEGAL);
    EXPECT_EQ(context->eligibilityReason, ROGUE_TRIAL_ELIGIBILITY_POKEDEX);
    EXPECT_EQ(context->ineligibleSpecies, SPECIES_BULBASAUR);

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadRandomPartnerUpgrade);
    FinishRunReviewTest();
}

TEST("Run review: eligibility reasons identify the exact violated rule")
{
    u16 param;

    ResetRunReviewTestState();

    SelectRunReviewTrial(ROGUE_TRIAL_TYPE_WATER, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_ZIGZAGOON, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_TYPE);
    EXPECT_EQ(param, TYPE_WATER);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LOW_BST, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_SLAKING, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_BST);
    EXPECT_EQ(param, 400);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LITTLE_CUP, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_CHARIZARD, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_LITTLE_CUP);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_CANT_PICK, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_ZIGZAGOON, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_STARTER_FAMILY);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_INSANE_MODE, 10, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_MEWTWO, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_LEGENDARY_FORBIDDEN);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_APOTHEOSIS, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_ZIGZAGOON, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_LEGENDARY_REQUIRED);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LIMITED_CAPTURE, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_TREECKO, TRUE, &param), ROGUE_TRIAL_ELIGIBILITY_DAY_CARE_DISABLED);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_CURSED_BODY, 0, POKEDEX_VARIANT_HOENN_RSE);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_BULBASAUR, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_POKEDEX);
    EXPECT_EQ(param, POKEDEX_VARIANT_HOENN_RSE);

    FinishRunReviewTest();
}

TEST("Run review: clearing removes staged settings and pending Trial state")
{
    ResetRunReviewTestState();
    SelectRunReviewTrial(ROGUE_TRIAL_TYPE_WATER, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT(RogueRunStart_GetContext() != NULL);
    EXPECT(Rogue_HasRunStartConfigOverride());
    RogueTrial_HasPendingSelection();
    EXPECT(gSpecialVar_Result);

    RogueRunStart_Clear();
    EXPECT(RogueRunStart_GetContext() == NULL);
    EXPECT(!Rogue_HasRunStartConfigOverride());
    RogueTrial_HasPendingSelection();
    EXPECT(!gSpecialVar_Result);

    FinishRunReviewTest();
}

TEST("Run review: Starting Team defaults to an eligible Current Party")
{
    bool8 hadStarterBagUpgrade;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    hadStarterBagUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT(context->canUseCurrentParty);
    EXPECT(context->canUseStarterBag);
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT(RogueRunStart_CanStart());

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadStarterBagUpgrade);
    FinishRunReviewTest();
}

TEST("Run review: Starting Team retains an explicit Starter Bag choice")
{
    bool8 hadStarterBagUpgrade;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    hadStarterBagUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);
    RogueRunStart_PrepareStandard();

    gSpecialVar_0x8007 = RUN_START_TEAM_SOURCE_STARTER_BAG;
    RogueRunStart_SetPreferredTeamSource();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);
    EXPECT(!context->requiresRandomPartner);
    EXPECT(RogueRunStart_CanStart());

    RogueRunStart_Refresh();
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadStarterBagUpgrade);
    FinishRunReviewTest();
}

TEST("Run review: Starting Team forces Starter Bag for an invalid party")
{
    bool8 hadStarterBagUpgrade;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    hadStarterBagUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, TRUE);
    SetRunReviewPartyMon(0, SPECIES_ZIGZAGOON);
    SetRunReviewPartyMon(1, SPECIES_ZIGZAGOON);

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT(!context->canUseCurrentParty);
    EXPECT(context->canUseStarterBag);
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);
    EXPECT(context->requiresRandomPartner);
    EXPECT(RogueRunStart_CanStart());

    RogueRunStart_Clear();
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, FALSE);
    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT(!context->canUseCurrentParty);
    EXPECT(!context->canUseStarterBag);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->readiness, RUN_START_BLOCKED_PARTY);
    EXPECT(!RogueRunStart_CanStart());

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadStarterBagUpgrade);
    FinishRunReviewTest();
}

TEST("Run review: Starting Team Trial rules override the preference")
{
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    SetRunReviewPartyMon(0, SPECIES_TREECKO);
    SetRunReviewPartyMon(1, SPECIES_TORCHIC);
    SelectRunReviewTrial(ROGUE_TRIAL_IRON_MONO, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_PARTY_CAPACITY);
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);

    RogueRunStart_Clear();
    memset(&gPlayerParty[1], 0, sizeof(gPlayerParty[1]));
    CalculatePlayerPartyCount();
    SelectRunReviewTrial(ROGUE_TRIAL_INSANE_MODE, 10, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);
    EXPECT(context->requiresRandomPartner);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_ORRE_STYLE, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    gSpecialVar_0x8007 = RUN_START_TEAM_SOURCE_STARTER_BAG;
    RogueRunStart_SetPreferredTeamSource();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_FIXED_TRIAL);

    FinishRunReviewTest();
}

TEST("Run review: Starting Team restores Current Party when it becomes eligible")
{
    bool8 hadStarterBagUpgrade;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    hadStarterBagUpgrade = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, TRUE);
    SetRunReviewPartyMon(0, SPECIES_ZIGZAGOON);
    SetRunReviewPartyMon(1, SPECIES_ZIGZAGOON);
    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_STARTER_BAG);

    memset(&gPlayerParty[1], 0, sizeof(gPlayerParty[1]));
    CalculatePlayerPartyCount();
    RogueRunStart_Refresh();
    EXPECT_EQ(context->preferredTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT_EQ(context->effectiveTeamSource, RUN_START_TEAM_SOURCE_CURRENT_PARTY);
    EXPECT(context->canUseCurrentParty);

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadStarterBagUpgrade);
    FinishRunReviewTest();
}

TEST("Run review: incompatible replays and locked ascensions cannot start")
{
    struct AdventureReplay *replay;
    ResetRunReviewTestState();
    SetRunReviewPartyMon(0, SPECIES_TREECKO);
    memset(gRogueSaveBlock->bestAscension, ASCENSION_NONE, sizeof(gRogueSaveBlock->bestAscension));
    gRogueSaveBlock->adventureConfig.ascension = 1;
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(RogueRunStart_GetContext()->readinessReason, RUN_START_REASON_ASCENSION_LOCKED);
    EXPECT(!RogueRunStart_CanStart());
    RogueRunStart_Clear();
    replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];
    replay->isValid = TRUE;
    replay->adventureConfig = gRogueSaveBlock->adventureConfig;
    replay->rulesVersion = ASCENSION_RULES_VERSION + 1;
    FlagSet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(RogueRunStart_GetContext()->readinessReason, RUN_START_REASON_INVALID_CONFIG);
    EXPECT(!RogueRunStart_CanStart());
    FinishRunReviewTest();
}

TEST("Run review: Trial editor commits forced rules without overwriting remembered setup")
{
    struct RogueAdventureConfig config, remembered;
    ResetRunReviewTestState();
    Rogue_CopyAdventureConfig(&remembered);
    SelectRunReviewTrial(ROGUE_TRIAL_INSANE_MODE, 10, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    config = RogueRunStart_GetContext()->effectiveConfig;
    config.ascension = 17;
    config.mode = ROGUE_GAME_MODE_GAUNTLET;
    config.battleFormat = BATTLE_FORMAT_SINGLES;
    RogueRunStart_UpdateConfig(&config);
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.ascension, 17);
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.mode, ROGUE_GAME_MODE_STANDARD);
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.battleFormat, BATTLE_FORMAT_MIXED);
    EXPECT_EQ(memcmp(&remembered, &gRogueSaveBlock->adventureConfig, sizeof(remembered)), 0);
    RogueRunStart_Commit();
    EXPECT(gSpecialVar_Result);
    EXPECT_EQ(gRogueSaveBlock->activeAdventureConfig.ascension, 17);
    FinishRunReviewTest();
}

TEST("Run review: Trial replay retains its fixed team and restrictions")
{
    struct AdventureReplay *replay;
    bool8 hadReplay = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_ADVENTURE_REPLAY);
    bool8 hadTrials = FlagGet(FLAG_SYS_TRIALS_UNLOCKED);
    ResetRunReviewTestState();
    FlagSet(FLAG_SYS_TRIALS_UNLOCKED);
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_ADVENTURE_REPLAY, TRUE);
    replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];
    replay->adventureConfig = gRogueSaveBlock->adventureConfig;
    EXPECT(RogueTrial_BuildSelectionConfig(ROGUE_TRIAL_ORRE_STYLE, 5, POKEDEX_VARIANT_NATIONAL_GEN9, &replay->adventureConfig));
    replay->trialId = ROGUE_TRIAL_ORRE_STYLE;
    replay->isValid = TRUE;
    replay->rulesVersion = ASCENSION_RULES_VERSION;
    replay->baseSeed = 0x1234;
    FlagSet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(RogueRunStart_GetContext()->source, RUN_START_SOURCE_REPLAY);
    EXPECT_EQ(RogueRunStart_GetContext()->trialId, ROGUE_TRIAL_ORRE_STYLE);
    EXPECT(RogueTrial_PendingHasFixedStartingPartyValue());
    EXPECT(!RogueRunStart_GetContext()->canEdit);
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.battleFormat, BATTLE_FORMAT_DOUBLES);
    EXPECT(RogueRunStart_CanStart());
    RogueRunStart_Commit();
    EXPECT(gSpecialVar_Result);
    RogueTest_BeginRunReset();
    EXPECT_EQ(gRogueRun.baseSeed, 0x1234);
    EXPECT(FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE));
    EXPECT_EQ(gRogueRun.trialState.trialId, ROGUE_TRIAL_ORRE_STYLE);
    EXPECT_EQ(gRogueSaveBlock->activeRunSource, RUN_START_SOURCE_REPLAY);
    EXPECT(!gRogueSaveBlock->ascensionEligible);
    EXPECT(Rogue_ShouldDisableMainQuests());
    EXPECT(Rogue_ShouldDisableTrialQuests());
    RogueTrial_OnQuestCompleted(QUEST_ID_ORRE_STYLE);
    EXPECT_EQ(gRogueSaveBlock->bestTrialAscension[ROGUE_TRIAL_ORRE_STYLE][BATTLE_FORMAT_DOUBLES], ASCENSION_NONE);
    RogueSave_LoadHubStates();
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_ADVENTURE_REPLAY, hadReplay);
    if (!hadTrials) FlagClear(FLAG_SYS_TRIALS_UNLOCKED);
    FinishRunReviewTest();
}

const u8 *RogueTest_AdventureSetupOption(u8 index, bool8 readOnly);
bool8 RogueTest_AdventurePokedexCanOpen(void);

static bool8 SetupHasOption(const u8 *label, bool8 readOnly)
{
    u8 i;
    const u8 *option;
    for (i = 0; (option = RogueTest_AdventureSetupOption(i, readOnly)) != NULL; ++i)
        if (StringCompare(label, option) == 0) return TRUE;
    return FALSE;
}

TEST("Adventure setup: reveal choices as their unlocks become available")
{
    bool8 hadStarterBag = RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    bool8 hadDex = FlagGet(FLAG_ROGUE_MET_POKABBIE);
    bool8 hadClear = FlagGet(FLAG_IS_CHAMPION);
    bool8 hadIntroduction = FlagGet(FLAG_ROGUE_ASCENSION_INTRODUCED);
    u8 baseText[160];
    const u8 *const initialRows[] = {
        COMPOUND_STRING("Battle format"), COMPOUND_STRING("Pokédex"),
        COMPOUND_STRING("Encounters"),
        COMPOUND_STRING("Base rules"), COMPOUND_STRING("Save and close")
    };
    u8 i;
    ResetRunReviewTestState();
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, FALSE);
    FlagClear(FLAG_ROGUE_MET_POKABBIE);
    FlagClear(FLAG_IS_CHAMPION);
    FlagClear(FLAG_ROGUE_ASCENSION_INTRODUCED);
    for (i = 0; i < ARRAY_COUNT(initialRows); ++i)
    {
        const u8 *option = RogueTest_AdventureSetupOption(i, FALSE);
        EXPECT(option != NULL);
        EXPECT_EQ(StringCompare(option, initialRows[i]), 0);
    }
    EXPECT(RogueTest_AdventureSetupOption(ARRAY_COUNT(initialRows), FALSE) == NULL);
    EXPECT(!SetupHasOption(COMPOUND_STRING("Ascension"), TRUE));
    EXPECT(!RogueAscension_GetBaseLine(6, baseText));
    EXPECT(SetupHasOption(COMPOUND_STRING("Pokédex"), TRUE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Trainers"), TRUE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Trainers"), FALSE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Starting team"), TRUE));

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, TRUE);
    EXPECT(SetupHasOption(COMPOUND_STRING("Starting team"), FALSE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Pokédex"), FALSE));
    EXPECT(!RogueTest_AdventurePokedexCanOpen());
    FlagSet(FLAG_ROGUE_MET_POKABBIE);
    EXPECT(SetupHasOption(COMPOUND_STRING("Trainers"), FALSE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Pokédex"), FALSE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Ascension"), FALSE));
    FlagSet(FLAG_IS_CHAMPION);
    FlagClear(FLAG_ROGUE_MET_POKABBIE);
    EXPECT(RogueTest_AdventurePokedexCanOpen());
    FlagSet(FLAG_ROGUE_MET_POKABBIE);
    EXPECT(!RogueAscension_IsRevealed());
    EXPECT(!SetupHasOption(COMPOUND_STRING("Ascension"), FALSE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Ascension"), TRUE));
    EXPECT(!RogueAscension_GetBaseLine(6, baseText));
    // Before the entrance introduction, Trials silently use the base level.
    gSpecialVar_0x8004 = ROGUE_TRIAL_ORRE_STYLE;
    gSpecialVar_0x8005 = 17;
    RogueTrial_SelectFixedAscension();
    EXPECT_EQ(gSpecialVar_Result, TRUE);
    EXPECT_EQ(gSpecialVar_0x8005, 0);
    FlagSet(FLAG_ROGUE_ASCENSION_INTRODUCED);
    EXPECT(RogueAscension_IsRevealed());
    EXPECT(RogueAscension_GetBaseLine(6, baseText));
    EXPECT(SetupHasOption(COMPOUND_STRING("Ascension"), TRUE));
    RogueTrial_SelectFixedAscension();
    EXPECT_EQ(gSpecialVar_Result, FALSE);
    FlagClear(FLAG_IS_CHAMPION);
    EXPECT(!RogueAscension_IsRevealed());
    FlagSet(FLAG_IS_CHAMPION);
    EXPECT(SetupHasOption(COMPOUND_STRING("Ascension"), FALSE));
    // Once introduced, the system is visible for all three format ladders.
    Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, BATTLE_FORMAT_MIXED);
    EXPECT(SetupHasOption(COMPOUND_STRING("Ascension"), FALSE));

    SetRunReviewPartyMon(0, SPECIES_TREECKO);
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(StringCompare(RogueTest_AdventureSetupOption(0, FALSE), COMPOUND_STRING("Begin Adventure")), 0);
    EXPECT(SetupHasOption(COMPOUND_STRING("Starting team"), FALSE));
    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, FALSE);
    RogueRunStart_Refresh();
    EXPECT(!SetupHasOption(COMPOUND_STRING("Starting team"), FALSE));

    RogueHub_SetUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER, hadStarterBag);
    if (!hadDex) FlagClear(FLAG_ROGUE_MET_POKABBIE);
    if (!hadClear) FlagClear(FLAG_IS_CHAMPION);
    if (!hadIntroduction) FlagClear(FLAG_ROGUE_ASCENSION_INTRODUCED);
    FinishRunReviewTest();
}

TEST("Adventure setup: hide Trial-fixed choices but retain read-only rules")
{
    bool8 hadPostgame = FlagGet(FLAG_ROGUE_MET_POKABBIE);
    ResetRunReviewTestState();
    FlagSet(FLAG_ROGUE_MET_POKABBIE);
    SelectRunReviewTrial(ROGUE_TRIAL_ORRE_STYLE, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueRunStart_GetContext()->trialId, ROGUE_TRIAL_ORRE_STYLE);
    EXPECT(!SetupHasOption(COMPOUND_STRING("Adventure"), FALSE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Battle format"), FALSE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Starting team"), FALSE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Trainers"), FALSE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Adventure"), TRUE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Battle format"), TRUE));
    EXPECT(SetupHasOption(COMPOUND_STRING("Starting team"), TRUE));
    if (!hadPostgame) FlagClear(FLAG_ROGUE_MET_POKABBIE);
    FinishRunReviewTest();
}

TEST("Adventure setup: Gauntlet unlocks postgame and Slow Path cannot start new runs")
{
    bool8 hadDex = FlagGet(FLAG_ROGUE_MET_POKABBIE);
    struct RogueQuestState previousFinalQuest = gRogueSaveBlock->questStates[QUEST_ID_ONE_LAST_QUEST];
    struct RogueAdventureConfig candidate;
    ResetRunReviewTestState();
    FlagClear(FLAG_ROGUE_MET_POKABBIE);
    RogueQuest_SetStateFlag(QUEST_ID_ONE_LAST_QUEST, QUEST_STATE_HAS_COMPLETE, FALSE);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);
    EXPECT(Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_STANDARD));
    EXPECT(!Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_GAUNTLET));
    EXPECT(!Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_SLOW_PATH));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Adventure"), FALSE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Trainers"), FALSE));

    // A remembered, unavailable mode falls back without altering the hub choice
    // until the player accepts setup.
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_GAUNTLET);
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.mode, ROGUE_GAME_MODE_STANDARD);
    EXPECT_EQ(gRogueSaveBlock->adventureConfig.mode, ROGUE_GAME_MODE_GAUNTLET);
    Rogue_CopyAdventureConfig(&candidate);
    candidate.mode = ROGUE_GAME_MODE_GAUNTLET;
    RogueRunStart_UpdateConfig(&candidate);
    EXPECT(!RogueRunStart_CanStart());
    RogueRunStart_Clear();

    FlagSet(FLAG_ROGUE_MET_POKABBIE);
    EXPECT(Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_GAUNTLET));
    EXPECT(SetupHasOption(COMPOUND_STRING("Adventure"), FALSE));
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.mode, ROGUE_GAME_MODE_GAUNTLET);
    EXPECT(RogueRunStart_CanStart());
    Rogue_CopyAdventureConfig(&candidate);
    candidate.mode = ROGUE_GAME_MODE_SLOW_PATH;
    RogueRunStart_UpdateConfig(&candidate);
    EXPECT(!RogueRunStart_CanStart());
    RogueRunStart_Clear();
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_SLOW_PATH);
    RogueRunStart_PrepareStandard();
    EXPECT_EQ(RogueRunStart_GetContext()->effectiveConfig.mode, ROGUE_GAME_MODE_STANDARD);
    EXPECT(RogueRunStart_CanStart());
    RogueRunStart_Clear();

    // Keep the legacy final-quest shortcut to the postgame unlock.
    FlagClear(FLAG_ROGUE_MET_POKABBIE);
    RogueQuest_SetStateFlag(QUEST_ID_ONE_LAST_QUEST, QUEST_STATE_UNLOCKED, TRUE);
    RogueQuest_SetStateFlag(QUEST_ID_ONE_LAST_QUEST, QUEST_STATE_HAS_COMPLETE, TRUE);
    RogueQuest_SetStateFlag(QUEST_ID_ONE_LAST_QUEST, QUEST_STATE_PENDING_REWARDS, FALSE);
    EXPECT(Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_GAUNTLET));
    EXPECT(SetupHasOption(COMPOUND_STRING("Trainers"), FALSE));
    EXPECT(!Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_SLOW_PATH));
    gRogueSaveBlock->questStates[QUEST_ID_ONE_LAST_QUEST] = previousFinalQuest;
    if (hadDex) FlagSet(FLAG_ROGUE_MET_POKABBIE);
    Rogue_SetConfigRange(CONFIG_RANGE_GAME_MODE_NUM, ROGUE_GAME_MODE_STANDARD);
    FinishRunReviewTest();
}

TEST("Run review: remembered Trials expose Choose Trial only in editable Trial setup")
{
    bool8 hadTrials = FlagGet(FLAG_SYS_TRIALS_UNLOCKED);
    ResetRunReviewTestState();
    FlagSet(FLAG_SYS_TRIALS_UNLOCKED);
    RogueQuest_TryUnlockQuest(QUEST_ID_ORRE_STYLE);
    EXPECT(RogueTrial_CommitSelection(ROGUE_TRIAL_ORRE_STYLE, 5, POKEDEX_VARIANT_NATIONAL_GEN9));
    RogueTrial_ApplyPendingSelection();
    RogueRunStart_Clear();
    RogueTrial_LoadLastSelection();
    EXPECT(gSpecialVar_Result);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueRunStart_GetContext()->trialId, ROGUE_TRIAL_ORRE_STYLE);
    EXPECT(SetupHasOption(COMPOUND_STRING("Choose Trial"), FALSE));
    EXPECT(!SetupHasOption(COMPOUND_STRING("Choose Trial"), TRUE));
    EXPECT_EQ(StringCompare(RogueTest_AdventureSetupOption(0, FALSE), COMPOUND_STRING("Begin Adventure")), 0);
    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LITTLE_CUP, 0, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueRunStart_GetContext()->trialId, ROGUE_TRIAL_LITTLE_CUP);
    RogueRunStart_Clear();
    RogueRunStart_PrepareStandard();
    EXPECT(!SetupHasOption(COMPOUND_STRING("Choose Trial"), FALSE));
    if (!hadTrials) FlagClear(FLAG_SYS_TRIALS_UNLOCKED);
    FinishRunReviewTest();
}

void Rogue_ShouldNursePromptConfigLabSettingsChange(void);

TEST("Run review: nurse reminders require revealed and available ascensions")
{
    bool8 hadClear = FlagGet(FLAG_IS_CHAMPION);
    bool8 hadIntroduction = FlagGet(FLAG_ROGUE_ASCENSION_INTRODUCED);
    u32 wins = GetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK);
    u32 losses = GetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK);
    ResetRunReviewTestState();
    SetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK, 0);
    SetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK, 5);
    FlagClear(FLAG_IS_CHAMPION);
    FlagClear(FLAG_ROGUE_ASCENSION_INTRODUCED);
    Rogue_ShouldNursePromptConfigLabSettingsChange();
    EXPECT(!gSpecialVar_Result);
    FlagSet(FLAG_IS_CHAMPION);
    Rogue_ShouldNursePromptConfigLabSettingsChange();
    EXPECT(!gSpecialVar_Result);
    FlagSet(FLAG_ROGUE_ASCENSION_INTRODUCED);
    Rogue_ShouldNursePromptConfigLabSettingsChange();
    EXPECT(!gSpecialVar_Result); // A0 has no lower level.
    Rogue_SetConfigRange(CONFIG_RANGE_ASCENSION, 5);
    Rogue_ShouldNursePromptConfigLabSettingsChange();
    EXPECT(gSpecialVar_Result);
    EXPECT_EQ(gSpecialVar_0x8004, 0);
    SetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK, 0);
    SetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK, 5);
    Rogue_ShouldNursePromptConfigLabSettingsChange();
    EXPECT(gSpecialVar_Result);
    EXPECT_EQ(gSpecialVar_0x8004, 1);
    Rogue_SetConfigRange(CONFIG_RANGE_ASCENSION, 20);
    Rogue_ShouldNursePromptConfigLabSettingsChange();
    EXPECT(!gSpecialVar_Result);
    SetGameStat(GAME_STAT_CURRENT_RUN_WIN_STREAK, wins);
    SetGameStat(GAME_STAT_CURRENT_RUN_LOSS_STREAK, losses);
    if (!hadClear) FlagClear(FLAG_IS_CHAMPION);
    if (!hadIntroduction) FlagClear(FLAG_ROGUE_ASCENSION_INTRODUCED);
    FinishRunReviewTest();
}
