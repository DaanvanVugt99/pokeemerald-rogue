#include "global.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/rogue.h"
#include "constants/rogue_pokedex.h"
#include "constants/script_menu.h"
#include "constants/vars.h"

#include "event_data.h"
#include "battle_setup.h"
#include "item.h"
#include "pokemon.h"
#include "random.h"
#include "script_menu.h"
#include "string_util.h"
#include "strings.h"

#include "rogue.h"
#include "rogue_baked.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_multiplayer.h"
#include "rogue_pokedex.h"
#include "rogue_query.h"
#include "rogue_quest.h"
#include "rogue_save.h"
#include "rogue_script.h"
#include "rogue_settings.h"
#include "rogue_trainers.h"
#include "rogue_trials.h"

struct RogueTrialPendingSelection
{
    u8 trialId;
    u8 difficulty;
    u8 pokedexVariant;
    bool8 isPending;
};

struct RogueTrialBattleMonBackup
{
    u32 exp;
    u16 hp;
    u16 maxHP;
    u16 attack;
    u16 defense;
    u16 speed;
    u16 spAttack;
    u16 spDefense;
    u8 level;
    bool8 isValid;
};

static EWRAM_DATA struct RogueTrialPendingSelection sPendingTrial = {0};
static EWRAM_DATA struct RogueTrialBattleMonBackup sPlayerBattleBackups[PARTY_SIZE] = {0};
static EWRAM_DATA struct RogueTrialBattleMonBackup sEnemyBattleBackups[PARTY_SIZE] = {0};
static EWRAM_DATA bool8 sHasBattleBackups = FALSE;
static EWRAM_DATA bool8 sCaughtMonPendingGive = FALSE;

static const u8 sText_Easy[] = _("Easy");
static const u8 sText_Average[] = _("Average");
static const u8 sText_Hard[] = _("Hard");
static const u8 sText_Brutal[] = _("Brutal");
static const u8 sText_Back[] = _("Back");
static const u8 sText_TypeNormal[] = _("Normal Trial");
static const u8 sText_TypeFighting[] = _("Fighting Trial");
static const u8 sText_TypeFlying[] = _("Flying Trial");
static const u8 sText_TypePoison[] = _("Poison Trial");
static const u8 sText_TypeGround[] = _("Ground Trial");
static const u8 sText_TypeRock[] = _("Rock Trial");
static const u8 sText_TypeBug[] = _("Bug Trial");
static const u8 sText_TypeGhost[] = _("Ghost Trial");
static const u8 sText_TypeSteel[] = _("Steel Trial");
static const u8 sText_TypeFire[] = _("Fire Trial");
static const u8 sText_TypeWater[] = _("Water Trial");
static const u8 sText_TypeGrass[] = _("Grass Trial");
static const u8 sText_TypeElectric[] = _("Electric Trial");
static const u8 sText_TypePsychic[] = _("Psychic Trial");
static const u8 sText_TypeIce[] = _("Ice Trial");
static const u8 sText_TypeDragon[] = _("Dragon Trial");
static const u8 sText_TypeDark[] = _("Dark Trial");
static const u8 sText_TypeFairy[] = _("Fairy Trial");
static const u8 sText_TypeTrials[] = _("Type Trials ▶");
static const u8 sText_LittleCup[] = _("Little Cup");
static const u8 sText_LowBst[] = _("BST Crown");
static const u8 sText_RandomanRoulette[] = _("Randoman Roulette");
static const u8 sText_Equalized[] = _("Equalized");
static const u8 sText_RegionKanto[] = _("Kanto Style");
static const u8 sText_RegionJohto[] = _("Johto Style");
static const u8 sText_RegionHoenn[] = _("Hoenn Style");
static const u8 sText_RegionSinnoh[] = _("Sinnoh Style");
static const u8 sText_RegionUnova[] = _("Unova Style");
static const u8 sText_RegionKalos[] = _("Kalos Style");
static const u8 sText_RegionAlola[] = _("Alola Style");
static const u8 sText_RegionGalar[] = _("Galar Style");
static const u8 sText_RegionPaldea[] = _("Paldea Style");
static const u8 sText_ZARoyale[] = _("Z-A Royale");
static const u8 sText_OrreStyle[] = _("Orre Style");
static const u8 sText_Roguelocke[] = _("Roguelocke");
static const u8 sText_CantPick[] = _("Can't Pick!?");
static const u8 sText_CursedBody[] = _("Cursed Body");
static const u8 sText_ProBuilding[] = _("Pro-Building");
static const u8 sText_InsaneMode[] = _("Insane Mode");
static const u8 sText_IronMono[] = _("Iron Mono");
static const u8 sText_IronKaizo[] = _("Iron Kaizo");
static const u8 sText_ChaosMaster[] = _("Chaos Master");
static const u8 sText_Apotheosis[] = _("Apotheosis");
static const u8 sText_LimitedCapture[] = _("Limited Capture");
static const u8 sText_RegionalStyles[] = _("Regional Styles ▶");
static const u8 sText_YourParty[] = _("your party");
static const u8 sText_DayCare[] = _("the Day Care");
static const u8 sText_ChosenPartner[] = _("your chosen partner");
static const u8 sText_PokedexRogue[] = _("Rogue ▶");
static const u8 sText_PokedexKanto[] = _("Kanto ▶");
static const u8 sText_PokedexJohto[] = _("Johto ▶");
static const u8 sText_PokedexHoenn[] = _("Hoenn ▶");
static const u8 sText_PokedexSinnoh[] = _("Sinnoh ▶");
static const u8 sText_PokedexUnova[] = _("Unova ▶");
static const u8 sText_PokedexKalos[] = _("Kalos ▶");
static const u8 sText_PokedexAlola[] = _("Alola ▶");
static const u8 sText_PokedexGalar[] = _("Galar ▶");
static const u8 sText_PokedexPaldea[] = _("Paldea ▶");
static const u8 sText_PokedexExtras[] = _("Extras ▶");
static const u8 sText_PokedexNational[] = _("National ▶");
static const u8 sText_PokedexLegends[] = _("Legends ▶");

static const u8 sText_TypeTrialDesc[] = _("Only matching-type {PKMN}\nmay enter Trainer battles.");
static const u8 sText_LittleCupDesc[] = _("Only first-stage evolving\n{PKMN}. Battles are Lv. 5.");
static const u8 sText_LowBstDesc[] = _("Only {PKMN} with 400 BST\nor lower may battle.");
static const u8 sText_RandomanRouletteDesc[] = _("Your full party is rerolled\non every adventure map.");
static const u8 sText_EqualizedDesc[] = _("All {PKMN} use proportional\n500 BST stat spreads.");
static const u8 sText_RegionTrialDesc[] = _("Uses that region's Pokédex\nand Trainer pool.");
static const u8 sText_ZARoyaleDesc[] = _("Z-A Pokédex with Rainbow\nregional Trainers.");
static const u8 sText_OrreStyleDesc[] = _("Doubles with Snag Curse.\nStart with Umbreon+Espeon.");
static const u8 sText_RoguelockeDesc[] = _("Random starter with 10\nWild Encounter Curses.");
static const u8 sText_CantPickDesc[] = _("Only starter-family {PKMN}\nare legal.");
static const u8 sText_CursedBodyDesc[] = _("Snowball Curse is active.");
static const u8 sText_ProBuildingDesc[] = _("Auto Move Curse is active.");
static const u8 sText_InsaneModeDesc[] = _("Fresh Start, Mixed battles,\nand no Legendary {PKMN}.");
static const u8 sText_IronMonoDesc[] = _("Party capacity is one.");
static const u8 sText_IronKaizoDesc[] = _("Fresh Start, one {PKMN},\nand maximum shop prices.");
static const u8 sText_ChaosMasterDesc[] = _("Your full party rerolls\nbefore every Trainer battle.");
static const u8 sText_ApotheosisDesc[] = _("Build a team using only\nLegendary and Mythical {PKMN}.");
static const u8 sText_LimitedCaptureDesc[] = _("Build a team from one starter\nand exactly five captures.");

static const u8 sRule_TypeLegality[] = _("Trainer party must match the Trial type.");
static const u8 sRule_LittleCupLegality[] = _("Only first-stage evolving {PKMN} are legal.");
static const u8 sRule_LittleCupLevel[] = _("All battle {PKMN} are set to Lv. 5.");
static const u8 sRule_LittleCupEvolution[] = _("Evolution is blocked.");
static const u8 sRule_NoTrainerExp[] = _("Trainer battles award no Exp. Points.");
static const u8 sRule_LowBstLegality[] = _("Only {PKMN} with 400 BST or lower are legal.");
static const u8 sRule_LowBstEvolution[] = _("Evolution is blocked above 400 BST.");
static const u8 sRule_RandomanReroll[] = _("The full party rerolls on every adventure map.");
static const u8 sRule_RandomanDex[] = _("Rerolled {PKMN} use the selected Pokédex.");
static const u8 sRule_EqualizedBst[] = _("All {PKMN} use a 500 BST stat spread.");
static const u8 sRule_EqualizedStats[] = _("Stats retain their original proportions.");
static const u8 sRule_EqualizedIdentity[] = _("Species, types, moves, and Abilities stay the same.");
static const u8 sRule_RegionalDex[] = _("Only selected Pokédex species are legal.");
static const u8 sRule_RegionalTrainers[] = _("Uses the matching regional Trainer pool.");
static const u8 sRule_ZADex[] = _("Only selected Z-A Pokédex species are legal.");
static const u8 sRule_ZATrainers[] = _("Uses Trainers from every region.");
static const u8 sRule_ZAOrder[] = _("Trainer order is Rainbow.");
static const u8 sRule_GimmickNone[] = _("Battle gimmicks are disabled.");
static const u8 sRule_GimmickMega[] = _("Mega Ring supplied; only Mega Evolution is active.");
static const u8 sRule_GimmickZMove[] = _("Z-Power Ring supplied; only Z-Moves are active.");
static const u8 sRule_GimmickDynamax[] = _("Dynamax Band supplied; only Dynamax is active.");
static const u8 sRule_GimmickTera[] = _("Tera Orb supplied; only Terastallization is active.");
static const u8 sRule_Doubles[] = _("Trainer battles are Doubles.");
static const u8 sRule_OrreSnag[] = _("Snag Curse is active.");
static const u8 sRule_OrreStart[] = _("Start with Umbreon and Espeon.");
static const u8 sRule_OrreFinish[] = _("Finish with Umbreon and Espeon.");
static const u8 sRule_RandomStarter[] = _("A random starter replaces the current party.");
static const u8 sRule_WildCurse10[] = _("10 Wild Encounter Curses are active.");
static const u8 sRule_StarterOnly[] = _("Only starter-family {PKMN} are legal.");
static const u8 sRule_Snowball[] = _("Snowball Curse is active.");
static const u8 sRule_AutoMove[] = _("Auto Move Curse is active.");
static const u8 sRule_FreshStart[] = _("Fresh Start removes the starting Bag.");
static const u8 sRule_MixedBattles[] = _("Trainer battles use the Mixed format.");
static const u8 sRule_Unaware[] = _("Unaware Curse is active.");
static const u8 sRule_NoLegendaries[] = _("Legendary and Mythical {PKMN} are illegal.");
static const u8 sRule_HardDifficulty[] = _("Difficulty is fixed to Hard.");
static const u8 sRule_PartyOne[] = _("Party capacity is one {PKMN}.");
static const u8 sRule_MaxShopPrices[] = _("99 Discount Curses maximize shop prices.");
static const u8 sRule_BattleReroll[] = _("The full party rerolls before every Trainer battle.");
static const u8 sRule_LegendaryOnly[] = _("Only Legendary and Mythical {PKMN} are legal.");
static const u8 sRule_GuaranteedCatch[] = _("Every legal Poké Ball throw catches successfully.");
static const u8 sRule_FiveCaptures[] = _("Exactly five successful captures are allowed.");
static const u8 sRule_NoDayCare[] = _("The Day Care is unavailable.");
static const u8 sRule_NoGiftMons[] = _("Gifts and trades cannot add {PKMN}.");

static const u8 *const sRules_Type[] = {sRule_TypeLegality};
static const u8 *const sRules_LittleCup[] = {sRule_LittleCupLegality, sRule_LittleCupLevel, sRule_NoTrainerExp, sRule_LittleCupEvolution};
static const u8 *const sRules_LowBst[] = {sRule_LowBstLegality, sRule_LowBstEvolution};
static const u8 *const sRules_Randoman[] = {sRule_RandomanReroll, sRule_RandomanDex};
static const u8 *const sRules_Equalized[] = {sRule_EqualizedBst, sRule_EqualizedStats, sRule_EqualizedIdentity};
static const u8 *const sRules_Regional[] = {sRule_RegionalDex, sRule_RegionalTrainers};
static const u8 *const sRules_ZA[] = {sRule_ZADex, sRule_ZATrainers, sRule_ZAOrder, sRule_Doubles};
static const u8 *const sRules_Orre[] = {sRule_Doubles, sRule_OrreSnag, sRule_OrreStart, sRule_OrreFinish};
static const u8 *const sRules_Roguelocke[] = {sRule_RandomStarter, sRule_WildCurse10};
static const u8 *const sRules_CantPick[] = {sRule_StarterOnly};
static const u8 *const sRules_CursedBody[] = {sRule_Snowball};
static const u8 *const sRules_ProBuilding[] = {sRule_AutoMove};
static const u8 *const sRules_InsaneMode[] = {sRule_HardDifficulty, sRule_FreshStart, sRule_MixedBattles, sRule_RandomStarter, sRule_Unaware, sRule_NoLegendaries};
static const u8 *const sRules_IronMono[] = {sRule_PartyOne};
static const u8 *const sRules_IronKaizo[] = {sRule_FreshStart, sRule_RandomStarter, sRule_PartyOne, sRule_MaxShopPrices};
static const u8 *const sRules_ChaosMaster[] = {sRule_BattleReroll};
static const u8 *const sRules_Apotheosis[] = {sRule_RandomStarter, sRule_LegendaryOnly, sRule_GuaranteedCatch};
static const u8 *const sRules_LimitedCapture[] = {sRule_FreshStart, sRule_RandomStarter, sRule_FiveCaptures, sRule_GuaranteedCatch, sRule_NoDayCare, sRule_NoGiftMons};

static const u16 sOrreStartingParty[] = {SPECIES_UMBREON, SPECIES_ESPEON};

#ifdef ROGUE_EXPANSION
static const struct RogueTrialBattleGimmickProfile sGimmicks_Kanto[] =
{
    {POKEDEX_VARIANT_KANTO_RBY, ROGUE_TRIAL_GIMMICK_NONE},
    {POKEDEX_VARIANT_KANTO_LETSGO, ROGUE_TRIAL_GIMMICK_MEGA},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Johto[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_NONE},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Hoenn[] =
{
    {POKEDEX_VARIANT_HOENN_RSE, ROGUE_TRIAL_GIMMICK_NONE},
    {POKEDEX_VARIANT_HOENN_ORAS, ROGUE_TRIAL_GIMMICK_MEGA},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Sinnoh[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_NONE},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Unova[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_NONE},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Kalos[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_MEGA},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Alola[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_Z_MOVE},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Galar[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_DYNAMAX},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Paldea[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_TERASTALLIZATION},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_ZA[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_MEGA},
};
static const struct RogueTrialBattleGimmickProfile sGimmicks_Orre[] =
{
    {POKEDEX_VARIANT_NONE, ROGUE_TRIAL_GIMMICK_NONE},
};
#endif

static const u8 *const sDifficultyNames[DIFFICULTY_PRESET_COUNT] =
{
    [DIFFICULTY_LEVEL_EASY] = sText_Easy,
    [DIFFICULTY_LEVEL_AVERAGE] = sText_Average,
    [DIFFICULTY_LEVEL_HARD] = sText_Hard,
    [DIFFICULTY_LEVEL_BRUTAL] = sText_Brutal,
};

static const u8 *const sPokedexRegionMenuNames[POKEDEX_REGION_COUNT] =
{
    [POKEDEX_REGION_ROGUE] = sText_PokedexRogue,
    [POKEDEX_REGION_KANTO] = sText_PokedexKanto,
    [POKEDEX_REGION_JOHTO] = sText_PokedexJohto,
    [POKEDEX_REGION_HOENN] = sText_PokedexHoenn,
    [POKEDEX_REGION_SINNOH] = sText_PokedexSinnoh,
    [POKEDEX_REGION_UNOVA] = sText_PokedexUnova,
    [POKEDEX_REGION_KALOS] = sText_PokedexKalos,
    [POKEDEX_REGION_ALOLA] = sText_PokedexAlola,
    [POKEDEX_REGION_GALAR] = sText_PokedexGalar,
    [POKEDEX_REGION_PALDEA] = sText_PokedexPaldea,
    [POKEDEX_REGION_EXTRAS] = sText_PokedexExtras,
    [POKEDEX_REGION_NATIONAL] = sText_PokedexNational,
    [POKEDEX_REGION_LEGENDS] = sText_PokedexLegends,
};

#define ROGUE_TRIAL_MENU_GROUP_TYPE 125
#define ROGUE_TRIAL_MENU_GROUP_REGIONAL 126
#define ROGUE_TRIAL_POKEDEX_MENU_GROUP_BASE 128

STATIC_ASSERT(ROGUE_TRIAL_COUNT < ROGUE_TRIAL_MENU_GROUP_TYPE, RogueTrialMenuGroupIdsFit);
STATIC_ASSERT(ROGUE_TRIAL_POKEDEX_MENU_GROUP_BASE + POKEDEX_REGION_COUNT < UINT8_MAX, RogueTrialPokedexMenuGroupIdsFit);

static const struct RogueTrialDefinition sTrialDefinitions[ROGUE_TRIAL_COUNT] =
{
    [ROGUE_TRIAL_NONE] =
    {
        .id = ROGUE_TRIAL_NONE,
        .name = NULL,
        .description = NULL,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .forcedTrainerToggle = ROGUE_TRIAL_NO_TRAINER_TOGGLE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
    },
    [ROGUE_TRIAL_TYPE_NORMAL] =
    {
        .id = ROGUE_TRIAL_TYPE_NORMAL,
        .name = sText_TypeNormal,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .forcedTrainerToggle = ROGUE_TRIAL_NO_TRAINER_TOGGLE,
        .requiredType = TYPE_NORMAL,
    },
    [ROGUE_TRIAL_TYPE_FIGHTING] =
    {
        .id = ROGUE_TRIAL_TYPE_FIGHTING,
        .name = sText_TypeFighting,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_FIGHTING,
    },
    [ROGUE_TRIAL_TYPE_FLYING] =
    {
        .id = ROGUE_TRIAL_TYPE_FLYING,
        .name = sText_TypeFlying,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_FLYING,
    },
    [ROGUE_TRIAL_TYPE_POISON] =
    {
        .id = ROGUE_TRIAL_TYPE_POISON,
        .name = sText_TypePoison,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_POISON,
    },
    [ROGUE_TRIAL_TYPE_GROUND] =
    {
        .id = ROGUE_TRIAL_TYPE_GROUND,
        .name = sText_TypeGround,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_GROUND,
    },
    [ROGUE_TRIAL_TYPE_ROCK] =
    {
        .id = ROGUE_TRIAL_TYPE_ROCK,
        .name = sText_TypeRock,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_ROCK,
    },
    [ROGUE_TRIAL_TYPE_BUG] =
    {
        .id = ROGUE_TRIAL_TYPE_BUG,
        .name = sText_TypeBug,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_BUG,
    },
    [ROGUE_TRIAL_TYPE_GHOST] =
    {
        .id = ROGUE_TRIAL_TYPE_GHOST,
        .name = sText_TypeGhost,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_GHOST,
    },
    [ROGUE_TRIAL_TYPE_STEEL] =
    {
        .id = ROGUE_TRIAL_TYPE_STEEL,
        .name = sText_TypeSteel,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_STEEL,
    },
    [ROGUE_TRIAL_TYPE_FIRE] =
    {
        .id = ROGUE_TRIAL_TYPE_FIRE,
        .name = sText_TypeFire,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_FIRE,
    },
    [ROGUE_TRIAL_TYPE_WATER] =
    {
        .id = ROGUE_TRIAL_TYPE_WATER,
        .name = sText_TypeWater,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_WATER,
    },
    [ROGUE_TRIAL_TYPE_GRASS] =
    {
        .id = ROGUE_TRIAL_TYPE_GRASS,
        .name = sText_TypeGrass,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_GRASS,
    },
    [ROGUE_TRIAL_TYPE_ELECTRIC] =
    {
        .id = ROGUE_TRIAL_TYPE_ELECTRIC,
        .name = sText_TypeElectric,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_ELECTRIC,
    },
    [ROGUE_TRIAL_TYPE_PSYCHIC] =
    {
        .id = ROGUE_TRIAL_TYPE_PSYCHIC,
        .name = sText_TypePsychic,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_PSYCHIC,
    },
    [ROGUE_TRIAL_TYPE_ICE] =
    {
        .id = ROGUE_TRIAL_TYPE_ICE,
        .name = sText_TypeIce,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_ICE,
    },
    [ROGUE_TRIAL_TYPE_DRAGON] =
    {
        .id = ROGUE_TRIAL_TYPE_DRAGON,
        .name = sText_TypeDragon,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_DRAGON,
    },
    [ROGUE_TRIAL_TYPE_DARK] =
    {
        .id = ROGUE_TRIAL_TYPE_DARK,
        .name = sText_TypeDark,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_DARK,
    },
    [ROGUE_TRIAL_TYPE_FAIRY] =
    {
        .id = ROGUE_TRIAL_TYPE_FAIRY,
        .name = sText_TypeFairy,
        .description = sText_TypeTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = TYPE_FAIRY,
    },
    [ROGUE_TRIAL_LITTLE_CUP] =
    {
        .id = ROGUE_TRIAL_LITTLE_CUP,
        .name = sText_LittleCup,
        .description = sText_LittleCupDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleLevel = 5,
        .requiresLittleCup = TRUE,
        .enforceOpponentSpeciesLegality = TRUE,
        .disableTrainerBattleExp = TRUE,
    },
    [ROGUE_TRIAL_LOW_BST] =
    {
        .id = ROGUE_TRIAL_LOW_BST,
        .name = sText_LowBst,
        .description = sText_LowBstDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .maxBst = 400,
    },
    [ROGUE_TRIAL_RANDOMAN_ROULETTE] =
    {
        .id = ROGUE_TRIAL_RANDOMAN_ROULETTE,
        .name = sText_RandomanRoulette,
        .description = sText_RandomanRouletteDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .randomizePartyOnEnter = TRUE,
    },
    [ROGUE_TRIAL_EQUALIZED] =
    {
        .id = ROGUE_TRIAL_EQUALIZED,
        .name = sText_Equalized,
        .description = sText_EqualizedDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .normalizedBst = 500,
    },
    [ROGUE_TRIAL_REGION_KANTO] =
    {
        .id = ROGUE_TRIAL_REGION_KANTO,
        .name = sText_RegionKanto,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_KANTO_LETSGO,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_KANTO,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_KANTO,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
#ifdef ROGUE_EXPANSION
        .battleGimmickProfiles = sGimmicks_Kanto,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Kanto),
#endif
    },
    [ROGUE_TRIAL_REGION_JOHTO] =
    {
        .id = ROGUE_TRIAL_REGION_JOHTO,
        .name = sText_RegionJohto,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_JOHTO_HGSS,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_JOHTO,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_JOHTO,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
#ifdef ROGUE_EXPANSION
        .battleGimmickProfiles = sGimmicks_Johto,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Johto),
#endif
    },
    [ROGUE_TRIAL_REGION_HOENN] =
    {
        .id = ROGUE_TRIAL_REGION_HOENN,
        .name = sText_RegionHoenn,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_HOENN_ORAS,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_HOENN,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_HOENN,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
#ifdef ROGUE_EXPANSION
        .battleGimmickProfiles = sGimmicks_Hoenn,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Hoenn),
#endif
    },
#ifdef ROGUE_EXPANSION
    [ROGUE_TRIAL_REGION_SINNOH] =
    {
        .id = ROGUE_TRIAL_REGION_SINNOH,
        .name = sText_RegionSinnoh,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_SINNOH_PL,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_SINNOH,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_SINNOH,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleGimmickProfiles = sGimmicks_Sinnoh,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Sinnoh),
    },
    [ROGUE_TRIAL_REGION_UNOVA] =
    {
        .id = ROGUE_TRIAL_REGION_UNOVA,
        .name = sText_RegionUnova,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_UNOVA_BW2,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_UNOVA,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_UNOVA,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleGimmickProfiles = sGimmicks_Unova,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Unova),
    },
    [ROGUE_TRIAL_REGION_KALOS] =
    {
        .id = ROGUE_TRIAL_REGION_KALOS,
        .name = sText_RegionKalos,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_KALOS,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_KALOS,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_KALOS,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleGimmickProfiles = sGimmicks_Kalos,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Kalos),
    },
    [ROGUE_TRIAL_REGION_ALOLA] =
    {
        .id = ROGUE_TRIAL_REGION_ALOLA,
        .name = sText_RegionAlola,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_ALOLA_USUM,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_ALOLA,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_ALOLA,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleGimmickProfiles = sGimmicks_Alola,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Alola),
    },
    [ROGUE_TRIAL_REGION_GALAR] =
    {
        .id = ROGUE_TRIAL_REGION_GALAR,
        .name = sText_RegionGalar,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_GALAR_FULLDLC,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_GALAR,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_GALAR,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleGimmickProfiles = sGimmicks_Galar,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Galar),
    },
    [ROGUE_TRIAL_REGION_PALDEA] =
    {
        .id = ROGUE_TRIAL_REGION_PALDEA,
        .name = sText_RegionPaldea,
        .description = sText_RegionTrialDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_PALDEA_FULLDLC,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_PALDEA,
        .forcedTrainerToggle = CONFIG_TOGGLE_TRAINER_PALDEA,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .battleGimmickProfiles = sGimmicks_Paldea,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Paldea),
    },
    [ROGUE_TRIAL_Z_A_ROYALE] =
    {
        .id = ROGUE_TRIAL_Z_A_ROYALE,
        .name = sText_ZARoyale,
        .description = sText_ZARoyaleDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_LEGENDS_ZAFULLDLC,
        .pokedexSet = ROGUE_TRIAL_POKEDEX_SET_LEGENDS_ZA,
        .forcedTrainerToggle = ROGUE_TRIAL_NO_TRAINER_TOGGLE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .forcedTrainerOrder = TRAINER_ORDER_RAINBOW,
        .hasForcedTrainerOrder = TRUE,
        .forcedBattleFormat = BATTLE_FORMAT_DOUBLES,
        .hasForcedBattleFormat = TRUE,
        .enableAllRegionalTrainers = TRUE,
        .battleGimmickProfiles = sGimmicks_ZA,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_ZA),
    },
#endif
    [ROGUE_TRIAL_ORRE_STYLE] =
    {
        .id = ROGUE_TRIAL_ORRE_STYLE,
        .name = sText_OrreStyle,
        .description = sText_OrreStyleDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .forcedTrainerToggle = ROGUE_TRIAL_NO_TRAINER_TOGGLE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .forcedBattleFormat = BATTLE_FORMAT_DOUBLES,
        .hasForcedBattleFormat = TRUE,
        .curseEffect = EFFECT_SNAG_TRAINER_MON,
        .curseCount = 1,
        .hasCurseEffect = TRUE,
        .fixedStartingParty = sOrreStartingParty,
        .fixedStartingPartyCount = ARRAY_COUNT(sOrreStartingParty),
#ifdef ROGUE_EXPANSION
        .battleGimmickProfiles = sGimmicks_Orre,
        .battleGimmickProfileCount = ARRAY_COUNT(sGimmicks_Orre),
#endif
    },
    [ROGUE_TRIAL_ROGUELOCKE] =
    {
        .id = ROGUE_TRIAL_ROGUELOCKE,
        .name = sText_Roguelocke,
        .description = sText_RoguelockeDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .curseEffect = EFFECT_WILD_ENCOUNTER_COUNT,
        .curseCount = 10,
        .hasCurseEffect = TRUE,
        .forceRandomStarter = TRUE,
    },
    [ROGUE_TRIAL_CANT_PICK] =
    {
        .id = ROGUE_TRIAL_CANT_PICK,
        .name = sText_CantPick,
        .description = sText_CantPickDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .requiresStarterSpecies = TRUE,
    },
    [ROGUE_TRIAL_CURSED_BODY] =
    {
        .id = ROGUE_TRIAL_CURSED_BODY,
        .name = sText_CursedBody,
        .description = sText_CursedBodyDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .curseEffect = EFFECT_SNOWBALL_CURSES,
        .curseCount = 1,
        .hasCurseEffect = TRUE,
    },
    [ROGUE_TRIAL_PRO_BUILDING] =
    {
        .id = ROGUE_TRIAL_PRO_BUILDING,
        .name = sText_ProBuilding,
        .description = sText_ProBuildingDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .curseEffect = EFFECT_AUTO_MOVE_SELECT,
        .curseCount = 1,
        .hasCurseEffect = TRUE,
    },
    [ROGUE_TRIAL_INSANE_MODE] =
    {
        .id = ROGUE_TRIAL_INSANE_MODE,
        .name = sText_InsaneMode,
        .description = sText_InsaneModeDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .forcedBattleFormat = BATTLE_FORMAT_MIXED,
        .hasForcedBattleFormat = TRUE,
        .forcedDifficulty = DIFFICULTY_LEVEL_HARD,
        .hasForcedDifficulty = TRUE,
        .curseEffect = EFFECT_UNAWARE_STATUS,
        .curseCount = 1,
        .hasCurseEffect = TRUE,
        .forceRandomStarter = TRUE,
        .forceFreshStart = TRUE,
        .forbidsLegendarySpecies = TRUE,
    },
    [ROGUE_TRIAL_IRON_MONO] =
    {
        .id = ROGUE_TRIAL_IRON_MONO,
        .name = sText_IronMono,
        .description = sText_IronMonoDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .curseEffect = EFFECT_PARTY_SIZE,
        .curseCount = 5,
        .hasCurseEffect = TRUE,
    },
    [ROGUE_TRIAL_IRON_KAIZO] =
    {
        .id = ROGUE_TRIAL_IRON_KAIZO,
        .name = sText_IronKaizo,
        .description = sText_IronKaizoDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .curseEffect = EFFECT_PARTY_SIZE,
        .curseCount = 5,
        .extraCurseEffect = EFFECT_SHOP_PRICE,
        .extraCurseCount = 99,
        .hasCurseEffect = TRUE,
        .forceRandomStarter = TRUE,
        .forceFreshStart = TRUE,
    },
    [ROGUE_TRIAL_CHAOS_MASTER] =
    {
        .id = ROGUE_TRIAL_CHAOS_MASTER,
        .name = sText_ChaosMaster,
        .description = sText_ChaosMasterDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .randomizeBeforeTrainerBattle = TRUE,
    },
    [ROGUE_TRIAL_APOTHEOSIS] =
    {
        .id = ROGUE_TRIAL_APOTHEOSIS,
        .name = sText_Apotheosis,
        .description = sText_ApotheosisDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .forceRandomStarter = TRUE,
        .requiresLegendarySpecies = TRUE,
        .guaranteeCatch = TRUE,
    },
    [ROGUE_TRIAL_LIMITED_CAPTURE] =
    {
        .id = ROGUE_TRIAL_LIMITED_CAPTURE,
        .name = sText_LimitedCapture,
        .description = sText_LimitedCaptureDesc,
        .forcedPokedexVariant = POKEDEX_VARIANT_NONE,
        .requiredType = ROGUE_TRIAL_NO_TYPE,
        .captureLimit = 5,
        .forceRandomStarter = TRUE,
        .forceFreshStart = TRUE,
        .guaranteeCatch = TRUE,
        .disableDayCare = TRUE,
        .disableGifts = TRUE,
        .disableRandoman = TRUE,
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

static bool8 IsTypeTrialId(u8 trialId)
{
    return trialId >= ROGUE_TRIAL_TYPE_NORMAL && trialId <= ROGUE_TRIAL_TYPE_FAIRY;
}

static bool8 IsRegionalStyleTrialId(u8 trialId)
{
    return trialId >= ROGUE_TRIAL_REGION_KANTO && trialId <= ROGUE_TRIAL_ORRE_STYLE;
}

static bool8 IsGroupedTrialId(u8 trialId)
{
    return IsTypeTrialId(trialId) || IsRegionalStyleTrialId(trialId);
}

static u16 GetTrialQuestId(u8 trialId)
{
    static const u16 sTypeQuestIds[] =
    {
        QUEST_ID_NORMAL_MASTER,
        QUEST_ID_FIGHTING_MASTER,
        QUEST_ID_FLYING_MASTER,
        QUEST_ID_POISON_MASTER,
        QUEST_ID_GROUND_MASTER,
        QUEST_ID_ROCK_MASTER,
        QUEST_ID_BUG_MASTER,
        QUEST_ID_GHOST_MASTER,
        QUEST_ID_STEEL_MASTER,
        QUEST_ID_FIRE_MASTER,
        QUEST_ID_WATER_MASTER,
        QUEST_ID_GRASS_MASTER,
        QUEST_ID_ELECTRIC_MASTER,
        QUEST_ID_PSYCHIC_MASTER,
        QUEST_ID_ICE_MASTER,
        QUEST_ID_DRAGON_MASTER,
        QUEST_ID_DARK_MASTER,
#ifdef ROGUE_EXPANSION
        QUEST_ID_FAIRY_MASTER,
#endif
    };

    if (IsTypeTrialId(trialId))
        return sTypeQuestIds[trialId - ROGUE_TRIAL_TYPE_NORMAL];

    switch (trialId)
    {
    case ROGUE_TRIAL_LITTLE_CUP: return QUEST_ID_LITTLE_CUP;
    case ROGUE_TRIAL_LOW_BST: return QUEST_ID_BST_CROWN;
    case ROGUE_TRIAL_RANDOMAN_ROULETTE: return QUEST_ID_RANDOMAN_ROULETTE;
    case ROGUE_TRIAL_EQUALIZED: return QUEST_ID_EQUALIZED;
    case ROGUE_TRIAL_REGION_KANTO: return QUEST_ID_KANTO_STYLE;
    case ROGUE_TRIAL_REGION_JOHTO: return QUEST_ID_JOHTO_STYLE;
    case ROGUE_TRIAL_REGION_HOENN: return QUEST_ID_HOENN_STYLE;
#ifdef ROGUE_EXPANSION
    case ROGUE_TRIAL_REGION_SINNOH: return QUEST_ID_SINNOH_STYLE;
    case ROGUE_TRIAL_REGION_UNOVA: return QUEST_ID_UNOVA_STYLE;
    case ROGUE_TRIAL_REGION_KALOS: return QUEST_ID_KALOS_STYLE;
    case ROGUE_TRIAL_REGION_ALOLA: return QUEST_ID_ALOLA_STYLE;
    case ROGUE_TRIAL_REGION_GALAR: return QUEST_ID_GALAR_STYLE;
    case ROGUE_TRIAL_REGION_PALDEA: return QUEST_ID_PALDEA_STYLE;
    case ROGUE_TRIAL_Z_A_ROYALE: return QUEST_ID_Z_A_ROYALE;
#endif
    case ROGUE_TRIAL_ORRE_STYLE: return QUEST_ID_ORRE_STYLE;
    case ROGUE_TRIAL_ROGUELOCKE: return QUEST_ID_ROGUELOCKE;
    case ROGUE_TRIAL_CANT_PICK: return QUEST_ID_CANT_PICKEMARKQMARK;
    case ROGUE_TRIAL_CURSED_BODY: return QUEST_ID_CURSED_BODY;
    case ROGUE_TRIAL_PRO_BUILDING: return QUEST_ID_PRO_BUILDING;
    case ROGUE_TRIAL_INSANE_MODE: return QUEST_ID_INSANE_MODE;
    case ROGUE_TRIAL_IRON_MONO: return QUEST_ID_IRON_MONO;
    case ROGUE_TRIAL_IRON_KAIZO: return QUEST_ID_IRON_KAIZO;
    case ROGUE_TRIAL_CHAOS_MASTER: return QUEST_ID_CHAOS_MASTER;
    case ROGUE_TRIAL_APOTHEOSIS: return QUEST_ID_APOTHEOSIS;
    case ROGUE_TRIAL_LIMITED_CAPTURE: return QUEST_ID_LIMITED_CAPTURE;
    default: return QUEST_ID_COUNT;
    }
}

static bool8 IsTrialAvailableForMenu(u8 trialId)
{
    u16 questId;

    if (!IsValidTrialId(trialId) || sTrialDefinitions[trialId].name == NULL)
        return FALSE;

#ifndef ROGUE_EXPANSION
    if (trialId == ROGUE_TRIAL_TYPE_FAIRY)
        return FALSE;
#endif

    questId = GetTrialQuestId(trialId);
    return questId < QUEST_ID_COUNT && RogueQuest_IsQuestUnlocked(questId);
}

static bool8 IsTrialInMenuGroup(u8 trialId, u16 groupId)
{
    switch (groupId)
    {
    case ROGUE_TRIAL_MENU_GROUP_TYPE:
        return IsTypeTrialId(trialId);
    case ROGUE_TRIAL_MENU_GROUP_REGIONAL:
        return IsRegionalStyleTrialId(trialId);
    }

    return FALSE;
}

static bool8 HasTrialInMenuGroup(u16 groupId)
{
    u8 i;

    for (i = ROGUE_TRIAL_NONE + 1; i < ROGUE_TRIAL_COUNT; ++i)
    {
        if (IsTrialAvailableForMenu(i) && IsTrialInMenuGroup(i, groupId))
            return TRUE;
    }

    return FALSE;
}

static bool8 HasVisibleTrialOptions(void)
{
    u8 i;

    for (i = ROGUE_TRIAL_NONE + 1; i < ROGUE_TRIAL_COUNT; ++i)
    {
        if (IsTrialAvailableForMenu(i))
            return TRUE;
    }

    return FALSE;
}

static bool8 IsValidPokedexVariant(u8 variant)
{
    return variant < POKEDEX_VARIANT_COUNT;
}

static bool8 IsValidPokedexRegion(u8 region)
{
    return region < POKEDEX_REGION_COUNT;
}

static bool8 IsPokedexVariantAllowedForSet(u8 pokedexSet, u8 variant)
{
    if (!IsValidPokedexVariant(variant))
        return FALSE;

    switch (pokedexSet)
    {
    case ROGUE_TRIAL_POKEDEX_SET_ALL:
        return TRUE;
    case ROGUE_TRIAL_POKEDEX_SET_KANTO:
        return variant == POKEDEX_VARIANT_KANTO_RBY || variant == POKEDEX_VARIANT_KANTO_LETSGO;
    case ROGUE_TRIAL_POKEDEX_SET_JOHTO:
        return variant == POKEDEX_VARIANT_JOHTO_GSC || variant == POKEDEX_VARIANT_JOHTO_HGSS;
    case ROGUE_TRIAL_POKEDEX_SET_HOENN:
        return variant == POKEDEX_VARIANT_HOENN_RSE || variant == POKEDEX_VARIANT_HOENN_ORAS;
    case ROGUE_TRIAL_POKEDEX_SET_SINNOH:
        return variant == POKEDEX_VARIANT_SINNOH_DP || variant == POKEDEX_VARIANT_SINNOH_PL;
    case ROGUE_TRIAL_POKEDEX_SET_UNOVA:
        return variant == POKEDEX_VARIANT_UNOVA_BW || variant == POKEDEX_VARIANT_UNOVA_BW2;
    case ROGUE_TRIAL_POKEDEX_SET_KALOS:
        return variant == POKEDEX_VARIANT_KALOS;
    case ROGUE_TRIAL_POKEDEX_SET_ALOLA:
        return variant == POKEDEX_VARIANT_ALOLA_SM || variant == POKEDEX_VARIANT_ALOLA_USUM;
    case ROGUE_TRIAL_POKEDEX_SET_GALAR:
        return variant == POKEDEX_VARIANT_GALAR_SWSH
            || variant == POKEDEX_VARIANT_GALAR_ISLEOFARMOR
            || variant == POKEDEX_VARIANT_GALAR_CROWNTUNDRA
            || variant == POKEDEX_VARIANT_GALAR_FULLDLC;
    case ROGUE_TRIAL_POKEDEX_SET_PALDEA:
        return variant == POKEDEX_VARIANT_PALDEA_SCVI
            || variant == POKEDEX_VARIANT_PALDEA_KITAKAMI
            || variant == POKEDEX_VARIANT_PALDEA_BLUEBERRY
            || variant == POKEDEX_VARIANT_PALDEA_FULLDLC;
    case ROGUE_TRIAL_POKEDEX_SET_LEGENDS_ZA:
        return variant == POKEDEX_VARIANT_LEGENDS_ZA || variant == POKEDEX_VARIANT_LEGENDS_ZAFULLDLC;
    }

    return FALSE;
}

static u16 GetPokedexRegionMenuGroupId(u8 region)
{
    return ROGUE_TRIAL_POKEDEX_MENU_GROUP_BASE + region;
}

static bool8 IsPokedexRegionMenuGroupId(u16 menuId)
{
    return menuId >= ROGUE_TRIAL_POKEDEX_MENU_GROUP_BASE
        && menuId < ROGUE_TRIAL_POKEDEX_MENU_GROUP_BASE + POKEDEX_REGION_COUNT;
}

static u8 GetPokedexRegionFromMenuGroupId(u16 menuId)
{
    if (!IsPokedexRegionMenuGroupId(menuId))
        return POKEDEX_REGION_NONE;

    return menuId - ROGUE_TRIAL_POKEDEX_MENU_GROUP_BASE;
}

static u8 GetAllowedPokedexVariantCountForRegion(const struct RogueTrialDefinition *trial, u8 region)
{
    u8 i;
    u8 count = 0;

    if (trial == NULL || !IsValidPokedexRegion(region))
        return 0;

    for (i = 0; i < gPokedexRegions[region].variantCount; ++i)
    {
        if (IsPokedexVariantAllowedForSet(trial->pokedexSet, gPokedexRegions[region].variantList[i]))
            ++count;
    }

    return count;
}

static u8 GetFirstAllowedPokedexVariantForRegion(const struct RogueTrialDefinition *trial, u8 region)
{
    u8 i;

    if (trial == NULL || !IsValidPokedexRegion(region))
        return POKEDEX_VARIANT_NONE;

    for (i = 0; i < gPokedexRegions[region].variantCount; ++i)
    {
        u8 variant = gPokedexRegions[region].variantList[i];

        if (IsPokedexVariantAllowedForSet(trial->pokedexSet, variant))
            return variant;
    }

    return POKEDEX_VARIANT_NONE;
}

static u8 GetDefaultPokedexVariant(const struct RogueTrialDefinition *trial)
{
    u8 i;
    u8 currentVariant = RoguePokedex_GetDexVariant();

    if (trial == NULL)
        return POKEDEX_VARIANT_NONE;

    if (trial->forcedPokedexVariant != POKEDEX_VARIANT_NONE
        && IsPokedexVariantAllowedForSet(trial->pokedexSet, trial->forcedPokedexVariant))
        return trial->forcedPokedexVariant;

    if (IsPokedexVariantAllowedForSet(trial->pokedexSet, currentVariant))
        return currentVariant;

    for (i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
    {
        if (IsPokedexVariantAllowedForSet(trial->pokedexSet, i))
            return i;
    }

    return POKEDEX_VARIANT_NONE;
}

static u8 CountPokedexOptions(const struct RogueTrialDefinition *trial)
{
    u8 i;
    u8 count = 0;

    if (trial == NULL)
        return 0;

    for (i = 0; i < POKEDEX_VARIANT_COUNT; ++i)
    {
        if (IsPokedexVariantAllowedForSet(trial->pokedexSet, i))
            ++count;
    }

    return count;
}

static bool8 IsSpeciesType(u16 species, u8 type)
{
    return RoguePokedex_GetSpeciesType(species, 0) == type || RoguePokedex_GetSpeciesType(species, 1) == type;
}

static bool8 SpeciesHasRealEvolution(u16 species)
{
    u8 i;
    const struct Evolution *evolutions = GetSpeciesEvolutions(species);

    if (evolutions == NULL)
        return FALSE;

    for (i = 0; evolutions[i].method != EVOLUTIONS_END; ++i)
    {
        if (evolutions[i].method != EVO_NONE && evolutions[i].targetSpecies != SPECIES_NONE)
            return TRUE;
    }

    return FALSE;
}

static bool8 SpeciesIsLittleCupLegal(u16 species)
{
    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;

    if (Rogue_GetEggSpecies(species) != species)
        return FALSE;

    return SpeciesHasRealEvolution(species);
}

static bool8 SpeciesIsStarterFamily(u16 species)
{
    switch (Rogue_GetEggSpecies(species))
    {
    case SPECIES_PICHU:
    case SPECIES_EEVEE:
    case SPECIES_BULBASAUR:
    case SPECIES_SQUIRTLE:
    case SPECIES_CHARMANDER:
    case SPECIES_CHIKORITA:
    case SPECIES_TOTODILE:
    case SPECIES_CYNDAQUIL:
    case SPECIES_TREECKO:
    case SPECIES_MUDKIP:
    case SPECIES_TORCHIC:
#ifdef ROGUE_EXPANSION
    case SPECIES_TURTWIG:
    case SPECIES_PIPLUP:
    case SPECIES_CHIMCHAR:
    case SPECIES_SNIVY:
    case SPECIES_OSHAWOTT:
    case SPECIES_TEPIG:
    case SPECIES_CHESPIN:
    case SPECIES_FROAKIE:
    case SPECIES_FENNEKIN:
    case SPECIES_ROWLET:
    case SPECIES_LITTEN:
    case SPECIES_POPPLIO:
    case SPECIES_GROOKEY:
    case SPECIES_SCORBUNNY:
    case SPECIES_SOBBLE:
    case SPECIES_SPRIGATITO:
    case SPECIES_FUECOCO:
    case SPECIES_QUAXLY:
#endif
        return TRUE;
    default:
        return FALSE;
    }
}

static bool8 TrialUsesSelectedDexLegality(const struct RogueTrialDefinition *trial)
{
    return trial != NULL && trial->pokedexSet != ROGUE_TRIAL_POKEDEX_SET_ALL;
}

static bool8 DefinitionRequiresSpeciesLegality(const struct RogueTrialDefinition *trial)
{
    return trial != NULL
        && (trial->requiredType != ROGUE_TRIAL_NO_TYPE
            || trial->maxBst != 0
            || trial->requiresLittleCup
            || trial->requiresStarterSpecies
            || trial->forbidsLegendarySpecies
            || trial->requiresLegendarySpecies
            || TrialUsesSelectedDexLegality(trial));
}

static bool8 SpeciesIsEnabledForDexVariant(u16 species, u8 dexVariant)
{
    bool8 enabled;
    u8 currentVariant;

    if (!IsValidPokedexVariant(dexVariant))
        return FALSE;

    currentVariant = RoguePokedex_GetDexVariant();
    if (currentVariant != dexVariant)
        RoguePokedex_SetDexVariant(dexVariant);

    enabled = Query_IsSpeciesEnabledForceDexChecking(species);

    if (currentVariant != dexVariant)
        RoguePokedex_SetDexVariant(currentVariant);

    return enabled;
}

static bool8 DefinitionAllowsSpeciesRules(const struct RogueTrialDefinition *trial, u16 species)
{
    if (trial == NULL || species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;

    if (trial->requiredType != ROGUE_TRIAL_NO_TYPE && !IsSpeciesType(species, trial->requiredType))
        return FALSE;

    if (trial->maxBst != 0 && RoguePokedex_GetSpeciesBST(species) > trial->maxBst)
        return FALSE;

    if (trial->requiresLittleCup && !SpeciesIsLittleCupLegal(species))
        return FALSE;

    if (trial->requiresStarterSpecies && !SpeciesIsStarterFamily(species))
        return FALSE;

    if (trial->forbidsLegendarySpecies && RoguePokedex_IsSpeciesLegendary(species))
        return FALSE;

    if (trial->requiresLegendarySpecies && !RoguePokedex_IsSpeciesLegendary(species))
        return FALSE;

    return TRUE;
}

static bool8 DefinitionAllowsSpeciesForDex(const struct RogueTrialDefinition *trial, u16 species, u8 dexVariant)
{
    if (trial == NULL || species == SPECIES_NONE || species == SPECIES_EGG)
        return FALSE;

    if (TrialUsesSelectedDexLegality(trial) && !SpeciesIsEnabledForDexVariant(species, dexVariant))
        return FALSE;

    return DefinitionAllowsSpeciesRules(trial, species);
}

static bool8 DefinitionAllowsSpecies(const struct RogueTrialDefinition *trial, u16 species)
{
    return DefinitionAllowsSpeciesForDex(trial, species, RoguePokedex_GetDexVariant());
}

bool8 RogueTrial_PendingAllowsSpecies(u16 species)
{
    const struct RogueTrialDefinition *trial;
    u8 dexVariant;

    if (species == SPECIES_NONE)
        return TRUE;

    dexVariant = sPendingTrial.isPending ? sPendingTrial.pokedexVariant : RoguePokedex_GetDexVariant();
    if (!SpeciesIsEnabledForDexVariant(species, dexVariant))
        return FALSE;

    if (!sPendingTrial.isPending)
        return TRUE;

    if (!IsValidTrialId(sPendingTrial.trialId) || !IsValidDifficulty(sPendingTrial.difficulty))
        return FALSE;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    if (trial == NULL || !IsPokedexVariantAllowedForSet(trial->pokedexSet, sPendingTrial.pokedexVariant))
        return FALSE;

    if (!DefinitionRequiresSpeciesLegality(trial))
        return TRUE;

    return DefinitionAllowsSpeciesRules(trial, species);
}

bool8 RogueTrial_PendingNeedsStarterFilter(void)
{
    const struct RogueTrialDefinition *trial;

    if (!sPendingTrial.isPending || !IsValidTrialId(sPendingTrial.trialId) || !IsValidDifficulty(sPendingTrial.difficulty))
        return FALSE;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    if (trial == NULL || !IsPokedexVariantAllowedForSet(trial->pokedexSet, sPendingTrial.pokedexVariant))
        return FALSE;

    return trial->requiredType != ROGUE_TRIAL_NO_TYPE
        || trial->maxBst != 0
        || trial->requiresLittleCup
        || trial->requiresStarterSpecies
        || trial->forbidsLegendarySpecies
        || trial->requiresLegendarySpecies;
}

bool8 RogueTrial_PendingRequiresLegendarySpecies(void)
{
    const struct RogueTrialDefinition *trial;

    if (!sPendingTrial.isPending)
        return FALSE;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    return trial != NULL && trial->requiresLegendarySpecies;
}

struct PendingTrialQueryFilterData
{
    const struct RogueTrialDefinition *trial;
};

static bool8 QueryFilter_PendingTrialSpecies(u16 species, void *usrData)
{
    const struct PendingTrialQueryFilterData *filterData = usrData;

    return Query_IsSpeciesEnabledForceDexChecking(species)
        && DefinitionAllowsSpeciesRules(filterData->trial, species);
}

void RogueTrial_FilterPendingMonQuery(void)
{
    const struct RogueTrialDefinition *trial;
    struct PendingTrialQueryFilterData filterData;
    u8 previousDexVariant;

    if (!sPendingTrial.isPending || !IsValidTrialId(sPendingTrial.trialId) || !IsValidDifficulty(sPendingTrial.difficulty))
        return;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    if (trial == NULL || !IsPokedexVariantAllowedForSet(trial->pokedexSet, sPendingTrial.pokedexVariant))
        return;

    if (DefinitionRequiresSpeciesLegality(trial))
    {
        filterData.trial = trial;
        previousDexVariant = RoguePokedex_GetDexVariant();

        if (previousDexVariant != sPendingTrial.pokedexVariant)
            RoguePokedex_SetDexVariant(sPendingTrial.pokedexVariant);

        RogueMonQuery_CustomFilter(QueryFilter_PendingTrialSpecies, &filterData);

        if (previousDexVariant != sPendingTrial.pokedexVariant)
            RoguePokedex_SetDexVariant(previousDexVariant);
    }
}

static bool8 BufferPendingValidationFailure(const u8 *sourceText, u16 species)
{
    if (species == SPECIES_NONE || RogueTrial_PendingAllowsSpecies(species))
        return FALSE;

    StringCopy(gStringVar1, sourceText);
    StringCopy(gStringVar2, RoguePokedex_GetSpeciesName(species));
    return TRUE;
}

static bool8 CurrentTrialRequiresSpeciesLegality(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

    return DefinitionRequiresSpeciesLegality(trial);
}

static void SetOnlyTrainerToggle(u8 trainerToggle)
{
    if (trainerToggle == ROGUE_TRIAL_NO_TRAINER_TOGGLE)
        return;

    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, trainerToggle == CONFIG_TOGGLE_TRAINER_KANTO);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, trainerToggle == CONFIG_TOGGLE_TRAINER_JOHTO);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, trainerToggle == CONFIG_TOGGLE_TRAINER_HOENN);
#ifdef ROGUE_EXPANSION
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH, trainerToggle == CONFIG_TOGGLE_TRAINER_SINNOH);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA, trainerToggle == CONFIG_TOGGLE_TRAINER_UNOVA);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS, trainerToggle == CONFIG_TOGGLE_TRAINER_KALOS);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA, trainerToggle == CONFIG_TOGGLE_TRAINER_ALOLA);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR, trainerToggle == CONFIG_TOGGLE_TRAINER_GALAR);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA, trainerToggle == CONFIG_TOGGLE_TRAINER_PALDEA);
#endif
}

static void SetAllRegionalTrainerToggles(void)
{
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ROGUE, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KANTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_JOHTO, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_HOENN, TRUE);
#ifdef ROGUE_EXPANSION
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_SINNOH, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_UNOVA, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_KALOS, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_ALOLA, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_GALAR, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_TRAINER_PALDEA, TRUE);
#endif
}

const struct RogueTrialDefinition *RogueTrial_GetDefinition(u8 trialId)
{
    if (trialId < ROGUE_TRIAL_COUNT)
        return &sTrialDefinitions[trialId];

    return NULL;
}

bool8 RogueTrial_GetBattleGimmick(u8 trialId, u8 pokedexVariant, u8 *gimmick)
{
    u8 i;
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(trialId);
    const struct RogueTrialBattleGimmickProfile *fallback = NULL;

    if (trial == NULL || gimmick == NULL)
        return FALSE;

    for (i = 0; i < trial->battleGimmickProfileCount; ++i)
    {
        const struct RogueTrialBattleGimmickProfile *profile = &trial->battleGimmickProfiles[i];

        if (profile->pokedexVariant == pokedexVariant)
        {
            *gimmick = profile->gimmick;
            return TRUE;
        }

        if (profile->pokedexVariant == POKEDEX_VARIANT_NONE)
            fallback = profile;
    }

    if (fallback != NULL)
    {
        *gimmick = fallback->gimmick;
        return TRUE;
    }

    return FALSE;
}

static const u8 *GetBattleGimmickRuleText(u8 gimmick)
{
    switch (gimmick)
    {
    case ROGUE_TRIAL_GIMMICK_NONE:
        return sRule_GimmickNone;
    case ROGUE_TRIAL_GIMMICK_MEGA:
        return sRule_GimmickMega;
    case ROGUE_TRIAL_GIMMICK_Z_MOVE:
        return sRule_GimmickZMove;
    case ROGUE_TRIAL_GIMMICK_DYNAMAX:
        return sRule_GimmickDynamax;
    case ROGUE_TRIAL_GIMMICK_TERASTALLIZATION:
        return sRule_GimmickTera;
    default:
        return gText_Exit;
    }
}

static u16 GetBattleGimmickItem(u8 gimmick)
{
    switch (gimmick)
    {
    case ROGUE_TRIAL_GIMMICK_MEGA:
        return ITEM_MEGA_RING;
    case ROGUE_TRIAL_GIMMICK_Z_MOVE:
        return ITEM_Z_POWER_RING;
    case ROGUE_TRIAL_GIMMICK_DYNAMAX:
        return ITEM_DYNAMAX_BAND;
    case ROGUE_TRIAL_GIMMICK_TERASTALLIZATION:
        return ITEM_TERA_ORB;
    default:
        return ITEM_NONE;
    }
}

static const u8 *const *GetTrialRules(u8 trialId, u8 *ruleCount)
{
    if (IsTypeTrialId(trialId))
    {
        *ruleCount = ARRAY_COUNT(sRules_Type);
        return sRules_Type;
    }

    if (trialId >= ROGUE_TRIAL_REGION_KANTO && trialId <= ROGUE_TRIAL_REGION_PALDEA)
    {
        *ruleCount = ARRAY_COUNT(sRules_Regional);
        return sRules_Regional;
    }

    switch (trialId)
    {
    case ROGUE_TRIAL_LITTLE_CUP:
        *ruleCount = ARRAY_COUNT(sRules_LittleCup);
        return sRules_LittleCup;
    case ROGUE_TRIAL_LOW_BST:
        *ruleCount = ARRAY_COUNT(sRules_LowBst);
        return sRules_LowBst;
    case ROGUE_TRIAL_RANDOMAN_ROULETTE:
        *ruleCount = ARRAY_COUNT(sRules_Randoman);
        return sRules_Randoman;
    case ROGUE_TRIAL_EQUALIZED:
        *ruleCount = ARRAY_COUNT(sRules_Equalized);
        return sRules_Equalized;
    case ROGUE_TRIAL_Z_A_ROYALE:
        *ruleCount = ARRAY_COUNT(sRules_ZA);
        return sRules_ZA;
    case ROGUE_TRIAL_ORRE_STYLE:
        *ruleCount = ARRAY_COUNT(sRules_Orre);
        return sRules_Orre;
    case ROGUE_TRIAL_ROGUELOCKE:
        *ruleCount = ARRAY_COUNT(sRules_Roguelocke);
        return sRules_Roguelocke;
    case ROGUE_TRIAL_CANT_PICK:
        *ruleCount = ARRAY_COUNT(sRules_CantPick);
        return sRules_CantPick;
    case ROGUE_TRIAL_CURSED_BODY:
        *ruleCount = ARRAY_COUNT(sRules_CursedBody);
        return sRules_CursedBody;
    case ROGUE_TRIAL_PRO_BUILDING:
        *ruleCount = ARRAY_COUNT(sRules_ProBuilding);
        return sRules_ProBuilding;
    case ROGUE_TRIAL_INSANE_MODE:
        *ruleCount = ARRAY_COUNT(sRules_InsaneMode);
        return sRules_InsaneMode;
    case ROGUE_TRIAL_IRON_MONO:
        *ruleCount = ARRAY_COUNT(sRules_IronMono);
        return sRules_IronMono;
    case ROGUE_TRIAL_IRON_KAIZO:
        *ruleCount = ARRAY_COUNT(sRules_IronKaizo);
        return sRules_IronKaizo;
    case ROGUE_TRIAL_CHAOS_MASTER:
        *ruleCount = ARRAY_COUNT(sRules_ChaosMaster);
        return sRules_ChaosMaster;
    case ROGUE_TRIAL_APOTHEOSIS:
        *ruleCount = ARRAY_COUNT(sRules_Apotheosis);
        return sRules_Apotheosis;
    case ROGUE_TRIAL_LIMITED_CAPTURE:
        *ruleCount = ARRAY_COUNT(sRules_LimitedCapture);
        return sRules_LimitedCapture;
    default:
        *ruleCount = 0;
        return NULL;
    }
}

u8 RogueTrial_GetRuleCount(u8 trialId, u8 pokedexVariant)
{
    u8 ruleCount;
    u8 gimmick;

    GetTrialRules(trialId, &ruleCount);
    if (RogueTrial_GetBattleGimmick(trialId, pokedexVariant, &gimmick))
        ++ruleCount;

    return ruleCount;
}

const u8 *RogueTrial_GetRuleText(u8 trialId, u8 pokedexVariant, u8 ruleIndex)
{
    u8 ruleCount;
    u8 gimmick;
    const u8 *const *rules = GetTrialRules(trialId, &ruleCount);

    if (rules != NULL && ruleIndex < ruleCount)
        return rules[ruleIndex];

    if (ruleIndex == ruleCount && RogueTrial_GetBattleGimmick(trialId, pokedexVariant, &gimmick))
        return GetBattleGimmickRuleText(gimmick);

    return gText_Exit;
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

    if (trial != NULL
        && IsValidTrialId(sPendingTrial.trialId)
        && IsValidDifficulty(sPendingTrial.difficulty)
        && IsPokedexVariantAllowedForSet(trial->pokedexSet, sPendingTrial.pokedexVariant))
    {
        gRogueRun.trialState.trialId = sPendingTrial.trialId;
        gRogueRun.trialState.difficulty = sPendingTrial.difficulty;
        gRogueRun.trialState.initialPartyCount = CalculatePlayerPartyCount();
        gRogueRun.trialState.invalidated = FALSE;
        gRogueRun.trialState.initialPartyCountSet = TRUE;

        gRogueSaveBlock->lastTrialId = sPendingTrial.trialId;
        gRogueSaveBlock->lastTrialDifficulty = sPendingTrial.difficulty;
        gRogueSaveBlock->lastTrialPokedexVariant = sPendingTrial.pokedexVariant;
        gRogueSaveBlock->hasLastTrialSelection = TRUE;

        Rogue_SetDifficultyPreset(sPendingTrial.difficulty);
        RoguePokedex_SetDexVariant(sPendingTrial.pokedexVariant);

        SetOnlyTrainerToggle(trial->forcedTrainerToggle);

        if (trial->enableAllRegionalTrainers)
            SetAllRegionalTrainerToggles();

        if (trial->hasForcedTrainerOrder)
            Rogue_SetConfigRange(CONFIG_RANGE_TRAINER_ORDER, trial->forcedTrainerOrder);

        if (trial->hasForcedBattleFormat)
            Rogue_SetConfigRange(CONFIG_RANGE_BATTLE_FORMAT, trial->forcedBattleFormat);

        if (trial->forceFreshStart)
            Rogue_SetConfigToggle(CONFIG_TOGGLE_BAG_WIPE, TRUE);

        FlagClear(FLAG_ROGUE_ADVENTURE_REPLAY_ACTIVE);
    }

    RogueTrial_ClearPendingSelection();
}

void RogueTrial_ApplyRunBagItems(void)
{
    u8 gimmick;
    u16 item;

    if (!RogueTrial_GetBattleGimmick(gRogueRun.trialState.trialId, RoguePokedex_GetDexVariant(), &gimmick))
        return;

    item = GetBattleGimmickItem(gimmick);

    if (item != ITEM_NONE && !CheckBagHasItem(item, 1))
    {
        bool8 success = AddBagItem(item, 1);

        if (!success)
            AGB_ASSERT(FALSE);
    }
}

void RogueTrial_ApplyBattleGimmickOverride(void)
{
#ifdef ROGUE_EXPANSION
    u8 gimmick;

    if (!RogueTrial_GetBattleGimmick(gRogueRun.trialState.trialId, RoguePokedex_GetDexVariant(), &gimmick))
        return;

    gRogueRun.megasEnabled = gimmick == ROGUE_TRIAL_GIMMICK_MEGA;
    gRogueRun.zMovesEnabled = gimmick == ROGUE_TRIAL_GIMMICK_Z_MOVE;
    gRogueRun.dynamaxEnabled = gimmick == ROGUE_TRIAL_GIMMICK_DYNAMAX;
    gRogueRun.terastallizeEnabled = gimmick == ROGUE_TRIAL_GIMMICK_TERASTALLIZATION;
#endif
}

u8 RogueTrial_GetPendingForcedPokedexVariant(void)
{
    const struct RogueTrialDefinition *trial;

    if (!sPendingTrial.isPending || !IsValidTrialId(sPendingTrial.trialId) || !IsValidDifficulty(sPendingTrial.difficulty))
        return POKEDEX_VARIANT_NONE;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    if (trial == NULL)
        return POKEDEX_VARIANT_NONE;

    if (!IsPokedexVariantAllowedForSet(trial->pokedexSet, sPendingTrial.pokedexVariant))
        return POKEDEX_VARIANT_NONE;

    return sPendingTrial.pokedexVariant;
}

u16 RogueTrial_GetCharmItemCount(u8 effectType UNUSED)
{
    return 0;
}

u16 RogueTrial_GetCurseItemCount(u8 effectType)
{
    if (RogueTrial_IsActiveTrial(ROGUE_TRIAL_LOW_BST) && effectType == EFFECT_WILD_EGG_SPECIES)
        return 1;

    if (RogueTrial_IsActive())
    {
        const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

        if (trial != NULL && trial->hasCurseEffect)
        {
            u16 count = 0;

            if (trial->curseEffect == effectType)
                count += trial->curseCount;
            if (trial->extraCurseEffect == effectType)
                count += trial->extraCurseCount;
            return count;
        }
    }

    return 0;
}

bool8 RogueTrial_IsSpeciesLegal(u16 species, u32 otId UNUSED)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_IsActive())
        return TRUE;

    if (!CurrentTrialRequiresSpeciesLegality())
        return TRUE;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    return DefinitionAllowsSpecies(trial, species);
}

bool8 RogueTrial_CanAcceptMon(struct Pokemon *mon)
{
    u16 species;
    u32 otId;

    if (!RogueTrial_IsActive())
        return TRUE;

    species = GetMonData(mon, MON_DATA_SPECIES);
    otId = GetMonData(mon, MON_DATA_OT_ID);

    return RogueTrial_IsSpeciesLegal(species, otId);
}

bool8 RogueTrial_TransformMonIfIllegal(struct Pokemon *mon)
{
    u16 species;
    u32 level;
    u32 heldItem;
    u32 pokeball;

    if (!RogueTrial_IsActive() || !CurrentTrialRequiresSpeciesLegality() || RogueTrial_CanAcceptMon(mon))
        return TRUE;

    level = GetMonData(mon, MON_DATA_LEVEL);
    heldItem = GetMonData(mon, MON_DATA_HELD_ITEM);
    pokeball = GetMonData(mon, MON_DATA_POKEBALL);

    RogueMonQuery_Begin();
    RogueMonQuery_IsSpeciesActive();
    RogueTrial_FilterMonQuery();

    while (RogueWeightQuery_IsOverSafeCapacity())
        RogueMiscQuery_FilterByChance(Random(), QUERY_FUNC_INCLUDE, 50, PARTY_SIZE);

    if (!RogueMiscQuery_AnyActiveElements())
    {
        RogueMonQuery_End();
        return FALSE;
    }

    species = RogueMiscQuery_SelectRandomElement(Random());
    RogueMonQuery_End();

    ZeroMonData(mon);
    CreateMon(mon, species, level, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    SetMonData(mon, MON_DATA_HELD_ITEM, &heldItem);
    SetMonData(mon, MON_DATA_POKEBALL, &pokeball);
    return TRUE;
}

bool8 RogueTrial_IsCompleteForQuest(u8 trialId)
{
    if (!RogueTrial_IsActiveTrial(trialId) || RogueTrial_IsInvalidated())
        return FALSE;

    if (trialId == ROGUE_TRIAL_LIMITED_CAPTURE
        && VarGet(VAR_ROGUE_TOTAL_RUN_CATCHES) != sTrialDefinitions[trialId].captureLimit)
        return FALSE;

    return TRUE;
}

void RogueTrial_OnCaughtMon(void)
{
    sCaughtMonPendingGive = TRUE;
}

bool8 RogueTrial_CanReceiveGift(void)
{
    const struct RogueTrialDefinition *trial;

    if (sCaughtMonPendingGive)
    {
        sCaughtMonPendingGive = FALSE;
        return TRUE;
    }

    if (!RogueTrial_IsActive())
        return TRUE;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    return trial == NULL || !trial->disableGifts;
}

bool8 RogueTrial_CanThrowBall(void)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_IsActive())
        return TRUE;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    return trial == NULL || trial->captureLimit == 0
        || VarGet(VAR_ROGUE_TOTAL_RUN_CATCHES) < trial->captureLimit;
}

bool8 RogueTrial_IsCatchGuaranteed(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

    return RogueTrial_IsActive() && trial != NULL && trial->guaranteeCatch;
}

bool8 RogueTrial_IsDayCareDisabled(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

    return RogueTrial_IsActive() && trial != NULL && trial->disableDayCare;
}

bool8 RogueTrial_IsRandomanDisabled(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

    return RogueTrial_IsActive() && trial != NULL && trial->disableRandoman;
}

bool8 RogueTrial_IsTrainerBattleExpDisabled(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

    return RogueTrial_IsActive() && trial != NULL && trial->disableTrainerBattleExp;
}

void RogueTrial_BufferDayCareDisabled(void)
{
    gSpecialVar_Result = RogueTrial_IsDayCareDisabled();
}

void RogueTrial_BufferRandomanDisabled(void)
{
    gSpecialVar_Result = RogueTrial_IsRandomanDisabled();
}

void RogueTrial_OnMonGiven(struct Pokemon *mon)
{
    if (RogueTrial_IsActive() && !RogueTrial_CanAcceptMon(mon))
        RogueTrial_Invalidate();
}

static bool8 QueryFilter_CurrentTrialSpecies(u16 species, void *usrData UNUSED)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

    return DefinitionAllowsSpecies(trial, species);
}

void RogueTrial_FilterMonQuery(void)
{
    if (RogueTrial_IsActive() && CurrentTrialRequiresSpeciesLegality())
        RogueMonQuery_CustomFilter(QueryFilter_CurrentTrialSpecies, NULL);
}

void RogueTrial_FilterOpponentMonQuery(void)
{
    if (RogueTrial_EnforcesOpponentSpeciesLegality())
        RogueTrial_FilterMonQuery();
}

bool8 RogueTrial_EnforcesOpponentSpeciesLegality(void)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_IsActive() || !CurrentTrialRequiresSpeciesLegality())
        return FALSE;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    return trial != NULL && trial->enforceOpponentSpeciesLegality;
}

u8 RogueTrial_ModifyOpponentEvoLevel(u8 level)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_EnforcesOpponentSpeciesLegality())
        return level;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    if (trial->battleLevel != 0)
        return min(level, trial->battleLevel);

    return level;
}

static bool8 ActivePartyIsLegal(void)
{
    u8 i;

    for (i = 0; i < PARTY_SIZE; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        u32 otId = GetMonData(&gPlayerParty[i], MON_DATA_OT_ID);

        if (species != SPECIES_NONE && !RogueTrial_IsSpeciesLegal(species, otId))
            return FALSE;
    }

    return TRUE;
}

void RogueTrial_OnEnterEncounter(void)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_IsActive())
        return;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    if (trial == NULL || !trial->randomizePartyOnEnter || CalculatePlayerPartyCount() == 0)
        return;

    gSpecialVar_0x8004 = 255;
    Rogue_RandomisePartyMon();
    CalculatePlayerPartyCount();
}

void RogueTrial_OnTrainerBattleStart(void)
{
    const struct RogueTrialDefinition *trial;

    if (!RogueTrial_IsActive() || Rogue_IsExpTrainer(gTrainerBattleOpponent_A))
        return;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    if (trial != NULL && trial->randomizeBeforeTrainerBattle && CalculatePlayerPartyCount() != 0)
    {
        gSpecialVar_0x8004 = 255;
        Rogue_RandomisePartyMon();
        CalculatePlayerPartyCount();
    }

    if (CurrentTrialRequiresSpeciesLegality() && !ActivePartyIsLegal())
        RogueTrial_Invalidate();
}

void RogueTrial_CanStartTrainerBattle(void)
{
    const struct RogueTrialDefinition *trial;
    u8 i;

    gSpecialVar_Result = TRUE;

    if (!RogueTrial_IsActive() || Rogue_IsExpTrainer(gTrainerBattleOpponent_A) || !CurrentTrialRequiresSpeciesLegality())
        return;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    for (i = 0; i < PARTY_SIZE; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);
        u32 otId = GetMonData(&gPlayerParty[i], MON_DATA_OT_ID);

        if (species != SPECIES_NONE && !RogueTrial_IsSpeciesLegal(species, otId))
        {
            StringCopy(gStringVar1, RoguePokedex_GetSpeciesName(species));
            StringCopy(gStringVar2, trial->name);
            gSpecialVar_Result = FALSE;
            return;
        }
    }
}

static void SaveMonBattleBackup(struct Pokemon *mon, struct RogueTrialBattleMonBackup *backup)
{
    backup->exp = GetMonData(mon, MON_DATA_EXP);
    backup->level = GetMonData(mon, MON_DATA_LEVEL);
    backup->hp = GetMonData(mon, MON_DATA_HP);
    backup->maxHP = GetMonData(mon, MON_DATA_MAX_HP);
    backup->attack = GetMonData(mon, MON_DATA_ATK);
    backup->defense = GetMonData(mon, MON_DATA_DEF);
    backup->speed = GetMonData(mon, MON_DATA_SPEED);
    backup->spAttack = GetMonData(mon, MON_DATA_SPATK);
    backup->spDefense = GetMonData(mon, MON_DATA_SPDEF);
    backup->isValid = GetMonData(mon, MON_DATA_SPECIES) != SPECIES_NONE;
}

static void RestoreMonBattleBackup(struct Pokemon *mon, const struct RogueTrialBattleMonBackup *backup)
{
    u16 battleMaxHP;
    u16 battleHP;
    u16 restoredMaxHP;
    u16 restoredHP;

    if (!backup->isValid || GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return;

    battleMaxHP = GetMonData(mon, MON_DATA_MAX_HP);
    battleHP = GetMonData(mon, MON_DATA_HP);

    SetMonData(mon, MON_DATA_EXP, &backup->exp);
    CalculateMonStats(mon);

    if (battleHP == 0)
    {
        restoredHP = 0;
    }
    else
    {
        if (battleMaxHP == 0)
            battleMaxHP = 1;

        restoredMaxHP = GetMonData(mon, MON_DATA_MAX_HP);
        restoredHP = (battleHP * restoredMaxHP + battleMaxHP - 1) / battleMaxHP;
        restoredHP = min(max(restoredHP, 1), restoredMaxHP);
    }

    SetMonData(mon, MON_DATA_HP, &restoredHP);
}

static void SetMonBattleLevel(struct Pokemon *mon, u8 level)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 exp;

    if (species == SPECIES_NONE || species == SPECIES_EGG)
        return;

    exp = Rogue_ModifyExperienceTables(gRogueSpeciesInfo[species].growthRate, level);
    SetMonData(mon, MON_DATA_EXP, &exp);
    CalculateMonStats(mon);
}

void RogueTrial_GetEffectiveSpeciesBaseStats(u16 species, u16 *stats, u8 statCount)
{
    u8 i;
    u16 currentBst;
    u16 normalizedBst = 0;
    u16 targetBst = 0;
    u16 baseStats[NUM_STATS] =
    {
        gRogueSpeciesInfo[species].baseHP,
        gRogueSpeciesInfo[species].baseAttack,
        gRogueSpeciesInfo[species].baseDefense,
        gRogueSpeciesInfo[species].baseSpeed,
        gRogueSpeciesInfo[species].baseSpAttack,
        gRogueSpeciesInfo[species].baseSpDefense,
    };

    AGB_ASSERT(statCount >= NUM_STATS);

    currentBst = 0;
    for (i = 0; i < NUM_STATS; ++i)
        currentBst += baseStats[i];

    if (RogueTrial_IsActive())
    {
        const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);

        if (trial != NULL)
            targetBst = trial->normalizedBst;
    }

    if (targetBst == 0 || currentBst == 0)
    {
        memcpy(stats, baseStats, sizeof(baseStats));
        return;
    }

    for (i = 0; i < NUM_STATS; ++i)
    {
        stats[i] = max(1, (baseStats[i] * targetBst + currentBst / 2) / currentBst);
        normalizedBst += stats[i];
    }

    while (normalizedBst != targetBst)
    {
        u8 bestStat = 0;

        for (i = 1; i < NUM_STATS; ++i)
        {
            if (normalizedBst < targetBst)
            {
                if (baseStats[i] > baseStats[bestStat])
                    bestStat = i;
            }
            else if (stats[i] > 1 && (stats[bestStat] <= 1 || baseStats[i] > baseStats[bestStat]))
            {
                bestStat = i;
            }
        }

        if (normalizedBst < targetBst)
        {
            ++stats[bestStat];
            ++normalizedBst;
        }
        else
        {
            --stats[bestStat];
            --normalizedBst;
        }
    }
}

static void ApplyBattleRulesToParty(struct Pokemon *party, struct RogueTrialBattleMonBackup *backups, u8 count, const struct RogueTrialDefinition *trial)
{
    u8 i;

    for (i = 0; i < count; ++i)
    {
        SaveMonBattleBackup(&party[i], &backups[i]);

        if (trial->battleLevel != 0)
            SetMonBattleLevel(&party[i], trial->battleLevel);

    }
}

void RogueTrial_OnTrainerTeamReady(void)
{
    const struct RogueTrialDefinition *trial;
    u8 partyCount;
    u8 i;

    if (!RogueTrial_IsActive() || Rogue_IsExpTrainer(gTrainerBattleOpponent_A))
        return;

    trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    if (trial == NULL)
        return;

    if (trial->enforceOpponentSpeciesLegality && CurrentTrialRequiresSpeciesLegality())
    {
        partyCount = CalculateEnemyPartyCount();
        for (i = 0; i < PARTY_SIZE; ++i)
        {
            if (GetMonData(&gEnemyParty[i], MON_DATA_SPECIES) != SPECIES_NONE
                && !RogueTrial_CanAcceptMon(&gEnemyParty[i]))
            {
                u16 species = Rogue_SelectTrainerReplacementSpecies(gTrainerBattleOpponent_A, gEnemyParty, i, partyCount);

                if (species != SPECIES_NONE)
                {
                    u32 level = GetMonData(&gEnemyParty[i], MON_DATA_LEVEL);
                    u32 heldItem = GetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM);
                    u32 pokeball = GetMonData(&gEnemyParty[i], MON_DATA_POKEBALL);

                    ZeroMonData(&gEnemyParty[i]);
                    CreateMon(&gEnemyParty[i], species, level, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
                    SetMonData(&gEnemyParty[i], MON_DATA_HELD_ITEM, &heldItem);
                    SetMonData(&gEnemyParty[i], MON_DATA_POKEBALL, &pokeball);
                }
                else
                {
                    RogueTrial_TransformMonIfIllegal(&gEnemyParty[i]);
                }
            }
        }
        CalculateEnemyPartyCount();
    }

    if (trial->battleLevel == 0)
        return;

    memset(sPlayerBattleBackups, 0, sizeof(sPlayerBattleBackups));
    memset(sEnemyBattleBackups, 0, sizeof(sEnemyBattleBackups));

    ApplyBattleRulesToParty(gPlayerParty, sPlayerBattleBackups, PARTY_SIZE, trial);
    ApplyBattleRulesToParty(gEnemyParty, sEnemyBattleBackups, PARTY_SIZE, trial);
    sHasBattleBackups = TRUE;

    CalculatePlayerPartyCount();
    CalculateEnemyPartyCount();
}

void RogueTrial_OnTrainerBattleEnd(void)
{
    u8 i;

    if (!sHasBattleBackups)
        return;

    for (i = 0; i < PARTY_SIZE; ++i)
    {
        RestoreMonBattleBackup(&gPlayerParty[i], &sPlayerBattleBackups[i]);
        RestoreMonBattleBackup(&gEnemyParty[i], &sEnemyBattleBackups[i]);
    }

    memset(sPlayerBattleBackups, 0, sizeof(sPlayerBattleBackups));
    memset(sEnemyBattleBackups, 0, sizeof(sEnemyBattleBackups));
    sHasBattleBackups = FALSE;

    CalculatePlayerPartyCount();
    CalculateEnemyPartyCount();
}

void RogueTrial_AppendTrialOptions(void)
{
    u8 i;

    if (HasTrialInMenuGroup(ROGUE_TRIAL_MENU_GROUP_TYPE))
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_TypeTrials, ROGUE_TRIAL_MENU_GROUP_TYPE);

    if (HasTrialInMenuGroup(ROGUE_TRIAL_MENU_GROUP_REGIONAL))
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_RegionalStyles, ROGUE_TRIAL_MENU_GROUP_REGIONAL);

    for (i = ROGUE_TRIAL_NONE + 1; i < ROGUE_TRIAL_COUNT; ++i)
    {
        if (!IsTrialAvailableForMenu(i) || IsGroupedTrialId(i))
            continue;

        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sTrialDefinitions[i].name, i);
    }

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gText_Exit, MULTI_B_PRESSED);
}

void RogueTrial_IsSelectedTrialGroup(void)
{
    gSpecialVar_Result = (gSpecialVar_0x8004 == ROGUE_TRIAL_MENU_GROUP_TYPE
        || gSpecialVar_0x8004 == ROGUE_TRIAL_MENU_GROUP_REGIONAL) ? TRUE : FALSE;
}

void RogueTrial_AppendSelectedTrialGroupOptions(void)
{
    u8 i;

    for (i = ROGUE_TRIAL_NONE + 1; i < ROGUE_TRIAL_COUNT; ++i)
    {
        if (!IsTrialAvailableForMenu(i) || !IsTrialInMenuGroup(i, gSpecialVar_0x8004))
            continue;

        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sTrialDefinitions[i].name, i);
    }

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
}

void RogueTrial_AppendDifficultyOptions(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sDifficultyNames); ++i)
        ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sDifficultyNames[i], i);

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
    ScriptMenu_ScrollingMultichoiceDynamicSetDefault(DIFFICULTY_LEVEL_AVERAGE);
}

void RogueTrial_SelectForcedDifficulty(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL && trial->hasForcedDifficulty && IsValidDifficulty(trial->forcedDifficulty))
    {
        gSpecialVar_0x8005 = trial->forcedDifficulty;
        gSpecialVar_Result = TRUE;
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

void RogueTrial_AppendPokedexOptions(void)
{
    u8 i;
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL && IsValidTrialId(gSpecialVar_0x8004))
    {
        for (i = 0; i < POKEDEX_REGION_COUNT; ++i)
        {
            u8 variantCount = GetAllowedPokedexVariantCountForRegion(trial, i);

            if (variantCount > 1)
                ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sPokedexRegionMenuNames[i], GetPokedexRegionMenuGroupId(i));
            else if (variantCount == 1)
            {
                u8 variant = GetFirstAllowedPokedexVariantForRegion(trial, i);

                if (IsValidPokedexVariant(variant))
                    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gPokedexVariants[variant].displayName, variant);
            }
        }
    }

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
}

void RogueTrial_IsSelectedPokedexGroup(void)
{
    gSpecialVar_Result = IsPokedexRegionMenuGroupId(gSpecialVar_0x8007) ? TRUE : FALSE;
}

void RogueTrial_AppendSelectedPokedexGroupOptions(void)
{
    u8 i;
    u8 region = GetPokedexRegionFromMenuGroupId(gSpecialVar_0x8007);
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL && IsValidTrialId(gSpecialVar_0x8004) && IsValidPokedexRegion(region))
    {
        for (i = 0; i < gPokedexRegions[region].variantCount; ++i)
        {
            u8 variant = gPokedexRegions[region].variantList[i];

            if (IsPokedexVariantAllowedForSet(trial->pokedexSet, variant))
                ScriptMenu_ScrollingMultichoiceDynamicAppendOption(gPokedexVariants[variant].displayName, variant);
        }
    }

    ScriptMenu_ScrollingMultichoiceDynamicAppendOption(sText_Back, MULTI_B_PRESSED);
}

void RogueTrial_GetPokedexOptionCount(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL && IsValidTrialId(gSpecialVar_0x8004))
        gSpecialVar_Result = CountPokedexOptions(trial);
    else
        gSpecialVar_Result = 0;
}

void RogueTrial_SelectDefaultPokedexOption(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);
    u8 variant = GetDefaultPokedexVariant(trial);

    if (variant != POKEDEX_VARIANT_NONE)
    {
        gSpecialVar_0x8006 = variant;
        gSpecialVar_Result = TRUE;
    }
    else
    {
        gSpecialVar_Result = FALSE;
    }
}

void RogueTrial_BufferTrialPreview(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL && IsValidTrialId(gSpecialVar_0x8004))
    {
        StringCopy(gStringVar1, trial->name);
        StringCopy(gStringVar2, trial->description);

        if (IsValidDifficulty(gSpecialVar_0x8005))
            StringCopy(gStringVar4, sDifficultyNames[gSpecialVar_0x8005]);
        else
            StringCopy(gStringVar4, sText_Back);
    }
    else
    {
        StringCopy(gStringVar1, gText_Exit);
        StringCopy(gStringVar2, gText_Exit);
        StringCopy(gStringVar4, gText_Exit);
    }

    if (IsValidPokedexVariant(gSpecialVar_0x8006))
        StringCopy(gStringVar3, gPokedexVariants[gSpecialVar_0x8006].displayName);
    else
        StringCopy(gStringVar3, sText_Back);
}

void RogueTrial_SetPendingSelectionFromScript(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gSpecialVar_0x8004);

    if (trial != NULL
        && IsValidTrialId(gSpecialVar_0x8004)
        && IsValidDifficulty(gSpecialVar_0x8005)
        && (!trial->hasForcedDifficulty || gSpecialVar_0x8005 == trial->forcedDifficulty)
        && IsPokedexVariantAllowedForSet(trial->pokedexSet, gSpecialVar_0x8006))
    {
        sPendingTrial.trialId = gSpecialVar_0x8004;
        sPendingTrial.difficulty = gSpecialVar_0x8005;
        sPendingTrial.pokedexVariant = gSpecialVar_0x8006;
        sPendingTrial.isPending = TRUE;
        RoguePokedex_SetDexVariant(sPendingTrial.pokedexVariant);
        gSpecialVar_Result = TRUE;
    }
    else
    {
        RogueTrial_ClearPendingSelection();
        gSpecialVar_Result = FALSE;
    }
}

void RogueTrial_HasPendingSelection(void)
{
    gSpecialVar_Result = sPendingTrial.isPending;
}

void RogueTrial_PendingHasFixedStartingParty(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(sPendingTrial.trialId);

    gSpecialVar_Result = sPendingTrial.isPending && trial != NULL && trial->fixedStartingPartyCount != 0;
}

void RogueTrial_PendingReplacesStartingParty(void)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(sPendingTrial.trialId);

    gSpecialVar_Result = sPendingTrial.isPending
        && trial != NULL
        && (trial->forceRandomStarter || trial->fixedStartingPartyCount != 0);
}

void RogueTrial_ApplyPendingPartyCapacity(void)
{
    const struct RogueTrialDefinition *trial;
    u8 trialCapacity;

    if (!sPendingTrial.isPending)
        return;

    trial = RogueTrial_GetDefinition(sPendingTrial.trialId);
    if (trial == NULL
        || trial->forceRandomStarter
        || trial->fixedStartingPartyCount != 0
        || !trial->hasCurseEffect
        || trial->curseEffect != EFFECT_PARTY_SIZE)
        return;

    trialCapacity = PARTY_SIZE - min(trial->curseCount, PARTY_SIZE - 1);
    gSpecialVar_Result = min(gSpecialVar_Result, trialCapacity);
}

void RogueTrial_CanUsePendingParty(void)
{
    u8 i;

    gSpecialVar_Result = TRUE;

    if (sPendingTrial.isPending)
    {
        const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(sPendingTrial.trialId);

        if (trial->forceRandomStarter || trial->fixedStartingPartyCount != 0)
            return;
    }

    for (i = 0; i < PARTY_SIZE; ++i)
    {
        u16 species = GetMonData(&gPlayerParty[i], MON_DATA_SPECIES);

        if (BufferPendingValidationFailure(sText_YourParty, species))
        {
            gSpecialVar_Result = FALSE;
            return;
        }
    }
}

void RogueTrial_CanUsePendingDayCare(void)
{
    const struct RogueTrialDefinition *trial;
    u8 i;

    gSpecialVar_Result = TRUE;
    trial = sPendingTrial.isPending ? RogueTrial_GetDefinition(sPendingTrial.trialId) : NULL;

    for (i = 0; i < Rogue_GetCurrentDaycareSlotCount(); ++i)
    {
        u16 species = GetBoxMonData(Rogue_GetDaycareBoxMon(i), MON_DATA_SPECIES);

        if (trial != NULL && trial->disableDayCare && species != SPECIES_NONE)
        {
            StringCopy(gStringVar1, sText_DayCare);
            StringCopy(gStringVar2, RoguePokedex_GetSpeciesName(species));
            gSpecialVar_Result = FALSE;
            return;
        }

        if (BufferPendingValidationFailure(sText_DayCare, species))
        {
            gSpecialVar_Result = FALSE;
            return;
        }
    }
}

void RogueTrial_CanStartPendingSelection(void)
{
    const struct RogueTrialDefinition *trial = sPendingTrial.isPending ? RogueTrial_GetDefinition(sPendingTrial.trialId) : NULL;
    u16 starterSpecies = VarGet(VAR_STARTER_SWAP_SPECIES);

    gSpecialVar_Result = TRUE;

    if (trial == NULL || trial->fixedStartingPartyCount == 0)
    {
        if (starterSpecies != SPECIES_NONE)
        {
            if (BufferPendingValidationFailure(sText_ChosenPartner, starterSpecies))
            {
                gSpecialVar_Result = FALSE;
                return;
            }
        }
        else
        {
            RogueTrial_CanUsePendingParty();
            if (!gSpecialVar_Result)
                return;
        }
    }

    RogueTrial_CanUsePendingDayCare();
}

void RogueTrial_ClearPendingSelection(void)
{
    memset(&sPendingTrial, 0, sizeof(sPendingTrial));
}

void RogueTrial_HasAvailableTrials(void)
{
    gSpecialVar_Result = HasVisibleTrialOptions();
}

void RogueTrial_LoadLastSelection(void)
{
    const struct RogueTrialDefinition *trial;

    gSpecialVar_Result = FALSE;
    if (!gRogueSaveBlock->hasLastTrialSelection
        || !IsValidTrialId(gRogueSaveBlock->lastTrialId)
        || !IsValidDifficulty(gRogueSaveBlock->lastTrialDifficulty))
        return;

    trial = RogueTrial_GetDefinition(gRogueSaveBlock->lastTrialId);
    if (trial == NULL
        || !IsTrialAvailableForMenu(gRogueSaveBlock->lastTrialId)
        || !IsPokedexVariantAllowedForSet(trial->pokedexSet, gRogueSaveBlock->lastTrialPokedexVariant)
        || (trial->hasForcedDifficulty && trial->forcedDifficulty != gRogueSaveBlock->lastTrialDifficulty))
        return;

    gSpecialVar_0x8004 = gRogueSaveBlock->lastTrialId;
    gSpecialVar_0x8005 = gRogueSaveBlock->lastTrialDifficulty;
    gSpecialVar_0x8006 = gRogueSaveBlock->lastTrialPokedexVariant;
    gSpecialVar_Result = TRUE;
}

void RogueTrial_CanUseAttendant(void)
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

bool8 RogueTrial_ApplyFixedStartingParty(u8 level)
{
    const struct RogueTrialDefinition *trial = RogueTrial_GetDefinition(gRogueRun.trialState.trialId);
    u8 i;

    if (trial == NULL || trial->fixedStartingPartyCount == 0)
        return FALSE;

    AGB_ASSERT(trial->fixedStartingPartyCount <= PARTY_SIZE);
    AGB_ASSERT(trial->fixedStartingParty != NULL);
    ZeroPlayerPartyMons();

    for (i = 0; i < trial->fixedStartingPartyCount; ++i)
    {
        AGB_ASSERT(trial->fixedStartingParty[i] != SPECIES_NONE);
        CreateMon(&gPlayerParty[i], trial->fixedStartingParty[i], level, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    }

    CalculatePlayerPartyCount();
    return TRUE;
}
