#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/rogue_pokedex.h"
#include "constants/script_menu.h"

#include "event_data.h"
#include "pokemon.h"
#include "script_menu.h"
#include "string_util.h"
#include "strings.h"

#include "rogue.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_multiplayer.h"
#include "rogue_pokedex.h"
#include "rogue_settings.h"
#include "rogue_trials.h"

struct RogueTrialPendingSelection
{
    u8 trialId;
    u8 difficulty;
    bool8 isPending;
};

static EWRAM_DATA struct RogueTrialPendingSelection sPendingTrial = {0};

static const u8 sText_SoloContract[] = _("Solo Contract");
static const u8 sText_HoennCircuit[] = _("Hoenn Circuit");
static const u8 sText_CursedCircuit[] = _("Cursed Circuit");

static const u8 sText_SoloContractDesc[] = _("Win without adding any Pokémon.");
static const u8 sText_HoennCircuitDesc[] = _("Win with the Hoenn Pokédex locked in.");
static const u8 sText_CursedCircuitDesc[] = _("Win under fixed curse pressure.");

static const u8 sText_Easy[] = _("Easy");
static const u8 sText_Average[] = _("Average");
static const u8 sText_Hard[] = _("Hard");
static const u8 sText_Brutal[] = _("Brutal");
static const u8 sText_Back[] = _("Back");

static const u8 *const sDifficultyNames[DIFFICULTY_PRESET_COUNT] =
{
    [DIFFICULTY_LEVEL_EASY] = sText_Easy,
    [DIFFICULTY_LEVEL_AVERAGE] = sText_Average,
    [DIFFICULTY_LEVEL_HARD] = sText_Hard,
    [DIFFICULTY_LEVEL_BRUTAL] = sText_Brutal,
};

static const struct RogueTrialDefinition sTrialDefinitions[ROGUE_TRIAL_COUNT] =
{
    [ROGUE_TRIAL_NONE] =
    {
        .id = ROGUE_TRIAL_NONE,
        .name = NULL,
        .description = NULL,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .blockCaughtMons = FALSE,
    },
    [ROGUE_TRIAL_SOLO_CONTRACT] =
    {
        .id = ROGUE_TRIAL_SOLO_CONTRACT,
        .name = sText_SoloContract,
        .description = sText_SoloContractDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .blockCaughtMons = TRUE,
    },
    [ROGUE_TRIAL_HOENN_CIRCUIT] =
    {
        .id = ROGUE_TRIAL_HOENN_CIRCUIT,
        .name = sText_HoennCircuit,
        .description = sText_HoennCircuitDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_HOENN_ORAS,
        .blockCaughtMons = FALSE,
    },
    [ROGUE_TRIAL_CURSED_CIRCUIT] =
    {
        .id = ROGUE_TRIAL_CURSED_CIRCUIT,
        .name = sText_CursedCircuit,
        .description = sText_CursedCircuitDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .blockCaughtMons = FALSE,
    },
};

static bool8 IsValidTrialId(u8 trialId)
{
    return trialId > ROGUE_TRIAL_NONE && trialId < ROGUE_TRIAL_COUNT;
}

static bool8 IsValidDifficulty(u8 difficulty)
{
    return difficulty < DIFFICULTY_PRESET_COUNT;
}

const struct RogueTrialDefinition *RogueTrial_GetDefinition(u8 trialId)
{
    if (trialId < ROGUE_TRIAL_COUNT)
        return &sTrialDefinitions[trialId];

    return NULL;
}

bool8 RogueTrial_IsActive(void)
{
    return Rogue_IsRunActive() && IsValidTrialId(gRogueRun.trialState.trialId);
}

bool8 RogueTrial_IsActiveTrial(u8 trialId)
{
    return RogueTrial_IsActive() && gRogueRun.trialState.trialId == trialId;
}

bool8 RogueTrial_IsInvalidated(void)
{
    return RogueTrial_IsActive() && gRogueRun.trialState.invalidated;
}

void RogueTrial_Invalidate(void)
{
    if (RogueTrial_IsActive())
        gRogueRun.trialState.invalidated = TRUE;
}

void RogueTrial_ApplyPendingSelection(void)
{
    const struct RogueTrialDefinition *trial;

    if (!sPendingTrial.isPending)
        return;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);

    if (trial != NULL && IsValidTrialId(sPendingTrial.trialId) && IsValidDifficulty(sPendingTrial.difficulty))
    {
        gRogueRun.trialState.trialId = sPendingTrial.trialId;
        gRogueRun.trialState.difficulty = sPendingTrial.difficulty;
        gRogueRun.trialState.initialPartyCount = CalculatePlayerPartyCount();
        gRogueRun.trialState.invalidated = FALSE;
        gRogueRun.trialState.initialPartyCountSet = TRUE;

        Rogue_SetDifficultyPreset(sPendingTrial.difficulty);

        if (trial->forcedPokedexVariant != POKEDEX_VARIANT_NONE)
            RoguePokedex_SetDexVariant(trial->forcedPokedexVariant);

        FlagClear(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    }

    RogueTrial_ClearPendingSelection();
}

u8 RogueTrial_GetPendingForcedPokedexVariant(void)
{
    const struct RogueTrialDefinition *trial;

    if (!sPendingTrial.isPending || !IsValidTrialId(sPendingTrial.trialId) || !IsValidDifficulty(sPendingTrial.difficulty))
        return POKEDEX_VARIANT_NONE;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    if (trial == NULL)
        return POKEDEX_VARIANT_NONE;

    return trial->forcedPokedexVariant;
}

u16 RogueTrial_GetCharmItemCount(u8 effectType UNUSED)
{
    return 0;
}

u16 RogueTrial_GetCurseItemCount(u8 effectType)
{
    if (!RogueTrial_IsActiveTrial(ROGUE_TRIAL_CURSED_CIRCUIT))
        return 0;

    switch (effectType)
    {
        case EFFECT_SHOP_PRICE:
        case EFFECT_CATCH_RATE:
        case EFFECT_WILD_ENCOUNTER_COUNT:
            return 1;
    }

    return 0;
}

bool8 RogueTrial_CanAcceptCaughtMon(void)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_IsActive())
        return TRUE;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    if (trial != NULL && trial->blockCaughtMons)
        return FALSE;

    return TRUE;
}

bool8 RogueTrial_IsCompleteForQuest(u8 trialId)
{
    if (!RogueTrial_IsActiveTrial(trialId) || RogueTrial_IsInvalidated())
        return FALSE;

    if (trialId == ROGUE_TRIAL_SOLO_CONTRACT)
    {
        if (VarGet(VAR_ROGUE_TOTAL_RUN_CATCHES) != 0)
            return FALSE;

        if (gRogueRun.trialState.initialPartyCountSet
         && CalculatePlayerPartyCount() > gRogueRun.trialState.initialPartyCount)
            return FALSE;
    }

    return TRUE;
}

void RogueTrial_OnMonGiven(void)
{
    if (RogueTrial_IsActiveTrial(ROGUE_TRIAL_SOLO_CONTRACT) && gRogueRun.trialState.initialPartyCountSet)
        RogueTrial_Invalidate();
}

void RogueTrial_AppendTrialOptions(void)
{
    u8 i;

    for (i = ROGUE_TRIAL_NONE + 1; i < ROGUE_TRIAL_COUNT; ++i)
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sTrialDefinitions[i].name, i);

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gText_Exit, MULTI_B_PRESSED);
}

void RogueTrial_AppendDifficultyOptions(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sDifficultyNames); ++i)
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sDifficultyNames[i], i);

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
}

void RogueTrial_BufferTrialPreview(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL && IsValidTrialId(gSpecialVar_0x8004))
    {
        StringCopy(gStringVar1, trial->name);
        StringCopy(gStringVar2, trial->description);
    }
    else
    {
        StringCopy(gStringVar1, gText_Exit);
        StringCopy(gStringVar2, gText_Exit);
    }

    if (IsValidDifficulty(gSpecialVar_0x8005))
        StringCopy(gStringVar3, sDifficultyNames[gSpecialVar_0x8005]);
    else
        StringCopy(gStringVar3, sText_Back);
}

void RogueTrial_SetPendingSelectionFromScript(void)
{
    if (IsValidTrialId(gSpecialVar_0x8004) && IsValidDifficulty(gSpecialVar_0x8005))
    {
        sPendingTrial.trialId = gSpecialVar_0x8004;
        sPendingTrial.difficulty = gSpecialVar_0x8005;
        sPendingTrial.isPending = TRUE;
        gSpecialVar_Result = TRUE;
    }
    else
    {
        RogueTrial_ClearPendingSelection();
        gSpecialVar_Result = FALSE;
    }
}

void RogueTrial_ClearPendingSelection(void)
{
    memset(&sPendingTrial, 0, sizeof(sPendingTrial));
}

void RogueTrial_CanUseTerminal(void)
{
    if (RogueMP_IsActive() || FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE))
        gSpecialVar_Result = FALSE;
    else
        gSpecialVar_Result = TRUE;
}

void RogueTrial_RecordInitialParty(void)
{
    if (RogueTrial_IsActive())
    {
        gRogueRun.trialState.initialPartyCount = CalculatePlayerPartyCount();
        gRogueRun.trialState.initialPartyCountSet = TRUE;
    }
}
