#include "global.h"
static const u8 sAscensionText0[] = _("Records only: alternate mode");
static const u8 sAscensionText1[] = _("Trial records only");
static const u8 sAscensionText2[] = _("Records only: replay");
static const u8 sAscensionText3[] = _("Records only: multiplayer");
static const u8 sAscensionText4[] = _("No advancement: imported charms");
static const u8 sAscensionText5[] = _("No advancement: debug rules");
static const u8 sAscensionText6[] = _("Ascension advancement enabled");
#include "event_data.h"
#include "constants/flags.h"
#include "constants/vars.h"
#include "string_util.h"
#include "rogue.h"
#include "rogue_ascension.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_multiplayer.h"
#include "rogue_run_start.h"
#include "rogue_save.h"
#include "rogue_settings.h"

u8 RogueAscension_CalculateIV(u8 level, u8 stage, bool8 boss)
{
    if (!boss) return 0;
    if (level >= ASCENSION_PERFECT_BOSSES) return 31;
    if (stage >= ROGUE_FINAL_CHAMP_DIFFICULTY) return 15;
    if (stage >= ROGUE_CHAMP_START_DIFFICULTY) return 10;
    if (stage >= ROGUE_ELITE_START_DIFFICULTY) return 5;
    return 0;
}

u8 RogueAscension_PartySize(u8 level, u8 baseline, u8 capacity, bool8 boss)
{
    if (boss && level >= ASCENSION_FULL_TEAMS) baseline = PARTY_SIZE;
    else if (boss && level >= ASCENSION_EXTRA_MON) ++baseline;
    return min(PARTY_SIZE, min(baseline, capacity));
}

bool8 RogueAscension_IsRevealed(void)
{
    return FlagGet(FLAG_IS_CHAMPION) && FlagGet(FLAG_ROGUE_ASCENSION_INTRODUCED);
}

u8 RogueAscension_GetUnlocked(u8 format)
{
    u8 best;
    if (format > BATTLE_FORMAT_MIXED) return 0;
    best = gRogueSaveBlock->bestAscension[format];
    return best == ASCENSION_NONE ? 0 : min(best + 1, ASCENSION_MAX);
}

bool8 RogueAscension_IsUnlocked(u8 level, u8 format)
{
    return format <= BATTLE_FORMAT_MIXED && level <= RogueAscension_GetUnlocked(format);
}

void RogueAscension_ResetProgress(void)
{
    memset(gRogueSaveBlock->bestAscension, ASCENSION_NONE, sizeof(gRogueSaveBlock->bestAscension));
    memset(gRogueSaveBlock->bestModeAscension, ASCENSION_NONE, sizeof(gRogueSaveBlock->bestModeAscension));
    memset(gRogueSaveBlock->bestTrialAscension, ASCENSION_NONE, sizeof(gRogueSaveBlock->bestTrialAscension));
    memset(gRogueSaveBlock->selectedAscension, 0, sizeof(gRogueSaveBlock->selectedAscension));
    gRogueSaveBlock->ascensionEligible = FALSE;
    gRogueSaveBlock->ascensionRecorded = FALSE;
}

u8 RogueAscension_GetEligibility(const struct RogueAdventureConfig *config, u8 source)
{
    if (source == RUN_START_SOURCE_TRIAL) return ASCENSION_INELIGIBLE_TRIAL;
    if (source == RUN_START_SOURCE_REPLAY) return ASCENSION_INELIGIBLE_REPLAY;
    if (source == RUN_START_SOURCE_MULTIPLAYER_HOST || source == RUN_START_SOURCE_MULTIPLAYER_CLIENT)
        return ASCENSION_INELIGIBLE_MULTIPLAYER;
    if (VarGet(VAR_ROGUE_DESIRED_CAMPAIGN) != ROGUE_CAMPAIGN_NONE) return ASCENSION_INELIGIBLE_SPECIAL;
    if (config->mode != ROGUE_GAME_MODE_STANDARD && config->mode != ROGUE_GAME_MODE_RAINBOW && config->mode != ROGUE_GAME_MODE_OFFICIAL)
        return ASCENSION_INELIGIBLE_MODE;
    if (config->ascension < ASCENSION_FRESH_START && !config->trialFreshStart && AnyCharmsInBag())
        return ASCENSION_INELIGIBLE_CHARMS;
#ifdef ROGUE_DEBUG
    if (RogueDebug_GetConfigToggle(DEBUG_TOGGLE_STEAL_TEAM)
     || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_TRAINER_LVL_5)
     || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_ALLOW_SAVE_SCUM)
     || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_INSTANT_CAPTURE)
     || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_SHOPS)
     || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_STOP_WILD_SPAWNING)
     || RogueDebug_GetConfigToggle(DEBUG_TOGGLE_DEBUG_LEGENDS)
     || RogueDebug_GetConfigRange(DEBUG_RANGE_START_DIFFICULTY) != 0
     || RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_ROUTE) != 0
     || RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_EVIL_TEAM) != 0
     || RogueDebug_GetConfigRange(DEBUG_RANGE_FORCED_ITEM_ROOM) != 0)
        return ASCENSION_INELIGIBLE_DEBUG;
#endif
    return ASCENSION_ELIGIBLE;
}

const u8 *RogueAscension_EligibilityText(u8 reason)
{
    static const u8 sSpecial[] = _("Records only: special campaign");
    switch (reason)
    {
    case ASCENSION_INELIGIBLE_SPECIAL: return sSpecial;
    case ASCENSION_INELIGIBLE_MODE: return sAscensionText0;
    case ASCENSION_INELIGIBLE_TRIAL: return sAscensionText1;
    case ASCENSION_INELIGIBLE_REPLAY: return sAscensionText2;
    case ASCENSION_INELIGIBLE_MULTIPLAYER: return sAscensionText3;
    case ASCENSION_INELIGIBLE_CHARMS: return sAscensionText4;
    case ASCENSION_INELIGIBLE_DEBUG: return sAscensionText5;
    default: return sAscensionText6;
    }
}

static void UpdateBest(u8 *best, u8 level)
{
    if (*best == ASCENSION_NONE || level > *best) *best = level;
}

void RogueAscension_RecordWin(void)
{
    const struct RogueAdventureConfig *config = &gRogueSaveBlock->activeAdventureConfig;
    u8 trial = gRogueRun.trialState.trialId;
    if (gRogueSaveBlock->ascensionRecorded || config->battleFormat > BATTLE_FORMAT_MIXED || config->ascension > ASCENSION_MAX)
        return;
    gRogueSaveBlock->ascensionRecorded = TRUE;
    // Trial bests are awarded by their quest success, after all final conditions.
    if (trial == ROGUE_TRIAL_NONE && config->mode < ROGUE_GAME_MODE_COUNT)
        UpdateBest(&gRogueSaveBlock->bestModeAscension[config->mode][config->battleFormat], config->ascension);
    if (gRogueSaveBlock->ascensionEligible)
        UpdateBest(&gRogueSaveBlock->bestAscension[config->battleFormat], config->ascension);
}

static const u8 *const sAdditions[] =
{
    COMPOUND_STRING("Standard Adventure rules."),
    COMPOUND_STRING("Each Gym Leader’s ace holds a Sitrus Berry."),
    COMPOUND_STRING("Your rival raises a Unique Pokémon\nfrom your first battle to your last."),
    COMPOUND_STRING("Type specialists train a wider variety\nof Pokémon, including other types."),
    COMPOUND_STRING("Boss aces get competitive sets: matching\nmoves, Ability, nature and held item."),
    COMPOUND_STRING("Bosses bring one extra Pokémon, up to six."),
    COMPOUND_STRING("Gym Leaders use their preferred\nweather or terrain."),
    COMPOUND_STRING("Every boss Pokémon gets a competitive set."),
    COMPOUND_STRING("Each Elite Four member brings\na Legendary Pokémon."),
    COMPOUND_STRING("Boss Pokémon have perfect IVs."),
    COMPOUND_STRING("Fresh start: keep your team and held items.\nBegin with a standard kit and ¥10,000."),
    COMPOUND_STRING("Each Gym Leader, Elite Four member and\nChampion brings a Unique Pokémon."),
    COMPOUND_STRING("Bosses bring six Pokémon when space allows."),
};
STATIC_ASSERT(ARRAY_COUNT(sAdditions) == ASCENSION_MAX + 1, AscensionLevelCount);

const u8 *RogueAscension_GetAddition(u8 level)
{
    return sAdditions[min(level, ASCENSION_MAX)];
}

// The rules screen has two pages with six single-line entries each.
struct RuleSummary
{
    u8 level;
    const u8 *text;
};

static const struct RuleSummary sRuleSummary[][4] =
{
    {{0, COMPOUND_STRING("Boss IVs: 0 / 5 / 10 / 15 by stage")},
     {ASCENSION_PERFECT_BOSSES, COMPOUND_STRING("Boss IVs: 31")}},
    {{0, COMPOUND_STRING("Standard rival partner")},
     {ASCENSION_RIVAL_UNIQUE, COMPOUND_STRING("Rival raises a Unique Pokémon")}},
    {{0, COMPOUND_STRING("Standard moves and held items")},
     {ASCENSION_GYM_BERRY, COMPOUND_STRING("Gym Leader aces hold Sitrus Berries")},
     {ASCENSION_ACE_SETS, COMPOUND_STRING("Competitive sets: boss aces")},
     {ASCENSION_BOSS_SETS, COMPOUND_STRING("Competitive sets: all boss Pokémon")}},
    {{0, COMPOUND_STRING("Sets: moves, Ability, nature and item")}},
    {{0, COMPOUND_STRING("Boss fields start at the Elite Four")},
     {ASCENSION_GYM_FIELDS, COMPOUND_STRING("Gyms: preferred weather or terrain")}},
    {{0, COMPOUND_STRING("Standard Elite Four teams")},
     {ASCENSION_ELITE_LEGEND, COMPOUND_STRING("Elite Four: one Legendary Pokémon")}},
    {{0, COMPOUND_STRING("Standard boss party sizes")},
     {ASCENSION_EXTRA_MON, COMPOUND_STRING("Boss parties: one extra, up to six")},
     {ASCENSION_FULL_TEAMS, COMPOUND_STRING("Boss parties: six Pokémon")}},
    {{0, COMPOUND_STRING("Standard trainer specialties")},
     {ASCENSION_DIVERSE_TRAINERS, COMPOUND_STRING("Broader trainer specialties")}},
    {{0, COMPOUND_STRING("Supplies: hub preparation")},
     {ASCENSION_FRESH_START, COMPOUND_STRING("Fresh start: kit and ¥10,000")}},
    {{0, COMPOUND_STRING("Standard League Pokémon")},
     {ASCENSION_BOSS_UNIQUE, COMPOUND_STRING("League bosses bring a Unique Pokémon")}},
    {{0, COMPOUND_STRING("Other trainer IVs: 0")}},
    {{0, COMPOUND_STRING("Enemy EVs: none")}},
};

u8 RogueAscension_GetRuleLine(u8 level, u8 line, u8 *dest)
{
    if (line >= ARRAY_COUNT(sRuleSummary)) return FALSE;
    const struct RuleSummary *row = sRuleSummary[line];
    u8 i = 0;
    while (i + 1 < ARRAY_COUNT(sRuleSummary[0]) && row[i + 1].text && row[i + 1].level <= level)
        ++i;
    StringCopy(dest, row[i].text);
    return TRUE;
}

u8 RogueAscension_GetConfigRuleLine(const struct RogueAdventureConfig *config, u8 line, u8 *dest)
{
    static const u8 sTrialBag[] = _("Supplies: Trial fresh start");
    static const u8 sTrialKit[] = _("Trial kit: 5 Balls, 1 Potion");
    static const u8 sTrialMoney[] = _("Trial starting fund: 0");
    static const u8 sGauntlet[] = _("Gauntlet: level 100, full parties");
    static const u8 sKit[] = _("15 Balls, 10 Potions, 3 Full Heals");
    static const u8 sKeep[] = _("Keep teams, key items and Day Care");
    if (!RogueAscension_GetRuleLine(config->ascension, line, dest)) return FALSE;
    if (line == 6 && (config->mode == ROGUE_GAME_MODE_GAUNTLET || config->mode == ROGUE_GAME_MODE_RAINBOW_GAUNTLET)) StringCopy(dest, sGauntlet);
    if (config->trialFreshStart)
    {
        if (line == 8) StringCopy(dest, sTrialBag);
        if (line == 10) StringCopy(dest, sTrialKit);
        if (line == 11) StringCopy(dest, sTrialMoney);
    }
    else if (config->ascension >= ASCENSION_FRESH_START)
    {
        if (line == 10) StringCopy(dest, sKit);
        if (line == 11) StringCopy(dest, sKeep);
    }
    return TRUE;
}

static const u8 *const sBaseLines[] = {
    COMPOUND_STRING("Fainted Pokémon leave the party."),
    COMPOUND_STRING("Set mode; enforced level caps."),
    COMPOUND_STRING("Battle bag: Poké Balls only."),
    COMPOUND_STRING("Player EVs allowed; enemy EVs 0."),
    COMPOUND_STRING("Species, item and legend clauses."),
    COMPOUND_STRING("EXP All is enabled."),
    COMPOUND_STRING("Boss aces are the last party Pokémon."),
    COMPOUND_STRING("Competitive sets unlock at A4 and A7."),
};

u8 RogueAscension_GetBaseLine(u8 line, u8 *dest)
{
    if (line >= ARRAY_COUNT(sBaseLines) || (line >= 6 && !RogueAscension_IsRevealed())) return FALSE;
    StringCopy(dest, sBaseLines[line]);
    return TRUE;
}
