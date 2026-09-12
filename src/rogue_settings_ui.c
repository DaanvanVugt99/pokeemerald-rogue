#include "global.h"
static const u8 sAscensionText0[] = _("Ascension");
static const u8 sAscensionText1[] = _("Adventure");
static const u8 sAscensionText2[] = _("Battle format");
static const u8 sAscensionText3[] = _("Pokédex");
static const u8 sAscensionText4[] = _("Trainers");
static const u8 sAscensionText5[] = _("Encounters");
static const u8 sAscensionText6[] = _("Starting team");
static const u8 sAscensionText7[] = _("Base rules");
static const u8 sAscensionText8[] = _("Save and close");
static const u8 sAscensionText9[] = _("Singles");
static const u8 sAscensionText10[] = _("Doubles");
static const u8 sAscensionText11[] = _("Mixed");
static const u8 sAscensionText12[] = _("Default");
static const u8 sAscensionText13[] = _("Rainbow");
static const u8 sAscensionText14[] = _("Official");
static const u8 sAscensionText15[] = _("Rogue");
static const u8 sAscensionText16[] = _("Kanto");
static const u8 sAscensionText17[] = _("Johto");
static const u8 sAscensionText18[] = _("Hoenn");
static const u8 sAscensionText19[] = _("Sinnoh");
static const u8 sAscensionText20[] = _("Unova");
static const u8 sAscensionText21[] = _("Kalos");
static const u8 sAscensionText22[] = _("Alola");
static const u8 sAscensionText23[] = _("Galar");
static const u8 sAscensionText24[] = _("Paldea");
static const u8 sAscensionText25[] = _("Choose a level. A opens its rules.");
static const u8 sAscensionText26[] = _("Only Standard advances ascension.");
static const u8 sAscensionText27[] = _("Each format has its own ladder.");
static const u8 sAscensionText28[] = _("Choose which Pokemon can appear.");
static const u8 sAscensionText29[] = _("A: choose trainer regions and order.");
static const u8 sAscensionText30[] = _("Overworld Pokemon or traditional grass.");
static const u8 sAscensionText31[] = _("Current party or the starter bag.");
static const u8 sAscensionText32[] = _("Read the fixed adventure rules.");
static const u8 sAscensionText33[] = _("Confirm these adventure settings.");
static const u8 sAscensionText34[] = _("A");
static const u8 sAscensionText35[] = _("Gauntlet");
static const u8 sAscensionText36[] = _("Slow Path");
static const u8 sAscensionText37[] = _("Standard");
static const u8 sAscensionText38[] = _("Select Pokédex");
static const u8 sAscensionText39[] = _("Visible");
static const u8 sAscensionText40[] = _("Traditional");
static const u8 sAscensionText41[] = _("Starter bag");
static const u8 sAscensionText42[] = _("Current party");
static const u8 sAscensionText43[] = _("ASCENSION ");
static const u8 sAscensionText44[] = _(" - LOCKED");
static const u8 sAscensionText45[] = _("Unlocked: A");
static const u8 sAscensionText46[] = _("Left/Right: level  L/R: page  B: back");
static const u8 sAscensionText47[] = _("ADVENTURE RULES");
static const u8 sAscensionText48[] = _("TRAINERS");
static const u8 sAscensionText49[] = _("SET UP ADVENTURE");
static const u8 sAscensionText50[] = _("Order");
static const u8 sAscensionText51[] = _("On");
static const u8 sAscensionText52[] = _("Off");
static const u8 sAscensionText53[] = _("Begin Adventure");
static const u8 sAscensionText54[] = _("Close");
static const u8 sAscensionText55[] = _("A/Left/Right: change  B: back");
static const u8 sAscensionText56[] = _("Select at least one trainer region.");
static const u8 sAscensionText57[] = _("Check starting team and requirements.");
static const u8 sAscensionText58[] = _("Clear the preceding level to unlock.");
static const u8 sAscensionText59[] = _("Clear an adventure to edit this.");
static const u8 sChooseTrial[] = _("Choose Trial");
static const u8 sChooseTrialHelp[] = _("Return to the Trial selection list.");
#include "main.h"
#include "malloc.h"
#include "menu.h"
#include "event_data.h"
#include "palette.h"
#include "sprite.h"
#include "task.h"
#include "bg.h"
#include "gpu_regs.h"
#include "window.h"
#include "text.h"
#include "script.h"
#include "string_util.h"
#include "constants/rgb.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_charms.h"
#include "rogue_pokedex.h"
#include "rogue_pokedex_selection.h"
#include "constants/items.h"
#include "constants/vars.h"
#include "rogue_run_start.h"
#include "rogue_save.h"
#include "rogue_settings.h"
#include "rogue_ascension.h"
#include "rogue_trials.h"
#include "rogue_hub.h"
#include "constants/flags.h"
#include "constants/rogue_hub.h"

// Shared by the entrance, C-Gear and Config Lab. Only an entrance context can start.
enum { PAGE_SETUP, PAGE_ASCENSION, PAGE_TRAINERS, PAGE_POOL };
enum { MENU_ADVENTURE, MENU_INTRO, MENU_CURATED_POOL, MENU_TRIAL_POOL };
enum { ROW_ASCENSION, ROW_MODE, ROW_FORMAT, ROW_DEX, ROW_TRAINERS, ROW_ENCOUNTERS,
       ROW_TEAM, ROW_RULES, ROW_DONE, ROW_TRIAL, ROW_COUNT };
struct AdventureMenu
{
    struct RogueAdventureConfig config;
    struct RoguePokedexSelection pool;
    u16 startingGimmick;
    u16 nationalGimmick;
    u8 poolTrialId;
    bool8 intro;
    bool8 poolOnly;
    bool8 curatedPool;
    u8 remembered[3];
    u8 rows[11];
    u8 rowCount;
    u8 page;
    u8 row;
    u8 top;
    u8 candidate;
    u8 rulesPage;
    bool8 readOnly;
    bool8 entrance;
    bool8 closing;
    const u8 *message;
};
static EWRAM_DATA struct AdventureMenu *sMenu;
static const struct BgTemplate sBg[] = {{ .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .paletteMode = 0 }};
static const struct WindowTemplate sWindows[] = {
    {.bg = 0, .tilemapLeft = 1, .tilemapTop = 1, .width = 28, .height = 18, .paletteNum = 15, .baseBlock = 1},
    DUMMY_WIN_TEMPLATE
};
static const u16 sPalette[16] = {RGB(2,3,7), RGB_WHITE, RGB(8,9,12), RGB(31,25,8), RGB(14,18,24)};
static const u8 sColors[] = {0, 1, 2};
static const u8 sSelectedColors[] = {0, 3, 2};
static const u8 *const sRows[] = {sAscensionText0, sAscensionText1, sAscensionText2, sAscensionText3,
    sAscensionText4, sAscensionText5, sAscensionText6, sAscensionText7, sAscensionText8, sChooseTrial};
static const u8 *const sFormats[] = {sAscensionText9, sAscensionText10, sAscensionText11};
static const u8 *const sOrders[] = {sAscensionText12, sAscensionText13, sAscensionText14};
static const u8 *const sRegions[] = {sAscensionText15, sAscensionText16, sAscensionText17, sAscensionText18, sAscensionText19, sAscensionText20, sAscensionText21, sAscensionText22, sAscensionText23, sAscensionText24};
static const u8 *const sDescriptions[] = {
    sAscensionText25,
    sAscensionText26,
    sAscensionText27,
    sAscensionText28,
    sAscensionText29,
    sAscensionText30,
    sAscensionText31,
    sAscensionText32,
    sAscensionText33,
    sChooseTrialHelp
};

static const u8 sTrialRestriction[] = _("This choice is fixed by the Trial.");
static const u8 sTrialDisabled[] = _("Trial records/rewards disabled");
static const u8 sTrialMinimum[] = _("Trial minimum: A");
static const u8 sBasePage[] = _("BASE RULES");
static const u8 sFixedRules[] = _("These settings cannot be changed.");
static const u8 sCumulativePage[] = _("ACTIVE RULES");
static const u8 sTrialTeam[] = _("Trial team");
static const u8 sModeBeforeAscension[] = _("Choose your adventure style.");
static const u8 sFormatBeforeAscension[] = _("Choose Singles, Doubles or Mixed.");
static const u8 sBaseControls[] = _("L/R: page  B: back");
static const u8 sPoolTitle[] = _("POKÉDEX");
static const u8 sPoolType[] = _("Pokédex type");
static const u8 sPoolRegion[] = _("Region");
static const u8 sPoolGame[] = _("Game");
static const u8 sPoolRoster[] = _("Roster");
static const u8 sPoolGeneration[] = _("Through gen.");
static const u8 sGeneration[] = _("Generation ");
static const u8 sPoolGimmick[] = _("Starting gimmick");
static const u8 sUsePool[] = _("Use this Pokédex");
static const u8 sPoolCount[] = _(" Pokemon");
static const u8 sPoolStart[] = _("Start: ");
static const u8 sContinue[] = _("Continue");
static const u8 sIntroHelp[] = _("Choose Continue when you're ready.");
static const u8 sPoolReadOnly[] = _("B: back");
static const u8 sIntroPoolHelp[] = _("Shapes encounters and first partners.");
static const u8 sPoolUnavailable[] = _("No compatible Pokédexes.");
static const u8 sLockedTeam[] = _("Starter bag is not available here.");

static const struct RogueTrialDefinition *MenuTrial(void)
{
    const struct RogueRunStartContext *context = RogueRunStart_GetContext();
    if (sMenu != NULL && sMenu->poolOnly && sMenu->poolTrialId != ROGUE_TRIAL_NONE)
        return RogueTrial_GetDefinition(sMenu->poolTrialId);
    if (Rogue_IsRunActive()) return gRogueRun.trialState.trialId != ROGUE_TRIAL_NONE
        ? RogueTrial_GetDefinition(gRogueRun.trialState.trialId) : NULL;
    return context != NULL && context->trialId != ROGUE_TRIAL_NONE
        ? RogueTrial_GetDefinition(context->trialId) : NULL;
}

static const u8 *StartFailureText(void)
{
    static const u8 sEmpty[] = _("Choose a starting Pokemon.");
    static const u8 sCapacity[] = _("Your starting party is too large.");
    static const u8 sIllegal[] = _("Starting Pokemon is not eligible.");
    static const u8 sDayCare[] = _("Day Care Pokemon is not eligible.");
    static const u8 sSpecies[] = _("Team repeats an evolution line.");
    static const u8 sLegend[] = _("Team has too many legendaries.");
    static const u8 sInvalid[] = _("Adventure setup is invalid.");
    static const u8 sHost[] = _("Waiting for the host to begin.");
    const struct RogueRunStartContext *context = RogueRunStart_GetContext();
    if (context == NULL) return sInvalid;
    if (context->readiness == RUN_START_WAITING_FOR_HOST) return sHost;
    switch (context->readinessReason)
    {
    case RUN_START_REASON_PARTY_EMPTY: return sEmpty;
    case RUN_START_REASON_PARTY_CAPACITY: return sCapacity;
    case RUN_START_REASON_PARTY_ILLEGAL: return sIllegal;
    case RUN_START_REASON_DAY_CARE_ILLEGAL: return sDayCare;
    case RUN_START_REASON_SPECIES_CLAUSE: return sSpecies;
    case RUN_START_REASON_LEGENDARY_CLAUSE: return sLegend;
    case RUN_START_REASON_ASCENSION_LOCKED: return sAscensionText58;
    case RUN_START_REASON_INVALID_CONFIG: return sInvalid;
    default: return sAscensionText57;
    }
}

static bool8 TrialRecordsDisabled(void)
{
    if (MenuTrial() == NULL) return FALSE;
    if (Rogue_IsRunActive()) return Rogue_ShouldDisableTrialQuests();
    return sMenu->config.trainerOrder == TRAINER_ORDER_OFFICIAL
        || (sMenu->config.ascension < 19 && !sMenu->config.trialFreshStart && AnyCharmsInBag());
}

static bool8 TrialAllowsConfig(const struct RogueAdventureConfig *config)
{
    struct RogueAdventureConfig validated = *config;
    const struct RogueTrialDefinition *trial = MenuTrial();
    return trial == NULL || RogueTrial_BuildSelectionConfig(trial->id, config->ascension, config->pokedexVariant, &validated);
}

static bool8 SetupRowAvailable(u8 row)
{
    const struct RogueTrialDefinition *trial = MenuTrial();
    const struct RogueRunStartContext *context = RogueRunStart_GetContext();
    if (sMenu->intro)
    {
        if (row == ROW_DEX)
        {
#ifdef ROGUE_EXPANSION
            return TRUE;
#else
            return FALSE;
#endif
        }
        return row == ROW_FORMAT || row == ROW_ENCOUNTERS || row == ROW_RULES || row == ROW_DONE;
    }
    if (row == ROW_ASCENSION && !RogueAscension_IsRevealed()) return FALSE;
    if (row == ROW_TRIAL)
        return !sMenu->readOnly && sMenu->entrance && context != NULL && context->source == RUN_START_SOURCE_TRIAL;
    if (sMenu->readOnly) return TRUE;
    switch (row)
    {
    case ROW_ASCENSION:
        return RogueAscension_IsRevealed() && (trial == NULL || !trial->hasFixedAscension);
    case ROW_MODE:
        return trial == NULL && Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_GAUNTLET);
    case ROW_FORMAT:
        return trial == NULL || !trial->hasForcedBattleFormat;
    case ROW_DEX:
        return TRUE;
    case ROW_TRAINERS:
        return Rogue_HasPostgameSettingsUnlocked()
            && (trial == NULL || !trial->hasForcedTrainerOrder
             || (trial->forcedTrainerToggle == ROGUE_TRIAL_NO_TRAINER_TOGGLE && !trial->enableAllRegionalTrainers));
    case ROW_TEAM:
        if (trial != NULL && (trial->forceRandomStarter || trial->fixedStartingPartyCount)) return FALSE;
        return context != NULL ? context->canUseCurrentParty && context->canUseStarterBag
            : RogueHub_HasUpgrade(HUB_UPGRADE_ADVENTURE_ENTRANCE_RANDOM_STARTER);
    default:
        return TRUE;
    }
}

static bool8 IsPoolEditingLocked(void)
{
    return !sMenu->intro && !sMenu->readOnly && !sMenu->poolOnly
        && MenuTrial() == NULL && !RoguePokedex_IsVariantEditEnabled();
}

static const u8 *DescriptionForRow(u8 row)
{
    if (row == ROW_DEX && IsPoolEditingLocked()) return sAscensionText59;
    if (sMenu->intro && row == ROW_DEX) return sIntroPoolHelp;
    if (!RogueAscension_IsRevealed())
    {
        if (row == ROW_MODE) return sModeBeforeAscension;
        if (row == ROW_FORMAT) return sFormatBeforeAscension;
    }
    return sDescriptions[row];
}

static u64 PoolAvailability(void)
{
    const struct RogueTrialDefinition *trial = MenuTrial();
    u64 mask = 0;
    u8 variant;
    for (variant = 0; variant < POKEDEX_VARIANT_COUNT; ++variant)
    {
        struct RogueAdventureConfig candidate = sMenu->config;
        if ((sMenu->intro || sMenu->curatedPool) && !RoguePokedex_IsCuratedVariant(variant)) continue;
        if (trial != NULL && !RogueTrial_BuildSelectionConfig(trial->id, candidate.ascension, variant, &candidate)) continue;
        mask |= 1ULL << variant;
    }
    if (sMenu->readOnly && sMenu->config.pokedexVariant < POKEDEX_VARIANT_COUNT)
        mask |= 1ULL << sMenu->config.pokedexVariant;
    return mask;
}

static void BuildPoolRows(void)
{
    u8 field, choices[POKEDEX_VARIANT_COUNT];
    sMenu->page = PAGE_POOL;
    sMenu->row = sMenu->top = sMenu->rowCount = 0;
    for (field = DEX_FIELD_TYPE; field < DEX_FIELD_DONE; ++field)
        if (RogueDexSelection_GetChoices(&sMenu->pool, field, choices) > 1)
            sMenu->rows[sMenu->rowCount++] = field;
    sMenu->rows[sMenu->rowCount++] = DEX_FIELD_DONE;
}

static bool8 BeginPool(void)
{
    if (IsPoolEditingLocked())
    {
        sMenu->message = sAscensionText59;
        return FALSE;
    }
    if (!RogueDexSelection_Init(&sMenu->pool, PoolAvailability(), sMenu->config.pokedexVariant,
            sMenu->nationalGimmick, sMenu->intro))
    {
        sMenu->message = sPoolUnavailable;
        return FALSE;
    }
    sMenu->message = NULL;
    BuildPoolRows();
    return TRUE;
}

static void BuildRows(u8 page)
{
    const struct RogueTrialDefinition *trial = MenuTrial();
    u8 row;
    if (page == PAGE_SETUP && !sMenu->readOnly && !Rogue_IsAdventureModeAvailable(sMenu->config.mode))
        sMenu->config.mode = ROGUE_GAME_MODE_STANDARD;
    sMenu->page = page;
    sMenu->row = sMenu->top = sMenu->rowCount = 0;
    if (page == PAGE_SETUP && sMenu->entrance)
        sMenu->rows[sMenu->rowCount++] = ROW_DONE;
    for (row = 0; row < (page == PAGE_SETUP ? ROW_COUNT : ARRAY_COUNT(sRegions) + 1); ++row)
    {
        bool8 available = page == PAGE_SETUP ? SetupRowAvailable(row)
            : sMenu->readOnly || trial == NULL || (row == 0 ? !trial->hasForcedTrainerOrder
                : trial->forcedTrainerToggle == ROGUE_TRIAL_NO_TRAINER_TOGGLE && !trial->enableAllRegionalTrainers);
        if (available && !(page == PAGE_SETUP && sMenu->entrance && row == ROW_DONE))
            sMenu->rows[sMenu->rowCount++] = row;
    }
}

static const u8 *LabelForRow(u8 row)
{
    return row == ROW_DONE ? sMenu->intro ? sContinue : sMenu->entrance ? sAscensionText53 : sMenu->readOnly ? sAscensionText54 : sAscensionText8 : sRows[row];
}

#if TESTING
const u8 *RogueTest_AdventureSetupOption(u8 index, bool8 readOnly)
{
    struct AdventureMenu menu = {0};
    struct AdventureMenu *previous = sMenu;
    const u8 *result;
    Rogue_CopyAdventureConfig(&menu.config);
    menu.readOnly = readOnly;
    menu.entrance = RogueRunStart_GetContext() != NULL;
    sMenu = &menu;
    BuildRows(PAGE_SETUP);
    result = index < menu.rowCount ? LabelForRow(menu.rows[index]) : NULL;
    sMenu = previous;
    return result;
}

bool8 RogueTest_AdventurePokedexCanOpen(void)
{
    struct AdventureMenu menu = {0};
    struct AdventureMenu *previous = sMenu;
    bool8 result;
    Rogue_CopyAdventureConfig(&menu.config);
    sMenu = &menu;
    result = BeginPool();
    sMenu = previous;
    return result;
}
#endif

static void Print(u8 x, u8 y, const u8 *str, bool8 selected)
{
    AddTextPrinterParameterized4(0, FONT_SMALL_NARROW, x, y, 0, 0, selected ? sSelectedColors : sColors, TEXT_SKIP_DRAW, str);
}

static u8 Cycle(u8 value, u8 count, s8 dir)
{
    return (value + count + dir) % count;
}

static void ValueForRow(u8 row, u8 *text)
{
    switch (row)
    {
    case ROW_ASCENSION:
        ConvertIntToDecimalStringN(StringCopy(text, sAscensionText34), sMenu->config.ascension, STR_CONV_MODE_LEFT_ALIGN, 2); break;
    case ROW_MODE:
        StringCopy(text, sMenu->config.mode == ROGUE_GAME_MODE_GAUNTLET ? sAscensionText35 : sMenu->config.mode == ROGUE_GAME_MODE_SLOW_PATH ? sAscensionText36 : sAscensionText37); break;
    case ROW_FORMAT: StringCopy(text, sFormats[sMenu->config.battleFormat]); break;
    case ROW_DEX:
        StringCopy(text, sMenu->config.pokedexVariant < POKEDEX_VARIANT_COUNT ? gPokedexVariants[sMenu->config.pokedexVariant].displayName : sAscensionText38); break;
    case ROW_TRAINERS: StringCopy(text, sOrders[sMenu->config.trainerOrder]); break;
    case ROW_ENCOUNTERS: StringCopy(text, sMenu->config.overworldMons ? sAscensionText39 : sAscensionText40); break;
    case ROW_TEAM:
        if (MenuTrial() != NULL && MenuTrial()->fixedStartingPartyCount) StringCopy(text, sTrialTeam);
        else if (MenuTrial() != NULL && MenuTrial()->forceRandomStarter) StringCopy(text, sAscensionText41);
        else StringCopy(text, sMenu->config.startingTeam == RUN_START_TEAM_SOURCE_STARTER_BAG ? sAscensionText41 : sAscensionText42);
        break;
    default: text[0] = EOS; break;
    }
    while (GetStringWidth(FONT_SMALL_NARROW, text, 0) > 107 && StringLength(text) > 0)
        text[StringLength(text) - 1] = EOS;
}

static void DrawPool(void)
{
    u8 i, field, text[160], type = RogueDexSelection_GetType(sMenu->pool.variant);
    Print(0, 0, sPoolTitle, TRUE);
    Print(0, 13, gPokedexVariants[sMenu->pool.variant].displayName, FALSE);
    for (i = 0; i < sMenu->rowCount; ++i)
    {
        const u8 *label = sUsePool;
        field = sMenu->rows[i];
        text[0] = EOS;
        switch (field)
        {
        case DEX_FIELD_TYPE:
            label = sPoolType;
            StringCopy(text, RogueDexSelection_GetTypeName(type)); break;
        case DEX_FIELD_REGION:
            label = sPoolRegion;
            StringCopy(text, RogueDexSelection_GetRegionName(RogueDexSelection_GetRegion(sMenu->pool.variant))); break;
        case DEX_FIELD_VARIANT:
            label = type == DEX_POOL_NATIONAL ? sPoolGeneration : type == DEX_POOL_SPECIAL ? sPoolRoster : sPoolGame;
            if (type == DEX_POOL_NATIONAL)
                ConvertIntToDecimalStringN(StringCopy(text, sGeneration), sMenu->pool.variant - POKEDEX_VARIANT_NATIONAL_GEN1 + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
            else StringCopy(text, gPokedexVariants[sMenu->pool.variant].displayName);
            break;
        case DEX_FIELD_GIMMICK:
            label = sPoolGimmick;
            StringCopy(text, RogueDexSelection_GetGimmickName(RogueDexSelection_GetGimmick(&sMenu->pool))); break;
        case DEX_FIELD_DONE:
            if (sMenu->readOnly) label = sAscensionText54;
            break;
        }
        Print(0, 28 + i * 14, label, i == sMenu->row);
        Print(90, 28 + i * 14, text, i == sMenu->row);
    }
    StringCopy(ConvertIntToDecimalStringN(text, gPokedexVariants[sMenu->pool.variant].speciesCount,
        STR_CONV_MODE_LEFT_ALIGN, 4), sPoolCount);
    Print(0, 100, text, FALSE);
    if (sMenu->intro)
    {
        StringCopy(StringCopy(text, sPoolStart), RogueDexSelection_GetGimmickName(RogueDexSelection_GetGimmick(&sMenu->pool)));
        Print(82, 100, text, FALSE);
    }
    RogueDexSelection_Describe(&sMenu->pool, text);
    Print(0, 114, text, FALSE);
    Print(0, 132, sMenu->message != NULL ? sMenu->message : sMenu->readOnly ? sPoolReadOnly : sAscensionText55, FALSE);
}

static u8 BaseRulePageCount(void)
{
    u8 text[160];
    return RogueAscension_GetBaseLine(6, text) ? 2 : 1;
}

static void Draw(void)
{
    u8 i;
    u8 text[160];
    struct RogueAdventureConfig preview = sMenu->config;
    preview.ascension = sMenu->candidate;
    FillWindowPixelBuffer(0, PIXEL_FILL(0));
    if (sMenu->page == PAGE_POOL)
        DrawPool();
    else if (sMenu->page == PAGE_ASCENSION)
    {
        bool8 baseOnly = sMenu->rows[sMenu->row] == ROW_RULES;
        u8 *end = ConvertIntToDecimalStringN(StringCopy(text, sAscensionText43), sMenu->candidate, STR_CONV_MODE_LEFT_ALIGN, 2);
        if (!RogueAscension_IsUnlocked(sMenu->candidate, sMenu->config.battleFormat)) StringCopy(end, sAscensionText44);
        Print(0, 0, baseOnly ? sBasePage : text, TRUE);
        ConvertIntToDecimalStringN(StringCopy(text, sAscensionText45), RogueAscension_GetUnlocked(sMenu->config.battleFormat), STR_CONV_MODE_LEFT_ALIGN, 2);
        if (!baseOnly) Print(120, 13, text, FALSE);
        Print(0, 13, baseOnly ? sFixedRules : sMenu->rulesPage >= 2 ? sBasePage : sCumulativePage, FALSE);
        for (i = 0; i < 6; ++i)
        {
            bool8 hasLine = sMenu->rulesPage < 2
                ? RogueAscension_GetConfigRuleLine(&preview, sMenu->rulesPage * 6 + i, text)
                : RogueAscension_GetBaseLine((sMenu->rulesPage - 2) * 6 + i, text);
            if (hasLine) Print(0, 29 + i * 12, text, FALSE);
        }
        if (!baseOnly) Print(0, 103, RogueAscension_GetAddition(sMenu->candidate), TRUE);
        Print(0, 132, sMenu->message != NULL ? sMenu->message : baseOnly ? (BaseRulePageCount() > 1 ? sBaseControls : sPoolReadOnly) : sAscensionText46, FALSE);
    }
    else
    {
        u8 count = sMenu->rowCount;
        Print(0, 0, sMenu->readOnly ? sAscensionText47 : sMenu->page == PAGE_TRAINERS ? sAscensionText48 : sAscensionText49, TRUE);
        if (sMenu->row < sMenu->top) sMenu->top = sMenu->row;
        if (sMenu->row >= sMenu->top + 6) sMenu->top = sMenu->row - 5;
        for (i = 0; i < 6 && i + sMenu->top < count; ++i)
        {
            u8 row = sMenu->rows[i + sMenu->top];
            bool8 selected = i + sMenu->top == sMenu->row;
            if (sMenu->page == PAGE_TRAINERS)
            {
                Print(0, 22 + 14*i, row == 0 ? sAscensionText50 : sRegions[row-1], selected);
                Print(112, 22 + 14*i, row == 0 ? sOrders[sMenu->config.trainerOrder] : (sMenu->config.trainerRegions & (1 << (row-1))) ? sAscensionText51 : sAscensionText52, selected);
            }
            else
            {
                const u8 *label = LabelForRow(row);
                Print(0, 22 + 14*i, label, selected);
                ValueForRow(row, text);
                Print(112, 22 + 14*i, text, selected);
            }
        }
        if (sMenu->intro && sMenu->page == PAGE_SETUP)
        {
            StringCopy(StringCopy(text, sPoolStart), RogueDexSelection_GetGimmickName(sMenu->startingGimmick));
            Print(0, 111, text, FALSE);
        }
        else if (sMenu->page == PAGE_SETUP && RogueAscension_IsRevealed())
            Print(0, 111, TrialRecordsDisabled() ? sTrialDisabled : RogueAscension_EligibilityText(Rogue_IsRunActive() ? gRogueSaveBlock->ascensionEligibilityReason : RogueAscension_GetEligibility(&sMenu->config, RogueRunStart_GetContext() != NULL ? RogueRunStart_GetContext()->source : RUN_START_SOURCE_NORMAL)), FALSE);
        if (RogueAscension_IsRevealed() && MenuTrial() != NULL && sMenu->message == NULL)
        {
            ConvertIntToDecimalStringN(StringCopy(text, sTrialMinimum), MenuTrial()->minimumAscension, STR_CONV_MODE_LEFT_ALIGN, 2);
            Print(0, 123, text, FALSE);
        }
        else Print(0, 123, sMenu->message != NULL ? sMenu->message : sMenu->page == PAGE_TRAINERS ? sAscensionText55 : DescriptionForRow(sMenu->rows[sMenu->row]), FALSE);
    }
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_FULL);
}

static void CloseMenu(u16 result)
{
    gSpecialVar_Result = result;
    sMenu->closing = TRUE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static void ExitMenu(u8 taskId, bool8 accept, bool8 start)
{
    if (sMenu->intro && accept)
    {
        if (!RoguePokedex_IsCuratedVariant(sMenu->config.pokedexVariant))
        { sMenu->message = sPoolUnavailable; Draw(); return; }
        Rogue_ApplyAdventureConfig(&sMenu->config);
        gSpecialVar_0x8006 = sMenu->config.pokedexVariant;
        gSpecialVar_0x8005 = sMenu->startingGimmick;
        RoguePokedex_StoreInitialSelection();
        if (!gSpecialVar_Result) { sMenu->message = sPoolUnavailable; Draw(); return; }
    }
    else if (accept && !sMenu->readOnly && !sMenu->poolOnly)
    {
        if (!TrialAllowsConfig(&sMenu->config))
        { sMenu->message = sTrialRestriction; Draw(); return; }
        if (sMenu->config.trainerRegions == 0)
        {
            sMenu->message = sAscensionText56; Draw(); return;
        }
        if (sMenu->entrance)
        {
            RogueRunStart_UpdateConfig(&sMenu->config);
            if (start && !RogueRunStart_CanStart())
            {
                sMenu->message = StartFailureText(); Draw(); return;
            }
        }
        if (MenuTrial() == NULL)
        {
            Rogue_ApplyAdventureConfig(&sMenu->config);
            memcpy(gRogueSaveBlock->selectedAscension, sMenu->remembered, sizeof(sMenu->remembered));
            gRogueSaveBlock->selectedAscension[sMenu->config.battleFormat] = sMenu->config.ascension;
        }
    }
    if (start)
    {
        RogueRunStart_Refresh();
        if (!RogueRunStart_CanStart())
        { sMenu->message = StartFailureText(); Draw(); return; }
    }
    if (sMenu->poolOnly && accept) gSpecialVar_0x8006 = sMenu->pool.variant;
    CloseMenu(sMenu->intro || sMenu->poolOnly ? accept : start ? RUN_REVIEW_ACTION_START : RUN_REVIEW_ACTION_BACK);
}

static void ReturnToSetup(u8 field)
{
    BuildRows(PAGE_SETUP);
    while (sMenu->row + 1 < sMenu->rowCount && sMenu->rows[sMenu->row] != field) ++sMenu->row;
    sMenu->message = NULL;
}

static void HandlePoolInput(u8 taskId, s8 direction)
{
    u8 field = sMenu->rows[sMenu->row];
    if (JOY_NEW(B_BUTTON))
    {
        if (sMenu->poolOnly) ExitMenu(taskId, FALSE, FALSE);
        else ReturnToSetup(ROW_DEX);
    }
    else if (JOY_REPEAT(DPAD_UP | DPAD_DOWN))
        sMenu->row = Cycle(sMenu->row, sMenu->rowCount, JOY_REPEAT(DPAD_UP) ? -1 : 1);
    else if (field == DEX_FIELD_DONE && JOY_NEW(A_BUTTON))
    {
        if (sMenu->poolOnly) ExitMenu(taskId, !sMenu->readOnly, FALSE);
        else
        {
            if (!sMenu->readOnly)
            {
                sMenu->config.pokedexVariant = sMenu->pool.variant;
                if (sMenu->intro)
                {
                    sMenu->startingGimmick = RogueDexSelection_GetGimmick(&sMenu->pool);
                    sMenu->nationalGimmick = sMenu->pool.nationalGimmick;
                }
            }
            ReturnToSetup(ROW_DEX);
        }
    }
    else if (!sMenu->readOnly && (direction || JOY_NEW(A_BUTTON)))
    {
        RogueDexSelection_Cycle(&sMenu->pool, field, direction ? direction : 1);
        BuildPoolRows();
        while (sMenu->row + 1 < sMenu->rowCount && sMenu->rows[sMenu->row] != field) ++sMenu->row;
    }
    Draw();
}

static void Task_Input(u8 taskId)
{
    u8 row = sMenu->rows[sMenu->row];
    s8 dir = JOY_REPEAT(DPAD_LEFT) ? -1 : JOY_REPEAT(DPAD_RIGHT) ? 1 : 0;
    if (gPaletteFade.active) return;
    if (sMenu->closing)
    {
        FreeAllWindowBuffers();
        Free(sMenu); sMenu = NULL;
        DestroyTask(taskId);
        SetMainCallback2(gMain.savedCallback);
        return;
    }
    if (sMenu->page == PAGE_POOL) { HandlePoolInput(taskId, dir); return; }
    if (sMenu->page == PAGE_ASCENSION)
    {
        if (JOY_NEW(B_BUTTON)) { sMenu->page = PAGE_SETUP; sMenu->message = NULL; }
        else if (JOY_NEW(L_BUTTON | R_BUTTON)) sMenu->rulesPage = row == ROW_RULES
            ? 2 + Cycle(sMenu->rulesPage - 2, BaseRulePageCount(), JOY_NEW(L_BUTTON) ? -1 : 1)
            : Cycle(sMenu->rulesPage, 2 + BaseRulePageCount(), JOY_NEW(L_BUTTON) ? -1 : 1);
        else if (dir && !sMenu->readOnly && row != ROW_RULES) { sMenu->candidate = Cycle(sMenu->candidate, ASCENSION_MAX + 1, dir); sMenu->message = NULL; }
        else if (JOY_NEW(A_BUTTON) && !sMenu->readOnly && row != ROW_RULES)
        {
            struct RogueAdventureConfig candidate = sMenu->config;
            candidate.ascension = sMenu->candidate;
            if (RogueAscension_IsUnlocked(sMenu->candidate, sMenu->config.battleFormat) && TrialAllowsConfig(&candidate))
            { sMenu->config.ascension = sMenu->candidate; sMenu->page = PAGE_SETUP; sMenu->message = NULL; }
            else sMenu->message = RogueAscension_IsUnlocked(sMenu->candidate, sMenu->config.battleFormat) ? sTrialRestriction : sAscensionText58;
        }
        Draw(); return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        if (sMenu->page == PAGE_TRAINERS)
        {
            BuildRows(PAGE_SETUP);
            while (sMenu->row + 1 < sMenu->rowCount && sMenu->rows[sMenu->row] != ROW_TRAINERS) ++sMenu->row;
            sMenu->message = NULL; Draw();
        }
        else if (sMenu->intro) { sMenu->message = sIntroHelp; Draw(); }
        else ExitMenu(taskId, FALSE, FALSE);
        return;
    }
    if (JOY_REPEAT(DPAD_UP | DPAD_DOWN))
    {
        u8 count = sMenu->rowCount;
        sMenu->row = Cycle(sMenu->row, count, JOY_REPEAT(DPAD_UP) ? -1 : 1);
        sMenu->message = NULL; Draw(); return;
    }
    if (!dir && !JOY_NEW(A_BUTTON)) return;
    if (sMenu->page == PAGE_TRAINERS)
    {
        if (!sMenu->readOnly)
        {
            const struct RogueTrialDefinition *trial = MenuTrial();
            if (trial != NULL && ((row == 0 && trial->hasForcedTrainerOrder)
             || (row != 0 && (trial->forcedTrainerToggle != ROGUE_TRIAL_NO_TRAINER_TOGGLE || trial->enableAllRegionalTrainers))))
            { sMenu->message = sTrialRestriction; Draw(); return; }
            if (row == 0) sMenu->config.trainerOrder = Cycle(sMenu->config.trainerOrder, 3, dir ? dir : 1);
            else sMenu->config.trainerRegions ^= 1 << (row - 1);
        }
        Draw(); return;
    }
    if (row == ROW_ASCENSION || row == ROW_RULES)
    {
        sMenu->page = PAGE_ASCENSION;
        sMenu->candidate = sMenu->config.ascension;
        sMenu->rulesPage = row == ROW_RULES ? 2 : 0;
    }
    else if (row == ROW_DEX) { BeginPool(); }
    else if (row == ROW_TRAINERS) { BuildRows(PAGE_TRAINERS); }
    else if (row == ROW_TRIAL)
    {
        if (JOY_NEW(A_BUTTON)) CloseMenu(RUN_REVIEW_ACTION_CHOOSE_TRIAL);
        return;
    }
    else if (row == ROW_DONE)
    {
        if (JOY_NEW(A_BUTTON)) ExitMenu(taskId, TRUE, sMenu->entrance);
        return;
    }
    else if (!sMenu->readOnly)
    {
        const struct RogueTrialDefinition *trial = MenuTrial();
        if (trial != NULL && (row == ROW_MODE || (row == ROW_FORMAT && trial->hasForcedBattleFormat)
         || (row == ROW_TEAM && (trial->forceRandomStarter || trial->fixedStartingPartyCount))))
        { sMenu->message = sTrialRestriction; Draw(); return; }
        if (!dir) dir = 1;
        switch (row)
        {
        case ROW_MODE:
        {
            sMenu->config.mode = sMenu->config.mode != ROGUE_GAME_MODE_GAUNTLET
                && Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_GAUNTLET)
                ? ROGUE_GAME_MODE_GAUNTLET : ROGUE_GAME_MODE_STANDARD;
            break;
        }
        case ROW_FORMAT:
            sMenu->remembered[sMenu->config.battleFormat] = sMenu->config.ascension;
            sMenu->config.battleFormat = Cycle(sMenu->config.battleFormat, 3, dir);
            sMenu->config.ascension = min(sMenu->remembered[sMenu->config.battleFormat], RogueAscension_GetUnlocked(sMenu->config.battleFormat)); break;
        case ROW_ENCOUNTERS: sMenu->config.overworldMons = !sMenu->config.overworldMons; break;
        case ROW_TEAM:
            if (sMenu->entrance && !RogueRunStart_GetContext()->canUseStarterBag)
                sMenu->message = sLockedTeam;
            else sMenu->config.startingTeam = !sMenu->config.startingTeam;
            break;
        }
    }
    Draw();
}

static void MainCB(void) { RunTasks(); AnimateSprites(); BuildOamBuffer(); UpdatePaletteFade(); }
static void VBlankCB(void) { LoadOam(); ProcessSpriteCopyRequests(); TransferPlttBuffer(); }

void Rogue_OpenAdventureConfigMenu(RogueAdventureMenuCallback callback)
{
    gMain.savedCallback = callback;
    SetMainCallback2(CB2_InitAdventureConfigMenu);
    LockPlayerFieldControls();
}

static void InitAdventureConfigMenu(u8 mode)
{
    SetVBlankCallback(NULL);
    SetGpuReg(REG_OFFSET_DISPCNT, 0);
    DmaClearLarge16(3, (void *)VRAM, VRAM_SIZE, 0x1000);
    DmaClear32(3, OAM, OAM_SIZE);
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBg, ARRAY_COUNT(sBg));
    InitWindows(sWindows);
    DeactivateAllTextPrinters();
    ResetPaletteFade(); ResetTasks(); ResetSpriteData();
    sMenu = AllocZeroed(sizeof(*sMenu));
    if (sMenu == NULL)
    { FreeAllWindowBuffers(); gSpecialVar_Result = RUN_REVIEW_ACTION_BACK; SetMainCallback2(gMain.savedCallback); return; }
    sMenu->intro = mode == MENU_INTRO;
    sMenu->poolOnly = mode == MENU_CURATED_POOL || mode == MENU_TRIAL_POOL;
    sMenu->curatedPool = mode == MENU_CURATED_POOL;
    sMenu->poolTrialId = mode == MENU_TRIAL_POOL ? gSpecialVar_0x8004 : ROGUE_TRIAL_NONE;
    Rogue_CopyAdventureConfig(&sMenu->config);
    if (mode == MENU_TRIAL_POOL) sMenu->config.ascension = gSpecialVar_0x8005;
    memcpy(sMenu->remembered, gRogueSaveBlock->selectedAscension, sizeof(sMenu->remembered));
    sMenu->readOnly = !Rogue_CanEditConfig() || (RogueRunStart_GetContext() != NULL && !RogueRunStart_GetContext()->canEdit);
    sMenu->entrance = !sMenu->intro && !sMenu->poolOnly && RogueRunStart_GetContext() != NULL;
    if (sMenu->intro)
    {
        sMenu->readOnly = FALSE;
        RogueDexSelection_Init(&sMenu->pool, PoolAvailability(), sMenu->config.pokedexVariant,
            VarGet(VAR_ROGUE_INITIAL_GIMMICK_ITEM), TRUE);
        sMenu->config.pokedexVariant = sMenu->pool.variant;
        sMenu->startingGimmick = RogueDexSelection_GetGimmick(&sMenu->pool);
        sMenu->nationalGimmick = sMenu->pool.nationalGimmick;
    }
    BuildRows(PAGE_SETUP);
    if (sMenu->poolOnly && !BeginPool())
    {
        FreeAllWindowBuffers(); Free(sMenu); sMenu = NULL;
        gSpecialVar_Result = FALSE; SetMainCallback2(gMain.savedCallback); return;
    }
    LoadPalette(sPalette, 240, sizeof(sPalette));
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_BG0_ON);
    ShowBg(0);
    Draw();
    CreateTask(Task_Input, 0);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetVBlankCallback(VBlankCB);
    SetMainCallback2(MainCB);
}

void CB2_InitAdventureConfigMenu(void) { InitAdventureConfigMenu(MENU_ADVENTURE); }
void CB2_InitIntroAdventureConfigMenu(void) { InitAdventureConfigMenu(MENU_INTRO); }
void CB2_InitPokedexSelectionMenu(void)
{
    InitAdventureConfigMenu(gSpecialVar_0x8009 == 1 ? MENU_TRIAL_POOL : MENU_CURATED_POOL);
}
