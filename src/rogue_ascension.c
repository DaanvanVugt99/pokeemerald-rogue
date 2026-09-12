#include "global.h"
static const u8 sAscensionText0[] = _("Records only: alternate mode");
static const u8 sAscensionText1[] = _("Trial records only");
static const u8 sAscensionText2[] = _("Records only: replay");
static const u8 sAscensionText3[] = _("Records only: multiplayer");
static const u8 sAscensionText4[] = _("No advancement: imported charms");
static const u8 sAscensionText5[] = _("No advancement: debug rules");
static const u8 sAscensionText6[] = _("Ascension advancement enabled");
static const u8 sAscensionText7[] = _("Base adventure rules.");
static const u8 sAscensionText8[] = _("Boss IVs: base +5.");
static const u8 sAscensionText9[] = _("Profile natures: boss aces.");
static const u8 sAscensionText10[] = _("Competitive moves and matching\nregular Ability: boss aces.");
static const u8 sAscensionText11[] = _("Profile held items: boss aces.");
static const u8 sAscensionText12[] = _("Boss IVs: base +10.");
static const u8 sAscensionText13[] = _("Profile natures: all bosses.");
static const u8 sAscensionText14[] = _("Competitive moves and matching\nregular Ability: all bosses.");
static const u8 sAscensionText15[] = _("Profile held items: all bosses.");
static const u8 sAscensionText16[] = _("Other trainer IVs: 10.");
static const u8 sAscensionText17[] = _("Boss parties: +1, maximum 6.");
static const u8 sAscensionText18[] = _("Profile natures: all trainers.");
static const u8 sAscensionText19[] = _("Competitive moves and matching\nregular Ability: all trainers.");
static const u8 sAscensionText20[] = _("Boss IVs: base +20, maximum 31.");
static const u8 sAscensionText21[] = _("Profile held items: all trainers.");
static const u8 sAscensionText22[] = _("Broader themed trainer specialties.");
static const u8 sAscensionText23[] = _("All trainer IVs: at least 20.");
static const u8 sAscensionText24[] = _("Boss ace IVs: 31.");
static const u8 sAscensionText25[] = _("All trainer IVs: 31. No enemy EVs.");
static const u8 sAscensionText26[] = _("Fixed supplies and ¥10,000.\nKeep prepared teams and held items.");
static const u8 sAscensionText27[] = _("All boss parties: 6 Pokemon.");
static const u8 sAscensionText28[] = _("Aces only");
static const u8 sAscensionText29[] = _("Boss Pokemon");
static const u8 sAscensionText30[] = _("Trainer Pokemon");
static const u8 sAscensionText31[] = _("None");
static const u8 sAscensionText32[] = _("All trainer IVs: 31");
static const u8 sAscensionText33[] = _("Boss IVs: base +");
static const u8 sAscensionText34[] = _(" (max 31)");
static const u8 sAscensionText35[] = _("Enemy EVs: none");
static const u8 sAscensionText36[] = _("Other trainer IVs: ");
static const u8 sAscensionText37[] = _("Boss ace IVs: 31");
static const u8 sAscensionText38[] = _("Boss aces: final party Pokemon");
static const u8 sAscensionText39[] = _("Natures: ");
static const u8 sAscensionText40[] = _("Moves: ");
static const u8 sAscensionText41[] = _("Items: ");
static const u8 sAscensionText42[] = _("Boss parties: 6 Pokemon");
static const u8 sAscensionText43[] = _("Boss parties: base +1 (max 6)");
static const u8 sAscensionText44[] = _("Boss parties: Standard sizes");
static const u8 sAscensionText45[] = _("Specialties: diverse");
static const u8 sAscensionText46[] = _("Specialties: Standard");
static const u8 sAscensionText47[] = _("Supplies: kit + ¥10,000");
static const u8 sAscensionText48[] = _("Supplies: hub preparation");
static const u8 sAscensionText49[] = _("Moves include matching Ability.");
static const u8 sAscensionText50[] = _("Base IVs: gyms 0, E4 5,");
static const u8 sAscensionText51[] = _("championship stages 10 / 15.");
static const u8 sAscensionText52[] = _("Fainted Pokemon leave the party.");
static const u8 sAscensionText53[] = _("Set mode; enforced level caps.");
static const u8 sAscensionText54[] = _("Battle bag: Poke Balls only.");
static const u8 sAscensionText55[] = _("Player EVs allowed; enemy EVs 0.");
static const u8 sAscensionText56[] = _("Species, item and legend clauses.");
static const u8 sAscensionText57[] = _("EXP All is enabled.");
static const u8 sAscensionText58[] = _("Standard AI, weather and gimmicks.");
static const u8 sAscensionText60[] = _("Profile natures, moves and items");
static const u8 sAscensionText61[] = _("unlock separately by ascension.");
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

static u8 Distribution(u8 level, u8 ace, u8 boss, u8 trainer)
{
    if (level >= trainer) return ASCENSION_UPGRADE_TRAINERS;
    if (level >= boss) return ASCENSION_UPGRADE_BOSSES;
    if (level >= ace) return ASCENSION_UPGRADE_ACES;
    return ASCENSION_UPGRADE_NONE;
}

void RogueAscension_Resolve(u8 level, struct RogueAscensionRules *rules)
{
    memset(rules, 0, sizeof(*rules));
    level = min(level, ASCENSION_MAX);
    rules->natures = Distribution(level, 2, 6, 11);
    rules->moves = Distribution(level, 3, 7, 12);
    rules->items = Distribution(level, 4, 8, 14);
    rules->bossIvBonus = level >= 13 ? 20 : level >= 5 ? 10 : level >= 1 ? 5 : 0;
    rules->trainerIv = level >= 16 ? 20 : level >= 9 ? 10 : 0;
    rules->perfectAces = level >= 17;
    rules->perfectTeams = level >= 18;
    rules->expandedParties = level >= 10;
    rules->fullParties = level >= 20;
    rules->diverseTrainers = level >= 15;
    rules->limitedSupplies = level >= 19;
}

bool8 RogueAscension_Applies(u8 distribution, bool8 boss, bool8 ace)
{
    return distribution == ASCENSION_UPGRADE_TRAINERS
        || (boss && (distribution == ASCENSION_UPGRADE_BOSSES
                 || (distribution == ASCENSION_UPGRADE_ACES && ace)));
}

u8 RogueAscension_CalculateIV(u8 level, u8 stage, bool8 boss, bool8 ace)
{
    struct RogueAscensionRules rules;
    u8 baseline = stage >= ROGUE_FINAL_CHAMP_DIFFICULTY ? 15
        : stage >= ROGUE_CHAMP_START_DIFFICULTY ? 10
        : stage >= ROGUE_ELITE_START_DIFFICULTY ? 5 : 0;
    RogueAscension_Resolve(level, &rules);
    if (rules.perfectTeams || (boss && ace && rules.perfectAces)) return 31;
    return boss ? min(31, max(baseline + rules.bossIvBonus, rules.trainerIv)) : rules.trainerIv;
}

u8 RogueAscension_PartySize(u8 level, u8 baseline, u8 capacity, bool8 boss)
{
    if (boss)
    {
        if (level >= 20) baseline = PARTY_SIZE;
        else if (level >= 10) ++baseline;
    }
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
    if (config->ascension < 19 && !config->trialFreshStart && AnyCharmsInBag())
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
    sAscensionText7,
    sAscensionText8,
    sAscensionText9,
    sAscensionText10,
    sAscensionText11,
    sAscensionText12,
    sAscensionText13,
    sAscensionText14,
    sAscensionText15,
    sAscensionText16,
    sAscensionText17,
    sAscensionText18,
    sAscensionText19,
    sAscensionText20,
    sAscensionText21,
    sAscensionText22,
    sAscensionText23,
    sAscensionText24,
    sAscensionText25,
    sAscensionText26,
    sAscensionText27,
};

const u8 *RogueAscension_GetAddition(u8 level)
{
    return sAdditions[min(level, ASCENSION_MAX)];
}

static const u8 *DistributionText(u8 distribution)
{
    switch (distribution)
    {
    case ASCENSION_UPGRADE_ACES: return sAscensionText28;
    case ASCENSION_UPGRADE_BOSSES: return sAscensionText29;
    case ASCENSION_UPGRADE_TRAINERS: return sAscensionText30;
    default: return sAscensionText31;
    }
}

u8 RogueAscension_GetRuleLine(u8 level, u8 line, u8 *dest)
{
    struct RogueAscensionRules r;
    u8 *end;
    RogueAscension_Resolve(level, &r);
    switch (line)
    {
    case 0:
        if (r.perfectTeams) StringCopy(dest, sAscensionText32);
        else
        {
            end = StringCopy(dest, sAscensionText33);
            end = ConvertIntToDecimalStringN(end, r.bossIvBonus, STR_CONV_MODE_LEFT_ALIGN, 2);
            StringCopy(end, sAscensionText34);
        }
        break;
    case 1:
        if (r.perfectTeams) StringCopy(dest, sAscensionText35);
        else ConvertIntToDecimalStringN(StringCopy(dest, sAscensionText36), r.trainerIv, STR_CONV_MODE_LEFT_ALIGN, 2);
        break;
    case 2: StringCopy(dest, r.perfectAces && !r.perfectTeams ? sAscensionText37 : sAscensionText38); break;
    case 3: StringCopy(StringCopy(dest, sAscensionText39), DistributionText(r.natures)); break;
    case 4: StringCopy(StringCopy(dest, sAscensionText40), DistributionText(r.moves)); break;
    case 5: StringCopy(StringCopy(dest, sAscensionText41), DistributionText(r.items)); break;
    case 6: StringCopy(dest, r.fullParties ? sAscensionText42 : r.expandedParties ? sAscensionText43 : sAscensionText44); break;
    case 7: StringCopy(dest, r.diverseTrainers ? sAscensionText45 : sAscensionText46); break;
    case 8: StringCopy(dest, r.limitedSupplies ? sAscensionText47 : sAscensionText48); break;
    case 9: StringCopy(dest, sAscensionText49); break;
    case 10: StringCopy(dest, r.perfectTeams ? sAscensionText55 : sAscensionText50); break;
    case 11: StringCopy(dest, r.perfectTeams ? sAscensionText58 : sAscensionText51); break;
    default: return FALSE;
    }
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
    else if (config->ascension >= 19)
    {
        if (line == 10) StringCopy(dest, sKit);
        if (line == 11) StringCopy(dest, sKeep);
    }
    return TRUE;
}

u8 RogueAscension_GetBaseLine(u8 line, u8 *dest)
{
    static const u8 *const lines[] = {
        sAscensionText52,
        sAscensionText53,
        sAscensionText54,
        sAscensionText55,
        sAscensionText56,
        sAscensionText57,
        sAscensionText60,
        sAscensionText61,
    };
    if (line >= ARRAY_COUNT(lines) || (line >= 6 && !RogueAscension_IsRevealed())) return FALSE;
    StringCopy(dest, lines[line]);
    return TRUE;
}
