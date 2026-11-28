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
static const u8 sAscensionText31[] = _("Use your party or choose a starter.");
static const u8 sAscensionText32[] = _("Read the fixed adventure rules.");
static const u8 sAscensionText33[] = _("Confirm these adventure settings.");
static const u8 sAscensionText34[] = _("A");
static const u8 sAscensionText35[] = _("Gauntlet");
static const u8 sAscensionText36[] = _("Slow Path");
static const u8 sAscensionText37[] = _("Standard");
static const u8 sAscensionText38[] = _("Select Pokédex");
static const u8 sAscensionText39[] = _("Visible");
static const u8 sAscensionText40[] = _("Traditional");
static const u8 sAscensionText41[] = _("Choose a Starter");
static const u8 sAscensionText42[] = _("Current party");
static const u8 sAscensionText43[] = _("ASCENSION ");
static const u8 sAscensionText44[] = _(" - LOCKED");
static const u8 sAscensionText45[] = _("Unlocked: A");
static const u8 sAscensionText46[] = _("Left/Right: level  L/R: page  A: OK  B: back");
static const u8 sAscensionText47[] = _("ADVENTURE RULES");
static const u8 sAscensionText48[] = _("TRAINERS");
static const u8 sAscensionText49[] = _("Set Up Adventure");
static const u8 sAscensionText50[] = _("Order");
static const u8 sAscensionText51[] = _("On");
static const u8 sAscensionText52[] = _("Off");
static const u8 sAscensionText53[] = _("Begin Adventure");
static const u8 sAscensionText54[] = _("Close");
static const u8 sAscensionText55[] = _("Left/Right: change  A: OK  B: back");
static const u8 sConfirmControls[] = _("{DPAD_LEFTRIGHT} Choose    {A_BUTTON} OK    {B_BUTTON} Back");
static const u8 sAscensionText56[] = _("Select at least one trainer region.");
static const u8 sAscensionText57[] = _("Check starting team and requirements.");
static const u8 sAscensionText58[] = _("Clear the preceding level to unlock.");
static const u8 sAscensionText59[] = _("Become Champion to change Pokédex.");
static const u8 sNone[] = _("None");
static const u8 sChooseTrial[] = _("Trial");
static const u8 sChooseTrialHelp[] = _("Choose a Trial, or None for a normal run.");
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
#include "strings.h"
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
enum { PAGE_SETUP, PAGE_ASCENSION, PAGE_TRAINERS, PAGE_POOL, PAGE_INTRO_CONFIRM };
enum { MENU_ADVENTURE, MENU_INTRO, MENU_CURATED_POOL, MENU_TRIAL_POOL };
enum { ROW_ASCENSION, ROW_MODE, ROW_FORMAT, ROW_DEX, ROW_TRAINERS, ROW_ENCOUNTERS,
       ROW_TEAM, ROW_RULES, ROW_DONE, ROW_TRIAL, ROW_COUNT };
// Keep the previous frame's primitives so shared menu pages can describe their
// layout normally without painting or uploading unchanged regions.
#define MENU_DRAW_COMMANDS 64
#define MENU_TEXT_BYTES 1536
struct MenuDrawCommand
{
    u8 x, y, w, h, ink, font;
    u16 textOffset; // zero for a filled rectangle
};

struct MenuDrawing
{
    struct MenuDrawCommand commands[MENU_DRAW_COMMANDS];
    u8 text[MENU_TEXT_BYTES];
    u16 used;
    u8 count;
};

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
    bool8 introConfirmed;
    bool8 introCommitted;
    u8 confirmChoice;
    const u8 *message;
    u8 drawingIndex;
    bool8 hasDrawing;
    struct MenuDrawing drawings[];
};
static EWRAM_DATA struct AdventureMenu *sMenu;
#if TESTING
static bool8 sTestSkipDraw;
#endif
static const struct BgTemplate sBg[] = {{ .bg = 0, .charBaseIndex = 0, .mapBaseIndex = 31, .screenSize = 0, .paletteMode = 0 }};
static const struct WindowTemplate sWindows[] = {
    {.bg = 0, .tilemapLeft = 0, .tilemapTop = 0, .width = 30, .height = 20, .paletteNum = 15, .baseBlock = 1},
    DUMMY_WIN_TEMPLATE
};
enum { INK_PAPER = 1, INK_EDGE, INK_MUTED, INK_RULE, INK_BG, INK_PANEL, INK_GOLD, INK_BLUE, INK_SELECTED };
static const u16 sPalette[16] = {
    RGB_BLACK, RGB(30,30,29), RGB(2,3,3), RGB(23,24,25), RGB(10,11,12),
    RGB(4,5,5), RGB(6,7,8), RGB(30,24,14), RGB(21,25,28), RGB(9,8,7)
};
static const u8 sGameSetup[] = _("Game Setup");
static const u8 sSelectCue[] = _("{DPAD_UPDOWN} Select");
static const u8 sChangeCue[] = _("{DPAD_LEFTRIGHT} Change");
static const u8 sOkCue[] = _("{A_BUTTON} OK");
static const u8 sBackCue[] = _("{B_BUTTON} Back");
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
static const u8 sBasePage[] = _("Base Rules");
static const u8 sFixedRules[] = _("These settings cannot be changed.");
static const u8 sCumulativePage[] = _("ACTIVE RULES");
static const u8 sTrialTeam[] = _("Trial team");
static const u8 sModeBeforeAscension[] = _("Choose your adventure style.");
static const u8 sFormatBeforeAscension[] = _("Choose Singles, Doubles or Mixed.");
static const u8 sBaseControls[] = _("{L_BUTTON}{R_BUTTON} Page");
static const u8 sPoolTitle[] = _("Pokédex");
static const u8 sPoolType[] = _("Pokédex type");
static const u8 sPoolRegion[] = _("Region");
static const u8 sPoolGame[] = _("Game");
static const u8 sPoolRoster[] = _("Roster");
static const u8 sPoolGeneration[] = _("Through gen.");
static const u8 sGeneration[] = _("Generation ");
static const u8 sPoolGimmick[] = _("Starting gimmick");
static const u8 sUsePool[] = _("Use this Pokédex");
static const u8 sPoolCount[] = _(" Pokémon");
static const u8 sPoolStart[] = _("Start: ");
static const u8 sContinue[] = _("Continue");
static const u8 sIntroHelp[] = _("Choose Continue when you're ready.");
static const u8 sEditDexCue[] = _("{A_BUTTON} Change");
static const u8 sViewCue[] = _("{A_BUTTON} View");
static const u8 sIntroPoolHelp[] = _("Can't change until you become Champion.");
static const u8 sPoolContextLocked[] = _("Change this in your own hub.");
static const u8 sIntroConfirmTitle[] = _("Your Starting Pokédex");
static const u8 sIntroConfirmQuestion[] = _("Begin with this Pokédex?");
static const u8 sIntroConfirmEffect[] = _("Sets encounters and first partners.");
static const u8 sIntroConfirmLock[] = _("You can't change it until\nyou become Champion.");
static const u8 sGoBack[] = _("Go back");
static const u8 sPoolUnavailable[] = _("No compatible Pokédexes.");
static const u8 sLockedTeam[] = _("Starter selection is unavailable here.");

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
        || (sMenu->config.ascension < ASCENSION_FRESH_START && !sMenu->config.trialFreshStart && AnyCharmsInBag());
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
        return !sMenu->readOnly && sMenu->entrance && RogueRunStart_CanChooseTrial();
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
    if (row == ROW_DEX && IsPoolEditingLocked()) return RoguePokedex_IsVariantEditUnlocked() ? sPoolContextLocked : sAscensionText59;
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
        sMenu->message = RoguePokedex_IsVariantEditUnlocked() ? sPoolContextLocked : sAscensionText59;
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
    if (page == PAGE_SETUP && (sMenu->entrance || sMenu->intro))
    {
        sMenu->rows[sMenu->rowCount++] = ROW_DONE;
        if (sMenu->entrance && SetupRowAvailable(ROW_TRIAL))
            sMenu->rows[sMenu->rowCount++] = ROW_TRIAL;
    }
    for (row = 0; row < (page == PAGE_SETUP ? ROW_COUNT : ARRAY_COUNT(sRegions) + 1); ++row)
    {
        bool8 available = page == PAGE_SETUP ? SetupRowAvailable(row)
            : sMenu->readOnly || trial == NULL || (row == 0 ? !trial->hasForcedTrainerOrder
                : trial->forcedTrainerToggle == ROGUE_TRIAL_NO_TRAINER_TOGGLE && !trial->enableAllRegionalTrainers);
        if (available && !(page == PAGE_SETUP && (sMenu->entrance || sMenu->intro) && (row == ROW_DONE || row == ROW_TRIAL)))
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

static void SetupText(u8 x, u8 y, const u8 *str, u8 ink, u8 font)
{
    struct MenuDrawing *d = &sMenu->drawings[sMenu->drawingIndex];
    u16 length = StringLength(str) + 1;
    struct MenuDrawCommand *cmd;
    if (length == 1) return;
    AGB_ASSERT(d->count < MENU_DRAW_COMMANDS && d->used + length <= MENU_TEXT_BYTES);
    if (d->count >= MENU_DRAW_COMMANDS || d->used + length > MENU_TEXT_BYTES) return;
    cmd = &d->commands[d->count++];
    *cmd = (struct MenuDrawCommand){x, y, 0, 16, ink, font, d->used + 1};
    memcpy(d->text + d->used, str, length);
    d->used += length;
}

static void Print(u8 x, u8 y, const u8 *str, bool8 selected)
{
    SetupText(x + 8, y + 8, str, selected ? INK_GOLD : INK_PAPER, FONT_SMALL_NARROW);
}

static void SetupRect(u8 x, u8 y, u8 w, u8 h, u8 ink)
{
    struct MenuDrawing *d = &sMenu->drawings[sMenu->drawingIndex];
    AGB_ASSERT(d->count < MENU_DRAW_COMMANDS);
    if (d->count < MENU_DRAW_COMMANDS)
        d->commands[d->count++] = (struct MenuDrawCommand){x, y, w, h, ink, 0, 0};

}

static void SetupPanel(void)
{
    SetupRect(3, 20, 234, 108, INK_EDGE);
    SetupRect(4, 21, 232, 1, INK_RULE);
    SetupRect(4, 22, 232, 105, INK_PANEL);
    SetupRect(5, 145, 230, 1, INK_RULE);
}

static void SelectedRow(u8 x, u8 y, u8 w, u8 h)
{
    SetupRect(x, y, w, h, INK_SELECTED);
    SetupRect(x, y, 1, h, INK_GOLD);
    SetupText(x + 3, y, gText_SelectorArrow, INK_GOLD, FONT_SMALL);
}

static void PrintSelectable(u8 x, u8 y, const u8 *str, bool8 selected)
{
    if (selected)
        Print(x, y, gText_SelectorArrow, TRUE);
    Print(x + 10, y, str, selected);
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
    case ROW_TRIAL:
        StringCopy(text, MenuTrial() != NULL ? MenuTrial()->name : sNone);
        break;
    case ROW_TEAM:
        if (MenuTrial() != NULL && MenuTrial()->fixedStartingPartyCount) StringCopy(text, sTrialTeam);
        else if (MenuTrial() != NULL && MenuTrial()->forceRandomStarter) StringCopy(text, sAscensionText41);
        else StringCopy(text, sMenu->config.startingTeam == RUN_START_TEAM_SOURCE_STARTER_BAG ? sAscensionText41 : sAscensionText42);
        break;
    default: text[0] = EOS; break;
    }
}

static void DrawPoolRow(u8 i)
{
    u8 text[160];
    u8 type = RogueDexSelection_GetType(sMenu->pool.variant);
    u8 y = 24 + 16 * i;
    u8 font;
    bool8 selected = i == sMenu->row;
    const u8 *label = sUsePool;
    u8 field = sMenu->rows[i];
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
    if (selected)
        SelectedRow(6, y, 228, 16);
    else
        SetupRect(6, y, 228, 16, INK_PANEL);
    SetupText(18, y, label, selected ? INK_GOLD : INK_PAPER, FONT_SMALL);
    font = GetFontIdToFit(text, FONT_SMALL, 0, 206 - GetStringWidth(FONT_SMALL, label, 0));
    SetupText(229 - GetStringWidth(font, text, 0), y, text, selected ? INK_GOLD : INK_MUTED, font);
}

static void DrawPoolControls(void)
{
    if (!sMenu->readOnly && sMenu->rows[sMenu->row] != DEX_FIELD_DONE)
        SetupText(65, 146, sChangeCue, INK_PAPER, FONT_SMALL);
    SetupText(151, 146, sOkCue, INK_PAPER, FONT_SMALL);
    SetupText(194, 146, sBackCue, INK_MUTED, FONT_SMALL);
}

static void DrawPool(void)
{
    u8 i, text[160];
    const u8 *name = gPokedexVariants[sMenu->pool.variant].displayName;
    u8 font = GetFontIdToFit(name, FONT_SMALL, 0, 164);
    SetupPanel();
    SetupText(7, 0, sPoolTitle, INK_PAPER, FONT_NORMAL);
    SetupText(232 - GetStringWidth(font, name, 0), 1, name, INK_MUTED, font);
    for (i = 0; i < sMenu->rowCount; ++i)
        DrawPoolRow(i);
    SetupRect(10, 106, 220, 1, INK_RULE);
    StringCopy(ConvertIntToDecimalStringN(text, gPokedexVariants[sMenu->pool.variant].speciesCount,
        STR_CONV_MODE_LEFT_ALIGN, 4), sPoolCount);
    SetupText(12, 110, text, INK_BLUE, FONT_SMALL);
    if (sMenu->intro)
    {
        StringCopy(StringCopy(text, sPoolStart), RogueDexSelection_GetGimmickName(RogueDexSelection_GetGimmick(&sMenu->pool)));
        SetupText(94, 110, text, INK_BLUE, GetFontIdToFit(text, FONT_SMALL, 0, 137));
    }
    RogueDexSelection_Describe(&sMenu->pool, text);
    name = sMenu->message != NULL ? sMenu->message : text;
    SetupText(8, 128, name, INK_MUTED, GetFontIdToFit(name, FONT_SMALL, 0, 224));
    SetupText(8, 146, sSelectCue, INK_MUTED, FONT_SMALL);
    DrawPoolControls();
}

static u8 BaseRulePageCount(void)
{
    u8 text[160];
    return RogueAscension_GetBaseLine(6, text) ? 2 : 1;
}

static u8 SetupRowHeight(void)
{
    return sMenu->rowCount > 5 ? 14 : 17;
}

static void DrawSetupRow(u8 visibleIndex)
{
    u8 index = sMenu->top + visibleIndex;
    u8 row = sMenu->rows[index];
    bool8 selected = index == sMenu->row;
    u8 height = SetupRowHeight();
    u8 text[160];
    const u8 *label = LabelForRow(row);
    u8 y = 23 + visibleIndex * height;
    u8 valueFont;
    if (selected)
        SelectedRow(6, y, 228, height);
    else
        SetupRect(6, y, 228, height, INK_PANEL);
    SetupText(18, y, label, selected ? INK_GOLD : INK_PAPER, FONT_SMALL);
    ValueForRow(row, text);
    if (row == ROW_RULES)
        StringCopy(text, gText_SelectorArrow);
    valueFont = GetFontIdToFit(text, FONT_SMALL, 0, 206 - GetStringWidth(FONT_SMALL, label, 0));
    SetupText(229 - GetStringWidth(valueFont, text, 0), y, text,
        selected ? INK_GOLD : INK_MUTED, valueFont);
}

static void DrawSetupFooter(void)
{
    u8 text[160];
    u8 row = sMenu->rows[sMenu->row];
    const u8 *help = sMenu->message != NULL ? sMenu->message : DescriptionForRow(row);
    if (RogueAscension_IsRevealed() && MenuTrial() != NULL && sMenu->message == NULL)
    {
        ConvertIntToDecimalStringN(StringCopy(text, sTrialMinimum), MenuTrial()->minimumAscension, STR_CONV_MODE_LEFT_ALIGN, 2);
        help = text;
    }
    SetupText(8, 128, help, INK_MUTED, GetFontIdToFit(help, FONT_SMALL, 0, 224));
    SetupText(8, 146, sSelectCue, INK_MUTED, FONT_SMALL);
    if (row == ROW_DEX && !IsPoolEditingLocked())
        SetupText(82, 146, sMenu->readOnly ? sViewCue : sEditDexCue, INK_PAPER, FONT_SMALL);
    else if (row == ROW_DONE || row == ROW_RULES || row == ROW_ASCENSION || row == ROW_TRAINERS || row == ROW_TRIAL)
        SetupText(82, 146, sOkCue, INK_PAPER, FONT_SMALL);
    else if (!sMenu->readOnly && row != ROW_DEX)
        SetupText(82, 146, sChangeCue, INK_PAPER, FONT_SMALL);
    if (!sMenu->intro)
        SetupText(230 - GetStringWidth(FONT_SMALL, sBackCue, 0), 146, sBackCue, INK_MUTED, FONT_SMALL);
}

// One bit per eight-pixel tile row. Copy contiguous runs in a single DMA request.
static u32 TileRows(const struct MenuDrawCommand *c, const struct MenuDrawing *d)
{
    u32 bottom = c->y + c->h;
    if (c->textOffset)
    {
        const u8 *p = d->text + c->textOffset - 1;
        while (*p != EOS)
            if (*p++ == CHAR_NEWLINE) bottom += 16;
    }
    return ((1u << ((min(bottom, DISPLAY_HEIGHT) + 7) / 8)) - 1)
        & ~((1u << (c->y / 8)) - 1);
}

static bool8 SameCommand(const struct MenuDrawCommand *a, const struct MenuDrawing *ad,
                         const struct MenuDrawCommand *b, const struct MenuDrawing *bd)
{
    if (a->x != b->x || a->y != b->y || a->w != b->w || a->h != b->h
        || a->ink != b->ink || a->font != b->font || !!a->textOffset != !!b->textOffset)
        return FALSE;
    return !a->textOffset || StringCompare(ad->text + a->textOffset - 1, bd->text + b->textOffset - 1) == 0;
}

static void PaintDrawing(bool8 full)
{
    const struct MenuDrawing *now = &sMenu->drawings[sMenu->drawingIndex], *old = &sMenu->drawings[sMenu->drawingIndex ^ 1];
    u32 dirty = 0;
    u8 i, j;
    u8 prefix = 0, end = now->count, oldEnd = old->count;
    if (full)
        dirty = (1u << 20) - 1;
    else
    {
        while (prefix < end && prefix < oldEnd
            && SameCommand(&now->commands[prefix], now, &old->commands[prefix], old)) ++prefix;
        while (end > prefix && oldEnd > prefix
            && SameCommand(&now->commands[end-1], now, &old->commands[oldEnd-1], old))
        {
            --end;
            --oldEnd;
        }
        for (i = prefix; i < max(end, oldEnd); ++i)
        {
            if (end == oldEnd && SameCommand(&now->commands[i], now, &old->commands[i], old)) continue;
            if (i < end) dirty |= TileRows(&now->commands[i], now);
            if (i < oldEnd) dirty |= TileRows(&old->commands[i], old);
        }
    }
    if (!dirty) return;
    if (full)
        FillWindowPixelBuffer(0, PIXEL_FILL(INK_BG));
    else
        for (i = 0; i < 20; ++i)
            if (dirty & (1u << i))
                CpuFastFill(PIXEL_FILL(INK_BG) * 0x01010101u, gWindows[0].tileData + i * 30 * 32, 30 * 32);

    for (i = 0; i < now->count; ++i)
    {
        const struct MenuDrawCommand *c = &now->commands[i];
        u32 rows = TileRows(c, now);
        if (!(rows & dirty)) continue;
        if (c->textOffset)
        {
            const u8 colors[] = {TEXT_COLOR_TRANSPARENT, c->ink, INK_BG};
            AddTextPrinterParameterized4(0, c->font, c->x, c->y, 0, 0, colors, TEXT_SKIP_DRAW, now->text + c->textOffset - 1);
            // A glyph may extend across a band boundary. Replay subsequent layers there too.
            dirty |= rows;
        }
        else
        {
            for (j = c->y / 8; j * 8 < c->y + c->h;)
            {
                u8 start = j++;
                if (!(dirty & (1u << start))) continue;
                while (j * 8 < c->y + c->h && (dirty & (1u << j))) ++j;
                u8 y = max(c->y, start * 8);
                FillWindowPixelRectFast(0, c->ink, c->x, y, c->w, min(c->y + c->h, j * 8) - y);
            }
        }
    }
    if (full)
    {
        PutWindowTilemap(0);
        CopyWindowToVram(0, COPYWIN_FULL);
    }
    else
        for (i = 0; i < 20;)
        {
            u8 start = i++;
            if (!(dirty & (1u << start))) continue;
            while (i < 20 && (dirty & (1u << i))) ++i;
            CopyWindowRectToVram(0, COPYWIN_GFX, 0, start, 30, i - start);
        }
}

static void Draw(void)
{
    u8 i;
    u8 text[160];
    struct RogueAdventureConfig preview = sMenu->config;
#if TESTING
    if (sTestSkipDraw) return;
#endif
    preview.ascension = sMenu->candidate;
    sMenu->drawingIndex ^= 1;
    sMenu->drawings[sMenu->drawingIndex].count = 0;
    sMenu->drawings[sMenu->drawingIndex].used = 0;
    if (sMenu->page == PAGE_INTRO_CONFIRM)
    {
        const u8 *name = gPokedexVariants[sMenu->config.pokedexVariant].displayName;
        SetupPanel();
        SetupText(7, 0, sIntroConfirmTitle, INK_PAPER, FONT_NORMAL);
        SetupText(12, 24, name, INK_GOLD, GetFontIdToFit(name, FONT_NORMAL, 0, 216));
        SetupRect(12, 44, 216, 1, INK_RULE);
        SetupText(12, 45, sIntroConfirmQuestion, INK_PAPER, FONT_SMALL);
        SetupText(12, 60, sIntroConfirmEffect, INK_MUTED, FONT_SMALL);
        SetupText(12, 75, sIntroConfirmLock, INK_MUTED, FONT_SMALL);
        for (i = 0; i < 2; ++i)
        {
            u8 x = 10 + 112 * i;
            SetupRect(x, 106, 105, 18, INK_RULE);
            SetupRect(x + 1, 107, 103, 16, INK_PANEL);
            if (i == sMenu->confirmChoice)
                SelectedRow(x + 1, 107, 103, 16);
            SetupText(x + 16, 107, i ? sGoBack : sContinue,
                i == sMenu->confirmChoice ? INK_GOLD : INK_PAPER, FONT_SMALL);
        }
        if (sMenu->message != NULL)
            SetupText(8, 128, sMenu->message, INK_PAPER, FONT_SMALL);
        SetupText(8, 146, sConfirmControls, INK_MUTED, FONT_SMALL);
    }
    else if (sMenu->page == PAGE_POOL)
        DrawPool();
    else if (sMenu->page == PAGE_ASCENSION)
    {
        bool8 baseOnly = sMenu->rows[sMenu->row] == ROW_RULES;
        u8 *end = ConvertIntToDecimalStringN(StringCopy(text, sAscensionText43), sMenu->candidate, STR_CONV_MODE_LEFT_ALIGN, 2);
        if (!RogueAscension_IsUnlocked(sMenu->candidate, sMenu->config.battleFormat)) StringCopy(end, sAscensionText44);
        if (baseOnly)
        {
            SetupPanel();
            SetupText(7, 0, sBasePage, INK_PAPER, FONT_NORMAL);
            SetupText(12, 23, sFixedRules, INK_MUTED, FONT_SMALL);
            SetupRect(11, 41, 218, 1, INK_RULE);
        }
        else
            PrintSelectable(0, 0, text, TRUE);
        ConvertIntToDecimalStringN(StringCopy(text, sAscensionText45), RogueAscension_GetUnlocked(sMenu->config.battleFormat), STR_CONV_MODE_LEFT_ALIGN, 2);
        if (!baseOnly) Print(120, 13, text, FALSE);
        if (!baseOnly) Print(0, 13, sMenu->rulesPage >= 2 ? sBasePage : sCumulativePage, FALSE);
        for (i = 0; i < 6; ++i)
        {
            bool8 hasLine = sMenu->rulesPage < 2
                ? RogueAscension_GetConfigRuleLine(&preview, sMenu->rulesPage * 6 + i, text)
                : RogueAscension_GetBaseLine((sMenu->rulesPage - 2) * 6 + i, text);
            if (hasLine && baseOnly)
                SetupText(12, 42 + i * 14, text, INK_PAPER, GetFontIdToFit(text, FONT_SMALL, 0, 216));
            else if (hasLine) Print(0, 29 + i * 12, text, FALSE);
        }
        if (!baseOnly) Print(0, 103, RogueAscension_GetAddition(sMenu->candidate), TRUE);
        if (baseOnly)
        {
            if (sMenu->message != NULL)
                SetupText(8, 128, sMenu->message, INK_MUTED, FONT_SMALL);
            if (BaseRulePageCount() > 1)
                SetupText(8, 146, sBaseControls, INK_PAPER, FONT_SMALL);
            SetupText(194, 146, sBackCue, INK_MUTED, FONT_SMALL);
        }
        else Print(0, 132, sMenu->message != NULL ? sMenu->message : sAscensionText46, FALSE);
    }
    else
    {
        u8 count = sMenu->rowCount;
        if (sMenu->page == PAGE_SETUP)
        {
            SetupPanel();
            SetupText(7, 0, sMenu->intro ? sGameSetup : sMenu->readOnly ? sAscensionText47 : sAscensionText49, INK_PAPER, FONT_NORMAL);
            if (MenuTrial() != NULL)
            {
                const u8 *name = MenuTrial()->name;
                u8 font = GetFontIdToFit(name, FONT_SMALL, 0, 118);
                SetupText(232 - GetStringWidth(font, name, 0), 1, name, INK_MUTED, font);
            }
        }
        else
            Print(0, 0, sAscensionText48, TRUE);
        if (sMenu->row < sMenu->top) sMenu->top = sMenu->row;
        if (sMenu->row >= sMenu->top + 6) sMenu->top = sMenu->row - 5;
        for (i = 0; i < 6 && i + sMenu->top < count; ++i)
        {
            u8 row = sMenu->rows[i + sMenu->top];
            bool8 selected = i + sMenu->top == sMenu->row;
            if (sMenu->page == PAGE_TRAINERS)
            {
                PrintSelectable(0, 22 + 14*i, row == 0 ? sAscensionText50 : sRegions[row-1], selected);
                Print(112, 22 + 14*i, row == 0 ? sOrders[sMenu->config.trainerOrder] : (sMenu->config.trainerRegions & (1 << (row-1))) ? sAscensionText51 : sAscensionText52, selected);
            }
            else
            {
                DrawSetupRow(i);
            }
        }
        if (sMenu->intro && sMenu->page == PAGE_SETUP)
        {
            StringCopy(StringCopy(text, sPoolStart), RogueDexSelection_GetGimmickName(sMenu->startingGimmick));
            SetupRect(10, 108, 220, 1, INK_RULE);
            SetupText(12, 110, text, INK_BLUE, FONT_SMALL);
        }
        else if (sMenu->page == PAGE_SETUP && RogueAscension_IsRevealed())
            SetupText(12, 110, TrialRecordsDisabled() ? sTrialDisabled : RogueAscension_EligibilityText(Rogue_IsRunActive() ? gRogueSaveBlock->ascensionEligibilityReason : RogueAscension_GetEligibility(&sMenu->config, RogueRunStart_GetContext() != NULL ? RogueRunStart_GetContext()->source : RUN_START_SOURCE_NORMAL)), INK_MUTED, FONT_SMALL_NARROW);
        if (sMenu->page == PAGE_TRAINERS)
            Print(0, 123, sMenu->message != NULL ? sMenu->message : sAscensionText55, FALSE);
        else
        {
            DrawSetupFooter();
        }
    }
    PaintDrawing(!sMenu->hasDrawing);
    sMenu->hasDrawing = TRUE;
}

static void CloseMenu(u16 result)
{
    gSpecialVar_Result = result;
    sMenu->closing = TRUE;
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
}

static bool8 RequestIntroConfirmation(void)
{
    if (!sMenu->intro || sMenu->introConfirmed)
        return FALSE;
    sMenu->page = PAGE_INTRO_CONFIRM;
    sMenu->confirmChoice = 1;
    sMenu->message = NULL;
    return TRUE;
}

static bool8 CommitIntroSelection(void)
{
    if (!sMenu->introConfirmed || sMenu->introCommitted
        || !RoguePokedex_IsCuratedVariant(sMenu->config.pokedexVariant))
        return FALSE;
    // Validate and store the initial choice before applying the remaining draft.
    gSpecialVar_0x8006 = sMenu->config.pokedexVariant;
    gSpecialVar_0x8005 = sMenu->startingGimmick;
    RoguePokedex_StoreInitialSelection();
    if (!gSpecialVar_Result)
        return FALSE;
    Rogue_ApplyAdventureConfig(&sMenu->config);
    // Keep the existing initial trainer-pool selection semantics.
    RoguePokedex_ApplyTrainerPoolForVariant(sMenu->config.pokedexVariant);
    sMenu->introCommitted = TRUE;
    return TRUE;
}

static void ExitMenu(u8 taskId, bool8 accept, bool8 start)
{
    if (sMenu->closing) return;
    if (sMenu->intro && accept)
    {
        if (RequestIntroConfirmation()) { Draw(); return; }
        if (!CommitIntroSelection()) { sMenu->message = sPoolUnavailable; Draw(); return; }
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

static void ResolveIntroConfirmation(bool8 accept)
{
    sMenu->introConfirmed = accept;
    if (!accept) ReturnToSetup(ROW_DONE);
}

#if TESTING
bool8 RogueTest_IntroConfirmationTextFits(void)
{
    const u8 *const lines[] = { sIntroConfirmTitle, sIntroConfirmQuestion,
        sIntroConfirmEffect, sIntroConfirmLock, sIntroPoolHelp, sPoolContextLocked,
        sAscensionText55, sConfirmControls, sAscensionText46 };
    u8 i;
    for (i = 0; i < ARRAY_COUNT(lines); ++i)
        if (GetStringWidth(FONT_SMALL_NARROW, lines[i], 0) > 224) return FALSE;
    return GetStringWidth(FONT_SMALL_NARROW, sContinue, 0) <= 112
        && GetStringWidth(FONT_SMALL_NARROW, sGoBack, 0) <= 112;
}

bool8 RogueTest_ExerciseIntroConfirmation(const struct RogueAdventureConfig *draft, u16 gimmick,
    bool8 accept, bool8 *defaultBack, bool8 *unchanged, bool8 *duplicateRejected)
{
    struct AdventureMenu menu = {0};
    struct AdventureMenu *previous = sMenu;
    bool8 result;
    menu.intro = TRUE;
    menu.config = *draft;
    menu.startingGimmick = gimmick;
    sMenu = &menu;
    RequestIntroConfirmation();
    *defaultBack = menu.page == PAGE_INTRO_CONFIRM && menu.confirmChoice == 1;
    ResolveIntroConfirmation(FALSE);
    *unchanged = memcmp(&menu.config, draft, sizeof(*draft)) == 0 && menu.startingGimmick == gimmick;
    RequestIntroConfirmation();
    ResolveIntroConfirmation(accept);
    result = CommitIntroSelection();
    *duplicateRejected = !CommitIntroSelection();
    sMenu = previous;
    return result;
}
#endif

static void HandlePoolInput(u8 taskId, s8 direction)
{
    u8 field = sMenu->rows[sMenu->row];
    if (JOY_NEW(B_BUTTON))
    {
        if (sMenu->poolOnly) ExitMenu(taskId, FALSE, FALSE);
        else ReturnToSetup(ROW_DEX);
    }
    else if (JOY_REPEAT(DPAD_UP | DPAD_DOWN))
    {
        sMenu->row = Cycle(sMenu->row, sMenu->rowCount, JOY_REPEAT(DPAD_UP) ? -1 : 1);
    }
    else if (JOY_NEW(A_BUTTON))
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
    else if (!sMenu->readOnly && direction && field != DEX_FIELD_DONE)
    {
        RogueDexSelection_Cycle(&sMenu->pool, field, direction);
        BuildPoolRows();
        while (sMenu->row + 1 < sMenu->rowCount && sMenu->rows[sMenu->row] != field) ++sMenu->row;
    }
    else return;
    Draw();
}

static void HandleSetupAction(u8 taskId, u8 row)
{
    switch (row)
    {
    case ROW_ASCENSION:
    case ROW_RULES:
        sMenu->page = PAGE_ASCENSION;
        sMenu->candidate = sMenu->config.ascension;
        sMenu->rulesPage = row == ROW_RULES ? 2 : 0;
        break;
    case ROW_DEX:
        BeginPool();
        break;
    case ROW_TRAINERS:
        BuildRows(PAGE_TRAINERS);
        break;
    case ROW_TRIAL:
        if (!RogueRunStart_CanChooseTrial()) return;
        if (!TrialAllowsConfig(&sMenu->config) || sMenu->config.trainerRegions == 0)
        {
            sMenu->message = sTrialRestriction;
            break;
        }
        RogueRunStart_UpdateConfig(&sMenu->config);
        if (MenuTrial() == NULL)
        {
            Rogue_ApplyAdventureConfig(&sMenu->config);
            memcpy(gRogueSaveBlock->selectedAscension, sMenu->remembered, sizeof(sMenu->remembered));
            gRogueSaveBlock->selectedAscension[sMenu->config.battleFormat] = sMenu->config.ascension;
        }
        CloseMenu(RUN_REVIEW_ACTION_CHOOSE_TRIAL);
        return;
    case ROW_DONE:
        ExitMenu(taskId, TRUE, sMenu->entrance);
        return;
    default:
        // Inline values are already in the draft; A leaves them and the cursor alone.
        return;
    }
    Draw();
}

static void HandleSetupValue(u8 row, s8 direction)
{
    const struct RogueTrialDefinition *trial = MenuTrial();
    if (sMenu->readOnly) return;
    if (trial != NULL && (row == ROW_MODE || (row == ROW_FORMAT && trial->hasForcedBattleFormat)
     || (row == ROW_TEAM && (trial->forceRandomStarter || trial->fixedStartingPartyCount))))
    {
        sMenu->message = sTrialRestriction;
        Draw();
        return;
    }
    switch (row)
    {
    case ROW_MODE:
        sMenu->config.mode = sMenu->config.mode != ROGUE_GAME_MODE_GAUNTLET
            && Rogue_IsAdventureModeAvailable(ROGUE_GAME_MODE_GAUNTLET)
            ? ROGUE_GAME_MODE_GAUNTLET : ROGUE_GAME_MODE_STANDARD;
        break;
    case ROW_FORMAT:
        sMenu->remembered[sMenu->config.battleFormat] = sMenu->config.ascension;
        sMenu->config.battleFormat = Cycle(sMenu->config.battleFormat, 3, direction);
        sMenu->config.ascension = min(sMenu->remembered[sMenu->config.battleFormat], RogueAscension_GetUnlocked(sMenu->config.battleFormat));
        break;
    case ROW_ENCOUNTERS:
        sMenu->config.overworldMons = !sMenu->config.overworldMons;
        break;
    case ROW_TEAM:
        if (sMenu->entrance && !RogueRunStart_GetContext()->canUseStarterBag)
            sMenu->message = sLockedTeam;
        else
            sMenu->config.startingTeam = !sMenu->config.startingTeam;
        break;
    default:
        return;
    }
    Draw();
}

static void Task_Input(u8 taskId)
{
    u8 row = sMenu->rows[sMenu->row];
    s8 dir = JOY_NEW(A_BUTTON) ? 0 : JOY_REPEAT(DPAD_LEFT) ? -1 : JOY_REPEAT(DPAD_RIGHT) ? 1 : 0;
    if (gPaletteFade.active) return;
    if (sMenu->closing)
    {
        FreeAllWindowBuffers();
        Free(sMenu); sMenu = NULL;
        DestroyTask(taskId);
        SetMainCallback2(gMain.savedCallback);
        return;
    }
    if (sMenu->page == PAGE_INTRO_CONFIRM)
    {
        if (JOY_NEW(B_BUTTON)) ResolveIntroConfirmation(FALSE);
        else if (dir) sMenu->confirmChoice ^= 1;
        else if (JOY_NEW(A_BUTTON))
        {
            ResolveIntroConfirmation(sMenu->confirmChoice == 0);
            if (sMenu->introConfirmed) { ExitMenu(taskId, TRUE, FALSE); return; }
        }
        else return;
        Draw();
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
        else return;
        Draw(); return;
    }
    if (JOY_NEW(B_BUTTON))
    {
        if (sMenu->page == PAGE_TRAINERS)
        {
            ReturnToSetup(ROW_TRAINERS);
            Draw();
        }
        else if (sMenu->intro) { sMenu->message = sIntroHelp; Draw(); }
        else ExitMenu(taskId, FALSE, FALSE);
        return;
    }
    if (JOY_REPEAT(DPAD_UP | DPAD_DOWN))
    {
        u8 previous = sMenu->row;
        sMenu->row = Cycle(previous, sMenu->rowCount, JOY_REPEAT(DPAD_UP) ? -1 : 1);
        sMenu->message = NULL;
        Draw();
        return;
    }
    if (!dir && !JOY_NEW(A_BUTTON)) return;
    if (sMenu->page == PAGE_TRAINERS)
    {
        if (JOY_NEW(A_BUTTON))
        {
            ReturnToSetup(ROW_TRAINERS);
            Draw(); return;
        }
        if (!sMenu->readOnly)
        {
            const struct RogueTrialDefinition *trial = MenuTrial();
            if (trial != NULL && ((row == 0 && trial->hasForcedTrainerOrder)
             || (row != 0 && (trial->forcedTrainerToggle != ROGUE_TRIAL_NO_TRAINER_TOGGLE || trial->enableAllRegionalTrainers))))
            { sMenu->message = sTrialRestriction; Draw(); return; }
            if (row == 0) sMenu->config.trainerOrder = Cycle(sMenu->config.trainerOrder, 3, dir);
            else sMenu->config.trainerRegions ^= 1 << (row - 1);
        }
        Draw(); return;
    }
    if (JOY_NEW(A_BUTTON)) HandleSetupAction(taskId, row);
    else if (dir) HandleSetupValue(row, dir);
}

#if TESTING
static void TestPressMenuKey(u16 key)
{
    gMain.newKeys = key;
    gMain.newAndRepeatedKeys = key;
    Task_Input(0);
}

// Exercise the real input handlers, suppressing only graphics. Each scenario
// starts with a fresh draft and leaves saved settings and key state untouched.
bool8 RogueTest_AdventureControlScenario(u8 scenario)
{
    struct AdventureMenu menu = {0};
    struct AdventureMenu *previous = sMenu;
    struct RogueAdventureConfig before;
    u16 oldNew = gMain.newKeys, oldRepeat = gMain.newAndRepeatedKeys;
    bool8 oldFade = gPaletteFade.active;
    bool8 ok = TRUE;
    u8 i;
    const u8 values[] = { ROW_FORMAT, ROW_ENCOUNTERS, ROW_TEAM };
    const u8 submenus[] = { ROW_DEX, ROW_TRAINERS, ROW_RULES, ROW_ASCENSION };
    Rogue_CopyAdventureConfig(&menu.config);
    menu.intro = scenario != 7;
    menu.entrance = scenario == 7;
    menu.config.pokedexVariant = POKEDEX_VARIANT_KANTO_RBY;
    menu.config.ascension = 0;
    before = menu.config;
    sMenu = &menu;
    sTestSkipDraw = TRUE;
    gPaletteFade.active = FALSE;
    if (scenario == 0 || scenario == 7)
    {
        for (i = 0; i < (scenario == 7 ? 2 : ARRAY_COUNT(values)); ++i)
        {
            menu.page = PAGE_SETUP; menu.row = 0; menu.rowCount = 2;
            menu.rows[0] = values[i]; menu.rows[1] = ROW_DONE;
            TestPressMenuKey(A_BUTTON);
            ok &= memcmp(&before, &menu.config, sizeof(before)) == 0 && menu.row == 0;
            menu.row = 0;
            TestPressMenuKey(DPAD_RIGHT);
            ok &= memcmp(&before, &menu.config, sizeof(before)) != 0;
            TestPressMenuKey(DPAD_LEFT);
            ok &= memcmp(&before, &menu.config, sizeof(before)) == 0;
        }
    }
    else if (scenario == 1)
    {
        for (i = 0; i < 2; ++i)
        {
            menu.page = PAGE_TRAINERS; menu.row = 0; menu.rowCount = 1; menu.rows[0] = i;
            TestPressMenuKey(DPAD_RIGHT);
            ok &= memcmp(&before, &menu.config, sizeof(before)) != 0;
            TestPressMenuKey(DPAD_LEFT);
            ok &= memcmp(&before, &menu.config, sizeof(before)) == 0;
            TestPressMenuKey(A_BUTTON);
            ok &= menu.page == PAGE_SETUP && memcmp(&before, &menu.config, sizeof(before)) == 0;
        }
    }
    else if (scenario == 2)
    {
        u16 selected;
        ok &= BeginPool();
        TestPressMenuKey(DPAD_RIGHT);
        selected = menu.pool.variant;
        ok &= selected != before.pokedexVariant;
        TestPressMenuKey(B_BUTTON);
        ok &= menu.page == PAGE_SETUP && menu.config.pokedexVariant == before.pokedexVariant;
        ok &= BeginPool();
        TestPressMenuKey(DPAD_RIGHT);
        selected = menu.pool.variant;
        TestPressMenuKey(A_BUTTON);
        ok &= menu.page == PAGE_SETUP && menu.config.pokedexVariant == selected;
    }
    else if (scenario == 3)
    {
        for (i = 0; i < ARRAY_COUNT(submenus); ++i)
        {
            menu.page = PAGE_SETUP; menu.row = 0; menu.rowCount = 1; menu.rows[0] = submenus[i];
            TestPressMenuKey(DPAD_RIGHT);
            ok &= menu.page == PAGE_SETUP;
            TestPressMenuKey(A_BUTTON);
            ok &= menu.page != PAGE_SETUP;
            TestPressMenuKey(B_BUTTON);
            ok &= menu.page == PAGE_SETUP && memcmp(&before, &menu.config, sizeof(before)) == 0;
        }
    }
    else if (scenario == 4)
    {
        menu.readOnly = TRUE;
        menu.page = PAGE_SETUP; menu.rowCount = 1; menu.rows[0] = ROW_FORMAT;
        TestPressMenuKey(DPAD_RIGHT);
        TestPressMenuKey(A_BUTTON);
        ok &= memcmp(&before, &menu.config, sizeof(before)) == 0;
        ok &= BeginPool();
        TestPressMenuKey(DPAD_RIGHT);
        TestPressMenuKey(A_BUTTON);
        ok &= menu.page == PAGE_SETUP && memcmp(&before, &menu.config, sizeof(before)) == 0;
    }
    else if (scenario == 5)
    {
        menu.rowCount = 1; menu.rows[0] = ROW_DONE;
        RequestIntroConfirmation();
        TestPressMenuKey(DPAD_UP);
        ok &= menu.confirmChoice == 1;
        TestPressMenuKey(DPAD_LEFT);
        ok &= menu.confirmChoice == 0 && !menu.introCommitted;
        TestPressMenuKey(DPAD_RIGHT);
        TestPressMenuKey(A_BUTTON); // Confirm Go back, not another value change.
        ok &= menu.page == PAGE_SETUP && !menu.introConfirmed && !menu.introCommitted;
        RequestIntroConfirmation();
        TestPressMenuKey(B_BUTTON);
        ok &= menu.page == PAGE_SETUP && memcmp(&before, &menu.config, sizeof(before)) == 0;
    }
    else if (scenario == 6)
    {
        menu.page = PAGE_ASCENSION; menu.rowCount = 1; menu.rows[0] = ROW_ASCENSION;
        TestPressMenuKey(DPAD_RIGHT);
        ok &= menu.candidate == 1 && menu.config.ascension == 0;
        TestPressMenuKey(B_BUTTON);
        ok &= menu.page == PAGE_SETUP && menu.config.ascension == 0;
        menu.page = PAGE_ASCENSION; menu.candidate = 0;
        TestPressMenuKey(A_BUTTON);
        ok &= menu.page == PAGE_SETUP && menu.config.ascension == 0;
    }
    else ok = FALSE;
    sMenu = previous;
    sTestSkipDraw = FALSE;
    gMain.newKeys = oldNew; gMain.newAndRepeatedKeys = oldRepeat;
    gPaletteFade.active = oldFade;
    return ok;
}
#endif

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
    sMenu = AllocZeroed(sizeof(*sMenu) + 2 * sizeof(struct MenuDrawing));
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
    sMenu->hasDrawing = FALSE;
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
