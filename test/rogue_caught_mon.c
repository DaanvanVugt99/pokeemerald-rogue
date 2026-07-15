#include "global.h"
#include "battle.h"
#include "constants/battle.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/rogue_pokedex.h"
#include "constants/species.h"
#include "constants/vars.h"
#include "event_data.h"
#include "pokemon.h"
#include "random.h"
#include "rogue.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_trials.h"
#include "test/test.h"

static void ResetCaughtMonTestState(void)
{
    u8 i;

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    memset(gEnemyParty, 0, sizeof(gEnemyParty));
    for (i = 0; i < DAYCARE_SLOT_COUNT; ++i)
        ZeroBoxMonData(Rogue_GetDaycareBoxMon(i));
    gPlayerPartyCount = 0;
    gEnemyPartyCount = 0;
    VarSet(VAR_STARTER_SWAP_SPECIES, SPECIES_NONE);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_HELD_ITEM_CLAUSE, FALSE);
    RogueTrial_ClearPendingSelection();
    gRogueSaveBlock->hasLastTrialSelection = FALSE;
}

static void ClearCaughtMonTestState(void)
{
    u8 i;

    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_HELD_ITEM_CLAUSE, FALSE);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    memset(gEnemyParty, 0, sizeof(gEnemyParty));
    for (i = 0; i < DAYCARE_SLOT_COUNT; ++i)
        ZeroBoxMonData(Rogue_GetDaycareBoxMon(i));
    gPlayerPartyCount = 0;
    gEnemyPartyCount = 0;
    VarSet(VAR_STARTER_SWAP_SPECIES, SPECIES_NONE);
    RogueTrial_ClearPendingSelection();
}

static void SetPartyMon(u8 slot, u16 species)
{
    CreateMon(&gPlayerParty[slot], species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();
}

static void SetEnemyMon(u8 slot, u16 species)
{
    CreateMon(&gEnemyParty[slot], species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculateEnemyPartyCount();
}

static void SetDaycareMon(u8 slot, u16 species)
{
    struct Pokemon mon;

    CreateMon(&mon, species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CopyMon(Rogue_GetDaycareBoxMon(slot), &mon.box, sizeof(struct BoxPokemon));
}

static struct Pokemon CreateCaughtMon(u16 species)
{
    struct Pokemon mon;

    CreateMon(&mon, species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    return mon;
}

static void ActivateCaughtMonTestTrial(u8 trialId)
{
    gRogueRun.trialState.trialId = trialId;
    gRogueRun.trialState.invalidated = FALSE;
    gRogueRun.trialState.initialPartyCountSet = TRUE;
}

static void SetPendingTrialSelection(u8 trialId, u8 pokedexVariant)
{
    gSpecialVar_0x8004 = trialId;
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_AVERAGE;
    gSpecialVar_0x8006 = pokedexVariant;
    RogueTrial_SetPendingSelectionFromScript();
    EXPECT(gSpecialVar_Result);
    EXPECT_EQ(RoguePokedex_GetDexVariant(), pokedexVariant);
}

static u16 GetGeneratedStarter(u8 slot)
{
    switch (slot)
    {
    case 0:
        return VarGet(VAR_ROGUE_STARTER0);
    case 1:
        return VarGet(VAR_ROGUE_STARTER1);
    case 2:
        return VarGet(VAR_ROGUE_STARTER2);
    }

    return SPECIES_NONE;
}

static void ExpectGeneratedStartersAllowedByPendingTrial(void)
{
    u8 i;

    for (i = 0; i < 3; ++i)
    {
        u16 species = GetGeneratedStarter(i);

        EXPECT(species != SPECIES_NONE);
        EXPECT(RogueTrial_PendingAllowsSpecies(species));
    }
}

static void ExpectGeneratedStartersDistinct(void)
{
    EXPECT_NE(GetGeneratedStarter(0), GetGeneratedStarter(1));
    EXPECT_NE(GetGeneratedStarter(0), GetGeneratedStarter(2));
    EXPECT_NE(GetGeneratedStarter(1), GetGeneratedStarter(2));
}

TEST("Species Clause duplicate catches with party room require releasing the matching evo chain")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    SetPartyMon(0, SPECIES_METAPOD);
    SetPartyMon(1, SPECIES_POOCHYENA);
    caughtMon = CreateCaughtMon(SPECIES_BUTTERFREE);

    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));
    EXPECT(Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 0));
    EXPECT(!Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 1));

    ClearCaughtMonTestState();
}

TEST("Species Clause off allows duplicate catches when the party has room")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, FALSE);
    SetPartyMon(0, SPECIES_METAPOD);
    caughtMon = CreateCaughtMon(SPECIES_BUTTERFREE);

    EXPECT(Rogue_CanAddCaughtMonToParty(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("Full party catches without a Species Clause conflict can release any party member")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    SetPartyMon(0, SPECIES_TREECKO);
    SetPartyMon(1, SPECIES_TORCHIC);
    SetPartyMon(2, SPECIES_MUDKIP);
    SetPartyMon(3, SPECIES_POOCHYENA);
    SetPartyMon(4, SPECIES_TAILLOW);
    SetPartyMon(5, SPECIES_RALTS);
    caughtMon = CreateCaughtMon(SPECIES_SHROOMISH);

    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));
    EXPECT(Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 0));
    EXPECT(Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 3));

    ClearCaughtMonTestState();
}

TEST("Type Trial catches must match the selected type")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_TYPE_WATER);

    caughtMon = CreateCaughtMon(SPECIES_MAGIKARP);
    EXPECT(Rogue_CanAddCaughtMonToParty(&caughtMon));

    caughtMon = CreateCaughtMon(SPECIES_ZIGZAGOON);
    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("Little Cup Trial catches must be first-stage Pokemon that can evolve")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LITTLE_CUP);

    caughtMon = CreateCaughtMon(SPECIES_MAGIKARP);
    EXPECT(Rogue_CanAddCaughtMonToParty(&caughtMon));

    caughtMon = CreateCaughtMon(SPECIES_GYARADOS);
    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));

    caughtMon = CreateCaughtMon(SPECIES_BRUXISH);
    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("BST Crown Trial rejects catches above 400 BST")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LOW_BST);

    caughtMon = CreateCaughtMon(SPECIES_ZIGZAGOON);
    EXPECT(Rogue_CanAddCaughtMonToParty(&caughtMon));

    caughtMon = CreateCaughtMon(SPECIES_ABSOL);
    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("Trial legality invalidates trainer battles with illegal party members")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_TYPE_WATER);

    SetPartyMon(0, SPECIES_MAGIKARP);
    RogueTrial_OnTrainerBattleStart();
    EXPECT(!RogueTrial_IsInvalidated());

    SetPartyMon(1, SPECIES_ZIGZAGOON);
    RogueTrial_OnTrainerBattleStart();
    EXPECT(RogueTrial_IsInvalidated());

    ClearCaughtMonTestState();
}

TEST("Trial legality blocks trainer battles with illegal party members")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LITTLE_CUP);

    SetPartyMon(0, SPECIES_MAGIKARP);
    RogueTrial_CanStartTrainerBattle();
    EXPECT(gSpecialVar_Result == TRUE);

    SetPartyMon(1, SPECIES_GYARADOS);
    RogueTrial_CanStartTrainerBattle();
    EXPECT(gSpecialVar_Result == FALSE);

    ClearCaughtMonTestState();
}

TEST("Little Cup and BST Crown block illegal evolutions")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LITTLE_CUP);
    EXPECT(!RogueTrial_IsSpeciesLegal(SPECIES_GYARADOS, 0));

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LOW_BST);
    EXPECT(!RogueTrial_IsSpeciesLegal(SPECIES_LINOONE, 0));
    EXPECT(RogueTrial_IsSpeciesLegal(SPECIES_METAPOD, 0));

    ClearCaughtMonTestState();
}

TEST("Trial generated Pokemon are transformed when they are illegal")
{
    struct Pokemon generatedMon;

    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_TYPE_WATER);
    generatedMon = CreateCaughtMon(SPECIES_ZIGZAGOON);

    EXPECT(!RogueTrial_CanAcceptMon(&generatedMon));
    EXPECT(RogueTrial_TransformMonIfIllegal(&generatedMon));
    EXPECT(RogueTrial_CanAcceptMon(&generatedMon));
    EXPECT(!RogueTrial_IsInvalidated());

    ClearCaughtMonTestState();
}

TEST("Regional Style Trial treats selected Pokedex as species legality")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_HOENN_RSE);
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_REGION_HOENN);

    caughtMon = CreateCaughtMon(SPECIES_TREECKO);
    EXPECT(Rogue_CanAddCaughtMonToParty(&caughtMon));

    caughtMon = CreateCaughtMon(SPECIES_BULBASAUR);
    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("Pending Type Trial blocks illegal party Pokemon before start")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_TYPE_WATER, POKEDEX_VARIANT_NATIONAL_GEN9);

    SetPartyMon(0, SPECIES_ZIGZAGOON);
    RogueTrial_CanUsePendingParty();
    EXPECT(!gSpecialVar_Result);
    RogueTrial_CanStartPendingSelection();
    EXPECT(!gSpecialVar_Result);

    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CalculatePlayerPartyCount();

    SetPartyMon(0, SPECIES_MAGIKARP);
    RogueTrial_CanUsePendingParty();
    EXPECT(gSpecialVar_Result);
    RogueTrial_CanStartPendingSelection();
    EXPECT(gSpecialVar_Result);

    ClearCaughtMonTestState();
}

TEST("Pending trial checks Day Care Pokemon before start")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_TYPE_WATER, POKEDEX_VARIANT_NATIONAL_GEN9);
    SetPartyMon(0, SPECIES_MAGIKARP);

    SetDaycareMon(0, SPECIES_ZIGZAGOON);
    RogueTrial_CanUsePendingDayCare();
    EXPECT(!gSpecialVar_Result);
    RogueTrial_CanStartPendingSelection();
    EXPECT(!gSpecialVar_Result);

    ZeroBoxMonData(Rogue_GetDaycareBoxMon(0));
    SetDaycareMon(0, SPECIES_MAGIKARP);
    RogueTrial_CanUsePendingDayCare();
    EXPECT(gSpecialVar_Result);
    RogueTrial_CanStartPendingSelection();
    EXPECT(gSpecialVar_Result);

    ClearCaughtMonTestState();
}

TEST("Pending regional trial blocks party Pokemon outside selected Pokedex")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_REGION_HOENN, POKEDEX_VARIANT_HOENN_RSE);

    SetPartyMon(0, SPECIES_BULBASAUR);
    RogueTrial_CanStartPendingSelection();
    EXPECT(!gSpecialVar_Result);

    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CalculatePlayerPartyCount();

    SetPartyMon(0, SPECIES_TREECKO);
    RogueTrial_CanStartPendingSelection();
    EXPECT(gSpecialVar_Result);

    ClearCaughtMonTestState();
}

TEST("Pending trial validates chosen partner instead of replaced party")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_TYPE_WATER, POKEDEX_VARIANT_NATIONAL_GEN9);
    SetPartyMon(0, SPECIES_ZIGZAGOON);

    VarSet(VAR_STARTER_SWAP_SPECIES, SPECIES_MAGIKARP);
    RogueTrial_CanStartPendingSelection();
    EXPECT(gSpecialVar_Result);

    VarSet(VAR_STARTER_SWAP_SPECIES, SPECIES_ZIGZAGOON);
    RogueTrial_CanStartPendingSelection();
    EXPECT(!gSpecialVar_Result);

    ClearCaughtMonTestState();
}

TEST("Normal runs reject party and Day Care Pokemon outside the selected Pokedex")
{
    ResetCaughtMonTestState();
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_HOENN_RSE);

    SetPartyMon(0, SPECIES_BULBASAUR);
    RogueTrial_CanUsePendingParty();
    EXPECT(!gSpecialVar_Result);

    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    CalculatePlayerPartyCount();
    SetPartyMon(0, SPECIES_TREECKO);
    RogueTrial_CanUsePendingParty();
    EXPECT(gSpecialVar_Result);

    SetDaycareMon(0, SPECIES_BULBASAUR);
    RogueTrial_CanUsePendingDayCare();
    EXPECT(!gSpecialVar_Result);

    ZeroBoxMonData(Rogue_GetDaycareBoxMon(0));
    SetDaycareMon(0, SPECIES_TREECKO);
    RogueTrial_CanUsePendingDayCare();
    EXPECT(gSpecialVar_Result);

    ClearCaughtMonTestState();
}

TEST("Pending Type Trial randomizes only legal starter choices")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_TYPE_WATER, POKEDEX_VARIANT_ROGUE_MODERN);

    Rogue_RandomiseStarters();
    ExpectGeneratedStartersAllowedByPendingTrial();
    ExpectGeneratedStartersDistinct();

    ClearCaughtMonTestState();
}

TEST("Pending Normal Type Trial handles move-based evolutions when generating starters")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_TYPE_NORMAL, POKEDEX_VARIANT_NATIONAL_GEN9);

    Rogue_RandomiseStarters();
    ExpectGeneratedStartersAllowedByPendingTrial();
    ExpectGeneratedStartersDistinct();

    ClearCaughtMonTestState();
}

TEST("Shrine guardian replaces Whirlwind with Tailwind")
{
    bool8 sawTailwind = FALSE;
    u16 seed;
    u8 moveSlot;

    ResetCaughtMonTestState();

    for(seed = 0; seed < 128; ++seed)
    {
        SeedRng(seed);
        Rogue_PrepareShrineChallenge();

        for(moveSlot = 0; moveSlot < MAX_MON_MOVES; ++moveSlot)
        {
            u16 move = GetMonData(&gEnemyParty[0], MON_DATA_MOVE1 + moveSlot);

            EXPECT_NE(move, MOVE_WHIRLWIND);
            if(move == MOVE_TAILWIND)
                sawTailwind = TRUE;
        }
    }

    EXPECT(sawTailwind);

    gBattleOutcome = B_OUTCOME_WON;
    Rogue_Battle_EndWildBattle();
    ClearCaughtMonTestState();
}

TEST("Pending Little Cup Trial randomizes only legal starter choices")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_LITTLE_CUP, POKEDEX_VARIANT_ROGUE_MODERN);

    Rogue_RandomiseStarters();
    ExpectGeneratedStartersAllowedByPendingTrial();

    ClearCaughtMonTestState();
}

TEST("Pending Low-BST Trial randomizes only legal starter choices")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_LOW_BST, POKEDEX_VARIANT_ROGUE_MODERN);

    Rogue_RandomiseStarters();
    ExpectGeneratedStartersAllowedByPendingTrial();

    ClearCaughtMonTestState();
}

TEST("Pending regional trial randomizes starters from the selected Pokedex")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_REGION_HOENN, POKEDEX_VARIANT_HOENN_RSE);

    Rogue_RandomiseStarters();
    ExpectGeneratedStartersAllowedByPendingTrial();

    ClearCaughtMonTestState();
}

TEST("Region Style Trial applies selected regional dex and trainer pool")
{
    ResetCaughtMonTestState();
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_ROGUE_MODERN);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, FALSE);

    gSpecialVar_0x8004 = ROGUE_TRIAL_REGION_HOENN;
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_AVERAGE;
    gSpecialVar_0x8006 = POKEDEX_VARIANT_HOENN_RSE;
    RogueTrial_SetPendingSelectionFromScript();
    RogueTrial_ApplyPendingSelection();

    EXPECT_EQ(gRogueRun.trialState.trialId, ROGUE_TRIAL_REGION_HOENN);
    EXPECT_EQ(RoguePokedex_GetDexVariant(), POKEDEX_VARIANT_HOENN_RSE);
    EXPECT(!Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE));
    EXPECT(!Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO));
    EXPECT(!Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN));

    ClearCaughtMonTestState();
}

#ifdef ROGUE_EXPANSION
TEST("Z-A Royale Trial applies Z-A dex, Rainbow order, Doubles, and regional trainer pools")
{
    ResetCaughtMonTestState();
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_ROGUE_MODERN);
    Rogue_SetConfigRange(CONFIG_RANGE_TRAINER_ORDER, TRAINER_ORDER_DEFAULT);
    Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, BATTLE_FORMAT_SINGLES);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA, FALSE);

    gSpecialVar_0x8004 = ROGUE_TRIAL_Z_A_ROYALE;
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_AVERAGE;
    gSpecialVar_0x8006 = POKEDEX_VARIANT_LEGENDS_ZA;
    RogueTrial_SetPendingSelectionFromScript();
    RogueTrial_ApplyPendingSelection();

    EXPECT_EQ(gRogueRun.trialState.trialId, ROGUE_TRIAL_Z_A_ROYALE);
    EXPECT_EQ(RoguePokedex_GetDexVariant(), POKEDEX_VARIANT_LEGENDS_ZA);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_TRAINER_ORDER), TRAINER_ORDER_RAINBOW);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT), BATTLE_FORMAT_DOUBLES);
    EXPECT(!Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR));
    EXPECT(Rogue_GetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA));

    ClearCaughtMonTestState();
}
#endif

TEST("Orre Style Trial applies Doubles and Snag Curse")
{
    ResetCaughtMonTestState();
    Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, BATTLE_FORMAT_SINGLES);

    gSpecialVar_0x8004 = ROGUE_TRIAL_ORRE_STYLE;
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_AVERAGE;
    gSpecialVar_0x8006 = POKEDEX_VARIANT_ROGUE_CLASSICPLUS;
    RogueTrial_SetPendingSelectionFromScript();
    RogueTrial_ApplyPendingSelection();

    EXPECT_EQ(gRogueRun.trialState.trialId, ROGUE_TRIAL_ORRE_STYLE);
    EXPECT_EQ(Rogue_GetConfigRange(CONFIG_RANGE_BATTLE_FORMAT), BATTLE_FORMAT_DOUBLES);
    EXPECT_EQ(RogueTrial_GetCurseItemCount(EFFECT_SNAG_TRAINER_MON), 1);

    ClearCaughtMonTestState();
}

TEST("Converted curse Trials apply their exact granular Curse rules")
{
    ResetCaughtMonTestState();

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_ROGUELOCKE);
    EXPECT_EQ(RogueTrial_GetCurseItemCount(EFFECT_WILD_ENCOUNTER_COUNT), 10);

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_CURSED_BODY);
    EXPECT_EQ(RogueTrial_GetCurseItemCount(EFFECT_SNOWBALL_CURSES), 1);

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_PRO_BUILDING);
    EXPECT_EQ(RogueTrial_GetCurseItemCount(EFFECT_AUTO_MOVE_SELECT), 1);

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_IRON_KAIZO);
    EXPECT_EQ(RogueTrial_GetCurseItemCount(EFFECT_PARTY_SIZE), 5);
    EXPECT_EQ(RogueTrial_GetCurseItemCount(EFFECT_SHOP_PRICE), 99);

    ClearCaughtMonTestState();
}

TEST("Converted legality Trials enforce starter and Legendary rules")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_CANT_PICK);
    EXPECT(RogueTrial_IsSpeciesLegal(SPECIES_CHARIZARD, 0));
    EXPECT(!RogueTrial_IsSpeciesLegal(SPECIES_RATTATA, 0));
    caughtMon = CreateCaughtMon(SPECIES_RATTATA);
    EXPECT(!RogueTrial_CanAcceptMon(&caughtMon));

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_INSANE_MODE);
    EXPECT(RogueTrial_IsSpeciesLegal(SPECIES_RATTATA, 0));
    EXPECT(!RogueTrial_IsSpeciesLegal(SPECIES_MEWTWO, 0));
    caughtMon = CreateCaughtMon(SPECIES_MEWTWO);
    EXPECT(!RogueTrial_CanAcceptMon(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("Insane Mode forces Hard difficulty")
{
    ResetCaughtMonTestState();

    gSpecialVar_0x8004 = ROGUE_TRIAL_INSANE_MODE;
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_AVERAGE;
    gSpecialVar_0x8006 = POKEDEX_VARIANT_NATIONAL_GEN9;
    RogueTrial_SelectForcedDifficulty();
    EXPECT(gSpecialVar_Result);
    EXPECT_EQ(gSpecialVar_0x8005, DIFFICULTY_LEVEL_HARD);

    RogueTrial_SetPendingSelectionFromScript();
    EXPECT(gSpecialVar_Result);

    RogueTrial_ClearPendingSelection();
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_AVERAGE;
    RogueTrial_SetPendingSelectionFromScript();
    EXPECT(!gSpecialVar_Result);

    ClearCaughtMonTestState();
}

TEST("Last Trial selection restores its exact setup")
{
    ResetCaughtMonTestState();
    RogueQuest_TryUnlockQuest(QUEST_ID_NORMAL_MASTER);
    SetPendingTrialSelection(ROGUE_TRIAL_TYPE_NORMAL, POKEDEX_VARIANT_NATIONAL_GEN9);
    RogueTrial_ApplyPendingSelection();

    EXPECT(gRogueSaveBlock->hasLastTrialSelection);
    EXPECT_EQ(gRogueSaveBlock->lastTrialId, ROGUE_TRIAL_TYPE_NORMAL);
    EXPECT_EQ(gRogueSaveBlock->lastTrialDifficulty, DIFFICULTY_LEVEL_AVERAGE);
    EXPECT_EQ(gRogueSaveBlock->lastTrialPokedexVariant, POKEDEX_VARIANT_NATIONAL_GEN9);

    gSpecialVar_0x8004 = ROGUE_TRIAL_NONE;
    gSpecialVar_0x8005 = DIFFICULTY_LEVEL_EASY;
    gSpecialVar_0x8006 = POKEDEX_VARIANT_NONE;
    RogueTrial_LoadLastSelection();

    EXPECT(gSpecialVar_Result);
    EXPECT_EQ(gSpecialVar_0x8004, ROGUE_TRIAL_TYPE_NORMAL);
    EXPECT_EQ(gSpecialVar_0x8005, DIFFICULTY_LEVEL_AVERAGE);
    EXPECT_EQ(gSpecialVar_0x8006, POKEDEX_VARIANT_NATIONAL_GEN9);

    ClearCaughtMonTestState();
}

TEST("The selected evil team unlocks only its Triumph quest")
{
    ResetCaughtMonTestState();

    RogueQuest_SetStateFlag(QUEST_ID_ROCKET_TRIUMPH, QUEST_STATE_UNLOCKED, FALSE);
    RogueQuest_SetStateFlag(QUEST_ID_AQUA_TRIUMPH, QUEST_STATE_UNLOCKED, FALSE);
    RogueQuest_SetStateFlag(QUEST_ID_MAGMA_TRIUMPH, QUEST_STATE_UNLOCKED, FALSE);

    gRogueRun.teamEncounterNum = TEAM_NUM_JOHTO_ROCKET;
    RogueQuest_UnlockCurrentEvilTeamQuest();

    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_ROCKET_TRIUMPH));
    EXPECT(!RogueQuest_IsQuestUnlocked(QUEST_ID_AQUA_TRIUMPH));
    EXPECT(!RogueQuest_IsQuestUnlocked(QUEST_ID_MAGMA_TRIUMPH));

    RogueQuest_UnlockAllEvilTeamQuests();
    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_ROCKET_TRIUMPH));
    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_AQUA_TRIUMPH));
    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_MAGMA_TRIUMPH));

#ifdef ROGUE_EXPANSION
    RogueQuest_SetStateFlag(QUEST_ID_PLASMA_TRIUMPH, QUEST_STATE_UNLOCKED, FALSE);
    gRogueRun.teamEncounterNum = TEAM_NUM_NEOPLASMA;
    RogueQuest_UnlockCurrentEvilTeamQuest();
    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_PLASMA_TRIUMPH));
#endif

    ClearCaughtMonTestState();
}

TEST("Optional accomplishments are Achievements instead of Main Quests")
{
    static const u16 sAchievementQuests[] =
    {
#ifdef ROGUE_EXPANSION
        QUEST_ID_MEGA_EVOLUTION,
        QUEST_ID_Z_MOVES,
        QUEST_ID_DYNAMAX,
        QUEST_ID_TERASTALLIZATION,
        QUEST_ID_GIMMICK_OVERFLOW,
#endif
        QUEST_ID_TRUE_TACTICS,
        QUEST_ID_DIVERSITY,
        QUEST_ID_AESTHETICS,
        QUEST_ID_I_AM_SPEED,
        QUEST_ID_ROCKET_TRIUMPH,
        QUEST_ID_AQUA_TRIUMPH,
        QUEST_ID_MAGMA_TRIUMPH,
#ifdef ROGUE_EXPANSION
        QUEST_ID_GALACTIC_TRIUMPH,
        QUEST_ID_PLASMA_TRIUMPH,
        QUEST_ID_FLARE_TRIUMPH,
#endif
    };
    u16 i;

    for(i = 0; i < ARRAY_COUNT(sAchievementQuests); ++i)
    {
        EXPECT(RogueQuest_GetConstFlag(sAchievementQuests[i], QUEST_CONST_IS_ACHIEVEMENT));
        EXPECT(!RogueQuest_GetConstFlag(sAchievementQuests[i], QUEST_CONST_IS_MAIN_QUEST));
    }
}

TEST("Pending Iron Mono limits the starting party to one Pokemon")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_IRON_MONO, POKEDEX_VARIANT_ROGUE_CLASSICPLUS);

    gSpecialVar_Result = PARTY_SIZE;
    RogueTrial_ApplyPendingPartyCapacity();
    EXPECT_EQ(gSpecialVar_Result, 1);

    ClearCaughtMonTestState();
}

TEST("Forced-starter Trials ignore incoming party capacity")
{
    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_IRON_KAIZO, POKEDEX_VARIANT_ROGUE_CLASSICPLUS);

    gSpecialVar_Result = PARTY_SIZE;
    RogueTrial_ApplyPendingPartyCapacity();
    EXPECT_EQ(gSpecialVar_Result, PARTY_SIZE);

    ClearCaughtMonTestState();
}

TEST("Apotheosis offers and accepts only Legendary Pokemon")
{
    u8 i;

    ResetCaughtMonTestState();
    SetPendingTrialSelection(ROGUE_TRIAL_APOTHEOSIS, POKEDEX_VARIANT_ROGUE_CLASSICPLUS);
    Rogue_RandomiseStarters();

    for (i = 0; i < 3; ++i)
        EXPECT(RoguePokedex_IsSpeciesLegendary(GetGeneratedStarter(i)));

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_APOTHEOSIS);
    EXPECT(RogueTrial_IsSpeciesLegal(SPECIES_MEWTWO, 0));
    EXPECT(!RogueTrial_IsSpeciesLegal(SPECIES_RATTATA, 0));
    EXPECT(RogueTrial_IsCatchGuaranteed());

    ClearCaughtMonTestState();
}

TEST("Limited Capture enforces its five-catch budget and bypass restrictions")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LIMITED_CAPTURE);

    EXPECT(RogueTrial_IsCatchGuaranteed());
    EXPECT(RogueTrial_IsDayCareDisabled());
    EXPECT(RogueTrial_IsRandomanDisabled());
    EXPECT(!RogueTrial_CanReceiveGift());

    VarSet(VAR_ROGUE_TOTAL_RUN_CATCHES, 4);
    EXPECT(RogueTrial_CanThrowBall());
    EXPECT(!RogueTrial_IsCompleteForQuest(ROGUE_TRIAL_LIMITED_CAPTURE));

    VarSet(VAR_ROGUE_TOTAL_RUN_CATCHES, 5);
    EXPECT(!RogueTrial_CanThrowBall());
    EXPECT(RogueTrial_IsCompleteForQuest(ROGUE_TRIAL_LIMITED_CAPTURE));

    ClearCaughtMonTestState();
}

TEST("Chaos Master rerolls the full party before Trainer battles")
{
    u16 originalSpecies;

    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_CHAOS_MASTER);
    SetPartyMon(0, SPECIES_MAGIKARP);
    originalSpecies = GetMonData(&gPlayerParty[0], MON_DATA_SPECIES);

    RogueTrial_OnTrainerBattleStart();

    EXPECT_EQ(CalculatePlayerPartyCount(), 1);
    EXPECT(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES) != SPECIES_NONE);
    EXPECT(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES) != originalSpecies);

    ClearCaughtMonTestState();
}

TEST("Little Cup Trial temporarily forces battle Pokemon to level 5")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LITTLE_CUP);
    SetPartyMon(0, SPECIES_MAGIKARP);
    SetEnemyMon(0, SPECIES_ZIGZAGOON);

    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_LEVEL), 50);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 50);

    RogueTrial_OnTrainerTeamReady();
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_LEVEL), 5);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 5);

    RogueTrial_OnTrainerBattleEnd();
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_LEVEL), 50);
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 50);

    ClearCaughtMonTestState();
}

TEST("Little Cup Trial disables Trainer battle experience")
{
    s32 expGain = 100;

    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LITTLE_CUP);
    SetPartyMon(0, SPECIES_MAGIKARP);
    gBattleTypeFlags = BATTLE_TYPE_TRAINER;

    Rogue_ModifyExpGained(&gPlayerParty[0], &expGain);
    EXPECT_EQ(expGain, 0);

    gBattleTypeFlags = 0;
    ClearCaughtMonTestState();
}

TEST("Little Cup Trial replaces illegal opponent species")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_LITTLE_CUP);
    SetPartyMon(0, SPECIES_MAGIKARP);
    SetEnemyMon(0, SPECIES_KLAWF);

    EXPECT(!RogueTrial_IsSpeciesLegal(SPECIES_KLAWF, 0));
    RogueTrial_OnTrainerTeamReady();
    EXPECT(RogueTrial_IsSpeciesLegal(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), 0));
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_LEVEL), 5);

    RogueTrial_OnTrainerBattleEnd();
    ClearCaughtMonTestState();
}

TEST("Handicap Trials do not apply player species legality to opponents")
{
    ResetCaughtMonTestState();
    ActivateCaughtMonTestTrial(ROGUE_TRIAL_TYPE_WATER);
    SetPartyMon(0, SPECIES_MAGIKARP);
    SetEnemyMon(0, SPECIES_ZIGZAGOON);

    RogueTrial_OnTrainerTeamReady();
    EXPECT_EQ(GetMonData(&gEnemyParty[0], MON_DATA_SPECIES), SPECIES_ZIGZAGOON);

    ClearCaughtMonTestState();
}

TEST("Equalized Trial applies 500 BST throughout the active run")
{
    u16 canonicalAttack;
    u16 normalizedAttack;

    ResetCaughtMonTestState();
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    SetPartyMon(0, SPECIES_MAGIKARP);
    canonicalAttack = GetMonData(&gPlayerParty[0], MON_DATA_ATK);
    EXPECT_NE(RoguePokedex_GetSpeciesBST(SPECIES_MAGIKARP), 500);

    ActivateCaughtMonTestTrial(ROGUE_TRIAL_EQUALIZED);
    CalculateMonStats(&gPlayerParty[0]);
    normalizedAttack = GetMonData(&gPlayerParty[0], MON_DATA_ATK);
    EXPECT_EQ(RoguePokedex_GetSpeciesBST(SPECIES_MAGIKARP), 500);
    EXPECT_GT(normalizedAttack, canonicalAttack);

    SetEnemyMon(0, SPECIES_MAGIKARP);
    RogueTrial_OnTrainerTeamReady();
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), SPECIES_MAGIKARP);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_ATK), normalizedAttack);

    RogueTrial_OnTrainerBattleEnd();
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    CalculateMonStats(&gPlayerParty[0]);
    EXPECT_EQ(RoguePokedex_GetSpeciesBST(SPECIES_MAGIKARP), 200);
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_ATK), canonicalAttack);

    ClearCaughtMonTestState();
}
