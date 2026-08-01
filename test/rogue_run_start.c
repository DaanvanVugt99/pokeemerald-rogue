#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"
#include "constants/rogue_pokedex.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_multiplayer.h"
#include "rogue_pokedex.h"
#include "rogue_run_start.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_trials.h"
#include "test/test.h"

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
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_GEN9);
    gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED].isValid = FALSE;
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
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_HARD);
    Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, BATTLE_FORMAT_DOUBLES);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();

    EXPECT(context != NULL);
    EXPECT_EQ(context->source, RUN_START_SOURCE_NORMAL);
    EXPECT_EQ(context->effectiveConfig.rangeValues[CONFIG_RANGE_TRAINER], DIFFICULTY_LEVEL_HARD);
    EXPECT_EQ(context->effectiveConfig.rangeValues[CONFIG_RANGE_BATTLE_FORMAT], BATTLE_FORMAT_DOUBLES);
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
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER, DIFFICULTY_LEVEL_EASY);
    replay = &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED];
    memcpy(&replay->difficultyConfig, &gRogueSaveBlock->difficultyConfig, sizeof(replay->difficultyConfig));
    replay->difficultyConfig.rangeValues[CONFIG_RANGE_TRAINER] = DIFFICULTY_LEVEL_BRUTAL;
    replay->difficultyConfig.rangeValues[CONFIG_RANGE_BATTLE_FORMAT] = BATTLE_FORMAT_MIXED;
    replay->isValid = TRUE;
    FlagSet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();

    EXPECT(context != NULL);
    EXPECT_EQ(context->source, RUN_START_SOURCE_REPLAY);
    EXPECT(!context->canEdit);
    EXPECT(context->mainQuestsDisabled);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER), DIFFICULTY_LEVEL_BRUTAL);
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
    memcpy(&multiplayer.gameState.hub.difficultyConfig,
           &gRogueSaveBlock->difficultyConfig,
           sizeof(multiplayer.gameState.hub.difficultyConfig));
    multiplayer.gameState.hub.difficultyConfig.rangeValues[CONFIG_RANGE_TRAINER] = DIFFICULTY_LEVEL_HARD;
    gRogueMultiplayer = &multiplayer;
    SetRunReviewPartyMon(0, SPECIES_TREECKO);

    RogueRunStart_PrepareStandard();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->source, RUN_START_SOURCE_MULTIPLAYER_CLIENT);
    EXPECT(!context->canEdit);
    EXPECT_EQ(context->readiness, RUN_START_WAITING_FOR_HOST);
    EXPECT(!RogueRunStart_CanStart());
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER), DIFFICULTY_LEVEL_HARD);

    multiplayer.gameState.adventure.isRunActive = TRUE;
    multiplayer.gameState.hub.difficultyConfig.rangeValues[CONFIG_RANGE_TRAINER] = DIFFICULTY_LEVEL_BRUTAL;
    multiplayer.gameState.hub.difficultyConfig.rangeValues[CONFIG_RANGE_POKEDEX_VARIANT] = POKEDEX_VARIANT_HOENN_RSE;
    RogueRunStart_Refresh();
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER), DIFFICULTY_LEVEL_BRUTAL);
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
    struct RogueDifficultyConfig expected;
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    Rogue_CopyReadableDifficultyConfig(&expected);
    EXPECT(RogueTrial_BuildSelectionConfig(
        ROGUE_TRIAL_Z_A_ROYALE,
        DIFFICULTY_LEVEL_AVERAGE,
        POKEDEX_VARIANT_LEGENDS_ZAFULLDLC,
        &expected));

    SelectRunReviewTrial(ROGUE_TRIAL_Z_A_ROYALE, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_LEGENDS_ZAFULLDLC);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT(context != NULL);
    EXPECT(memcmp(&context->effectiveConfig, &expected, sizeof(expected)) == 0);

    RogueRunStart_Commit();
    EXPECT(gSpecialVar_Result);
    RogueTrial_ApplyPendingSelection();
    RogueRunStart_Clear();
    EXPECT(memcmp(&gRogueSaveBlock->difficultyConfig, &expected, sizeof(expected)) == 0);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER_ORDER), TRAINER_ORDER_RAINBOW);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT), BATTLE_FORMAT_DOUBLES);
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS));

    FinishRunReviewTest();
}
#endif

TEST("Run review: forced Trial settings are present in the preview")
{
    struct RogueDifficultyConfig config;

    ResetRunReviewTestState();
    Rogue_CopyReadableDifficultyConfig(&config);
    EXPECT(!RogueTrial_BuildSelectionConfig(
        ROGUE_TRIAL_INSANE_MODE,
        DIFFICULTY_LEVEL_AVERAGE,
        POKEDEX_VARIANT_NATIONAL_GEN9,
        &config));
    EXPECT(RogueTrial_BuildSelectionConfig(
        ROGUE_TRIAL_INSANE_MODE,
        DIFFICULTY_LEVEL_HARD,
        POKEDEX_VARIANT_NATIONAL_GEN9,
        &config));
    EXPECT_EQ(config.rangeValues[CONFIG_RANGE_TRAINER], DIFFICULTY_LEVEL_HARD);
    EXPECT_EQ(config.rangeValues[CONFIG_RANGE_DIFFICULTY_PRESET], DIFFICULTY_LEVEL_CUSTOM);
    EXPECT_EQ(config.rangeValues[CONFIG_RANGE_BATTLE_FORMAT], BATTLE_FORMAT_MIXED);
    EXPECT(config.toggleBits[CONFIG_TOGGLE_BAG_WIPE / 8] & (1 << (CONFIG_TOGGLE_BAG_WIPE % 8)));

    FinishRunReviewTest();
}

TEST("Run review: readiness distinguishes recovery, Day Care, and fixed parties")
{
    const struct RogueRunStartContext *context;

    ResetRunReviewTestState();
    SetRunReviewPartyMon(0, SPECIES_ZIGZAGOON);
    SelectRunReviewTrial(ROGUE_TRIAL_TYPE_WATER, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
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
    SelectRunReviewTrial(ROGUE_TRIAL_ORRE_STYLE, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
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
    SelectRunReviewTrial(ROGUE_TRIAL_IRON_MONO, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    context = RogueRunStart_GetContext();
    EXPECT_EQ(context->readiness, RUN_START_READY_REQUIRES_PARTNER);
    EXPECT_EQ(context->readinessReason, RUN_START_REASON_PARTY_CAPACITY);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_INSANE_MODE, DIFFICULTY_LEVEL_HARD, POKEDEX_VARIANT_NATIONAL_GEN9);
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

    SelectRunReviewTrial(ROGUE_TRIAL_TYPE_WATER, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_ZIGZAGOON, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_TYPE);
    EXPECT_EQ(param, TYPE_WATER);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LOW_BST, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_SLAKING, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_BST);
    EXPECT_EQ(param, 400);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LITTLE_CUP, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_CHARIZARD, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_LITTLE_CUP);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_CANT_PICK, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_ZIGZAGOON, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_STARTER_FAMILY);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_INSANE_MODE, DIFFICULTY_LEVEL_HARD, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_MEWTWO, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_LEGENDARY_FORBIDDEN);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_APOTHEOSIS, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_ZIGZAGOON, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_LEGENDARY_REQUIRED);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_LIMITED_CAPTURE, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_TREECKO, TRUE, &param), ROGUE_TRIAL_ELIGIBILITY_DAY_CARE_DISABLED);

    RogueRunStart_Clear();
    SelectRunReviewTrial(ROGUE_TRIAL_CURSED_BODY, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_HOENN_RSE);
    RogueRunStart_PrepareTrial();
    EXPECT_EQ(RogueTrial_GetPendingSpeciesEligibilityReason(SPECIES_BULBASAUR, FALSE, &param), ROGUE_TRIAL_ELIGIBILITY_POKEDEX);
    EXPECT_EQ(param, POKEDEX_VARIANT_HOENN_RSE);

    FinishRunReviewTest();
}

TEST("Run review: clearing removes staged settings and pending Trial state")
{
    ResetRunReviewTestState();
    SelectRunReviewTrial(ROGUE_TRIAL_TYPE_WATER, DIFFICULTY_LEVEL_AVERAGE, POKEDEX_VARIANT_NATIONAL_GEN9);
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
