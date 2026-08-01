#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/rogue_hub.h"

#include "event_data.h"
#include "pokemon.h"
#include "script.h"
#include "script_menu.h"

#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_hub.h"
#include "rogue_multiplayer.h"
#include "rogue_pokedex.h"
#include "rogue_quest.h"
#include "rogue_run_start.h"
#include "rogue_save.h"
#include "rogue_script.h"
#include "rogue_settings.h"
#include "rogue_trials.h"

static EWRAM_DATA struct RogueRunStartContext sRunStartContext;

static const u8 sText_AdventureSettings[] = _("Adventure Settings");
static const u8 sText_Pokedex[] = _("Pokédex");
static const u8 sText_Trial[] = _("Trial");
static const u8 sText_Difficulty[] = _("Difficulty");
static const u8 sText_Back[] = _("Back");

static bool8 HasPendingQuestRewards(void)
{
    u16 questId;

    for (questId = 0; questId < QUEST_ID_COUNT; ++questId)
    {
        if (RogueQuest_HasPendingRewards(questId))
            return TRUE;
    }

    return FALSE;
}

static bool8 IsMultiplayerClientJoinable(void)
{
    if (sRunStartContext.source != RUN_START_SOURCE_MULTIPLAYER_CLIENT)
        return TRUE;

    if (!RogueMP_IsClient() || gRogueMultiplayer == NULL)
        return FALSE;

    return gRogueMultiplayer->gameState.adventure.isRunActive;
}

static u8 GetPartyClauseReason(void)
{
    bool8 hasLegendaryOrMythical = FALSE;
    u8 i, j;

    for (i = 0; i < gPlayerPartyCount; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if (species == SPECIES_NONE || species == SPECIES_EGG)
            continue;

        if (Rogue_GetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE))
        {
            for (j = i + 1; j < gPlayerPartyCount; ++j)
            {
                u16 otherSpecies = GetMonData(&gPlayerParty[j], MON_DATA_SPECIES);

                if (otherSpecies != SPECIES_NONE
                 && otherSpecies != SPECIES_EGG
                 && Rogue_SpeciesShareEvolutionLine(species, otherSpecies))
                    return RUN_START_REASON_SPECIES_CLAUSE;
            }
        }

        if (Rogue_GetConfigToggle(CONFIG_TOGGLE_LEGENDARY_CLAUSE)
         && RoguePokedex_IsSpeciesLegendary(species))
        {
            if (hasLegendaryOrMythical)
                return RUN_START_REASON_LEGENDARY_CLAUSE;
            hasLegendaryOrMythical = TRUE;
        }
    }

    return RUN_START_REASON_NONE;
}

static void CaptureEligibilityFailure(bool8 isDayCare)
{
    u8 i;
    u8 count = isDayCare ? Rogue_GetCurrentDaycareSlotCount() : PARTY_SIZE;

    for (i = 0; i < count; ++i)
    {
        u16 species = isDayCare
            ? GetBoxMonData(Rogue_GetDaycareBoxMon(i), MON_DATA_SPECIES)
            : GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        u16 param;
        u8 reason = RogueTrial_GetPendingSpeciesEligibilityReason(species, isDayCare, &param);

        if (reason != ROGUE_TRIAL_ELIGIBILITY_OK)
        {
            sRunStartContext.eligibilityReason = reason;
            sRunStartContext.ineligibleSpecies = species;
            sRunStartContext.eligibilityParam = param;
            return;
        }
    }
}

static void ResolveStandardConfig(void)
{
    if (RogueMP_IsClient())
    {
        AGB_ASSERT(gRogueMultiplayer != NULL);
        memcpy(&sRunStartContext.effectiveConfig,
               &gRogueMultiplayer->gameState.hub.difficultyConfig,
               sizeof(sRunStartContext.effectiveConfig));
        sRunStartContext.source = RUN_START_SOURCE_MULTIPLAYER_CLIENT;
        sRunStartContext.canEdit = FALSE;
    }
    else if (FlagGet(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE)
          && gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED].isValid)
    {
        memcpy(&sRunStartContext.effectiveConfig,
               &gRogueSaveBlock->adventureReplay[ROGUE_ADVENTURE_REPLAY_REMEMBERED].difficultyConfig,
               sizeof(sRunStartContext.effectiveConfig));
        sRunStartContext.source = RUN_START_SOURCE_REPLAY;
        sRunStartContext.canEdit = FALSE;
    }
    else
    {
        Rogue_CopyReadableDifficultyConfig(&sRunStartContext.effectiveConfig);
        sRunStartContext.source = RogueMP_IsHost()
            ? RUN_START_SOURCE_MULTIPLAYER_HOST
            : RUN_START_SOURCE_NORMAL;
        sRunStartContext.canEdit = Rogue_CanEditConfig();
    }
}

void RogueRunStart_PrepareStandard(void)
{
    RogueRunStart_Clear();
    ResolveStandardConfig();

    sRunStartContext.isActive = TRUE;
    sRunStartContext.configRevision = 1;
    sRunStartContext.trialId = ROGUE_TRIAL_NONE;
    sRunStartContext.trialDifficulty = DIFFICULTY_LEVEL_CUSTOM;
    sRunStartContext.pokedexVariant = sRunStartContext.effectiveConfig.rangeValues[CONFIG_RANGE_POKEDEX_VARIANT];
    Rogue_SetRunStartConfigOverride(&sRunStartContext.effectiveConfig);
    RogueRunStart_Refresh();
}

void RogueRunStart_PrepareTrial(void)
{
    struct RogueDifficultyConfig config;

    RogueRunStart_Clear();
    Rogue_CopyReadableDifficultyConfig(&config);

    sRunStartContext.trialId = gSpecialVar_0x8004;
    sRunStartContext.trialDifficulty = gSpecialVar_0x8005;
    sRunStartContext.pokedexVariant = gSpecialVar_0x8006;

    if (!RogueTrial_BuildSelectionConfig(
            sRunStartContext.trialId,
            sRunStartContext.trialDifficulty,
            sRunStartContext.pokedexVariant,
            &config))
        return;

    memcpy(&sRunStartContext.effectiveConfig, &config, sizeof(config));
    sRunStartContext.source = RUN_START_SOURCE_TRIAL;
    sRunStartContext.canEdit = TRUE;
    sRunStartContext.isActive = TRUE;
    sRunStartContext.configRevision = 1;
    Rogue_SetRunStartConfigOverride(&sRunStartContext.effectiveConfig);
    RogueTrial_SetPreviewSelectionFromScript();
    RogueRunStart_Refresh();
}

void RogueRunStart_Refresh(void)
{
    bool8 partyInvalid = FALSE;
    bool8 canUseRandomPartner;
    bool8 replacesParty;
    bool8 fixedParty;
    u8 clauseReason;
    u8 partyCount;

    if (!sRunStartContext.isActive)
        return;

    // Multiplayer settings may change while a client waits at the review.
    if (sRunStartContext.source == RUN_START_SOURCE_MULTIPLAYER_CLIENT
     && RogueMP_IsClient()
     && gRogueMultiplayer != NULL)
    {
        if (memcmp(&sRunStartContext.effectiveConfig,
                   &gRogueMultiplayer->gameState.hub.difficultyConfig,
                   sizeof(sRunStartContext.effectiveConfig)) != 0)
        {
            memcpy(&sRunStartContext.effectiveConfig,
                   &gRogueMultiplayer->gameState.hub.difficultyConfig,
                   sizeof(sRunStartContext.effectiveConfig));
            sRunStartContext.pokedexVariant = sRunStartContext.effectiveConfig.rangeValues[CONFIG_RANGE_POKEDEX_VARIANT];
            Rogue_SetRunStartConfigOverride(&sRunStartContext.effectiveConfig);
            ++sRunStartContext.configRevision;
        }
    }

    sRunStartContext.hasPendingQuestRewards = HasPendingQuestRewards();
    sRunStartContext.mainQuestsDisabled = sRunStartContext.source == RUN_START_SOURCE_REPLAY
        || Rogue_GetModeRules()->disableMainQuests
        || AnyCharmsActive();
    sRunStartContext.trialQuestsDisabled = sRunStartContext.source == RUN_START_SOURCE_REPLAY
        || Rogue_GetModeRules()->disableTrialQuests
        || AnyCharmsActive();
    sRunStartContext.requiresRandomPartner = FALSE;
    sRunStartContext.readinessReason = RUN_START_REASON_NONE;
    sRunStartContext.eligibilityReason = ROGUE_TRIAL_ELIGIBILITY_OK;
    sRunStartContext.ineligibleSpecies = SPECIES_NONE;
    sRunStartContext.eligibilityParam = 0;
    sRunStartContext.partyCapacity = RogueTrial_GetPendingPartyCapacity(Rogue_GetStartingMonCapacity());
    sRunStartContext.teamPolicy = sRunStartContext.source == RUN_START_SOURCE_TRIAL
        || RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER)
        ? RUN_START_TEAM_OPTIONAL_PARTNER
        : RUN_START_TEAM_CURRENT;

    if (!IsMultiplayerClientJoinable())
    {
        sRunStartContext.readiness = RUN_START_WAITING_FOR_HOST;
        return;
    }

    RogueTrial_CanUsePendingDayCare();
    if (gSpecialVar_Result == FALSE)
    {
        CaptureEligibilityFailure(TRUE);
        sRunStartContext.readinessReason = RUN_START_REASON_DAY_CARE_ILLEGAL;
        sRunStartContext.readiness = RUN_START_BLOCKED_DAY_CARE;
        return;
    }

    fixedParty = RogueTrial_PendingHasFixedStartingPartyValue();
    replacesParty = RogueTrial_PendingReplacesStartingPartyValue();
    if (fixedParty)
    {
        const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(sRunStartContext.trialId);

        if (trial != NULL)
            sRunStartContext.partyCapacity = trial->fixedStartingPartyCount;
        sRunStartContext.teamPolicy = RUN_START_TEAM_FIXED_TRIAL;
        sRunStartContext.readiness = RUN_START_READY;
        return;
    }
    if (replacesParty)
    {
        sRunStartContext.requiresRandomPartner = TRUE;
        sRunStartContext.teamPolicy = RUN_START_TEAM_REQUIRED_PARTNER;
        sRunStartContext.readinessReason = RUN_START_REASON_TRIAL_REPLACES_PARTY;
        sRunStartContext.readiness = RUN_START_READY_REQUIRES_PARTNER;
        return;
    }

    partyCount = CalculatePlayerPartyCount();
    if (partyCount == 0)
    {
        partyInvalid = TRUE;
        sRunStartContext.readinessReason = RUN_START_REASON_PARTY_EMPTY;
    }
    else if (partyCount > sRunStartContext.partyCapacity)
    {
        partyInvalid = TRUE;
        sRunStartContext.readinessReason = RUN_START_REASON_PARTY_CAPACITY;
    }

    RogueTrial_CanUsePendingParty();
    if (gSpecialVar_Result == FALSE)
    {
        partyInvalid = TRUE;
        CaptureEligibilityFailure(FALSE);
        if (sRunStartContext.readinessReason == RUN_START_REASON_NONE)
            sRunStartContext.readinessReason = RUN_START_REASON_PARTY_ILLEGAL;
    }

    clauseReason = GetPartyClauseReason();
    if (sRunStartContext.readinessReason == RUN_START_REASON_NONE)
        sRunStartContext.readinessReason = clauseReason;

    canUseRandomPartner = sRunStartContext.source == RUN_START_SOURCE_TRIAL
        || RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);

    if (partyInvalid || clauseReason != RUN_START_REASON_NONE)
    {
        if (canUseRandomPartner)
        {
            sRunStartContext.requiresRandomPartner = TRUE;
            sRunStartContext.teamPolicy = RUN_START_TEAM_REQUIRED_PARTNER;
            sRunStartContext.readiness = RUN_START_READY_REQUIRES_PARTNER;
        }
        else
        {
            sRunStartContext.readiness = RUN_START_BLOCKED_PARTY;
        }
        return;
    }

    sRunStartContext.readiness = RUN_START_READY;
}

void RogueRunStart_Clear(void)
{
    Rogue_ClearRunStartConfigOverride();
    RogueTrial_ClearPendingSelection();
    memset(&sRunStartContext, 0, sizeof(sRunStartContext));
}

const struct RogueRunStartContext *RogueRunStart_GetContext(void)
{
    return sRunStartContext.isActive ? &sRunStartContext : NULL;
}

bool8 RogueRunStart_CanStart(void)
{
    return sRunStartContext.isActive
        && (sRunStartContext.readiness == RUN_START_READY
         || sRunStartContext.readiness == RUN_START_READY_REQUIRES_PARTNER);
}

void RogueRunStart_CheckCanStart(void)
{
    RogueRunStart_Refresh();
    gSpecialVar_Result = RogueRunStart_CanStart();
}

void RogueRunStart_Commit(void)
{
    RogueRunStart_CheckCanStart();
    if (!gSpecialVar_Result)
        return;

    if (sRunStartContext.source == RUN_START_SOURCE_TRIAL)
        RogueTrial_SetPendingSelectionFromScript();
}

void RogueRunStart_RequiresRandomPartner(void)
{
    gSpecialVar_Result = sRunStartContext.isActive && sRunStartContext.requiresRandomPartner;
}

void RogueRunStart_AppendStandardEditOptions(void)
{
    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_AdventureSettings, RUN_START_EDIT_SETTINGS);
    if (sRunStartContext.canEdit && RoguePokedex_IsVariantEditEnabled())
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Pokedex, RUN_START_EDIT_POKEDEX);
    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
}

void RogueRunStart_AppendTrialEditOptions(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(sRunStartContext.trialId);
    u16 pokedexOptionCount;

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Trial, RUN_START_EDIT_TRIAL);

    if (trial != NULL && !trial->hasForcedDifficulty)
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Difficulty, RUN_START_EDIT_DIFFICULTY);

    RogueTrial_GetPokedexOptionCount();
    pokedexOptionCount = gSpecialVar_Result;
    if (pokedexOptionCount > 1)
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Pokedex, RUN_START_EDIT_TRIAL_POKEDEX);

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
}
