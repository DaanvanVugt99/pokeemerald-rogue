#include "global.h"
#include "main.h"
#include "battle.h"
#include "battle_anim.h"
#include "battle_main.h"
#include "frontier_util.h"
#include "battle_message.h"
#include "battle_tent.h"
#include "battle_factory.h"
#include "bg.h"
#include "contest.h"
#include "data.h"
#include "daycare.h"
#include "decompress.h"
#include "dynamic_placeholder_text_util.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "graphics.h"
#include "international_string_util.h"
#include "item.h"
#include "item_icon.h"
#include "link.h"
#include "m4a.h"
#include "malloc.h"
#include "menu.h"
#include "menu_helpers.h"
#include "mon_markings.h"
#include "move_relearner.h"
#include "party_menu.h"
#include "palette.h"
#include "pokeball.h"
#include "pokemon.h"
#include "pokemon_debug.h"
#include "pokemon_storage_system.h"
#include "pokemon_summary_screen.h"
#include "region_map.h"
#include "scanline_effect.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "tv.h"
#include "window.h"
#include "constants/battle_move_effects.h"
#include "constants/abilities.h"
#include "constants/hold_effects.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/party_menu.h"
#include "constants/region_map_sections.h"
#include "constants/rgb.h"
#include "constants/songs.h"

#include "rogue_baked.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_quest.h"
#include "rogue_route_events.h"
#include "rogue_trials.h"

enum {
    PSS_PAGE_UNIQUE_ABILITY,
    PSS_PAGE_INFO,
    PSS_PAGE_SKILLS,
    PSS_PAGE_BATTLE_MOVES,
    PSS_PAGE_COUNT,
};

#define TYPE_ICON_SPRITE_COUNT (MAX_MON_MOVES + 1)
// for the spriteIds field in PokemonSummaryScreenData
enum
{
    SPRITE_ARR_ID_MON,
    SPRITE_ARR_ID_BALL,
    SPRITE_ARR_ID_STATUS,
    SPRITE_ARR_ID_TYPE, // 2 for mon types, 5 for move types(4 moves and 1 to learn), used interchangeably, because mon types and move types aren't shown on the same screen
    SPRITE_ARR_ID_COUNT = SPRITE_ARR_ID_TYPE + TYPE_ICON_SPRITE_COUNT
};

static EWRAM_DATA struct PokemonSummaryScreenData
{
    /*0x00*/ union {
        struct Pokemon *mons;
        struct BoxPokemon *boxMons;
    } monList;
    /*0x04*/ MainCallback callback;
    /*0x08*/ struct Sprite *markingsSprite;
    /*0x0C*/ struct Pokemon currentMon;
    /*0x70*/ struct PokeSummary
    {
        u16 species; // 0x0
        u16 species2; // 0x2
        u8 isEgg : 1; // 0x4
        u8 isShiny : 1; // 0x4
        u8 genderFlag : 1;
        u8 level; // 0x5
        u8 ailment; // 0x7
        u16 ability;
        u8 metLocation; // 0x9
        u8 metLevel; // 0xA
        u8 metGame; // 0xB
        u8 hiddenPowerType;
        u32 pid; // 0xC
        u32 exp; // 0x10
        u16 moves[MAX_MON_MOVES]; // 0x14
        u8 pp[MAX_MON_MOVES]; // 0x1C
        u16 currentHP; // 0x20
        u16 maxHP; // 0x22
        u16 atk; // 0x24
        u16 def; // 0x26
        u16 spatk; // 0x28
        u16 spdef; // 0x2A
        u16 speed; // 0x2C
        u16 HPIV; // ??
        u16 atkIV; // ??
        u16 defIV; // ??
        u16 spatkIV; // ??
        u16 spdefIV; // ??
        u16 speedIV; // ??
        u16 HPEV; // ??
        u16 atkEV; // ??
        u16 defEV; // ??
        u16 spatkEV; // ??
        u16 spdefEV; // ??
        u16 speedEV; // ??
        u16 item; // 0x2E
        u16 friendship : 8; // 0x30
        u8 OTGender; // 0x32
        u8 nature; // 0x33
        u8 ppBonuses; // 0x34
        u8 sanity; // 0x35
        u8 OTName[17]; // 0x36
        u32 OTID; // 0x48
        u8 teraType;
        u8 gigatamaxFactor;
    } summary;
    u8 mode;
    bool8 isBoxMon;
    u8 curMonIndex;
    u8 maxMonIndex;
    u8 currPageIndex;
    bool8 infoAbilityExpanded;
    u8 minPageIndex;
    u8 maxPageIndex;
    u8 currTabIndex;
    bool8 lockMonFlag; // This is used to prevent the player from changing pokemon in the move deleter select, etc, but it is not needed because the input is handled differently there
    u16 newMove;
    u8 firstMoveIndex;
    u8 secondMoveIndex;
    bool8 lockMovesFlag; // This is used to prevent the player from changing position of moves in a battle or when trading.
    u8 spriteIds[SPRITE_ARR_ID_COUNT];
    bool8 handleDeoxys;
    s16 switchCounter; // Used for various switch statement cases that decompress/load graphics or pokemon data
    u8 itemSprite;
    u8 moveView;
} *sMonSummaryScreen = NULL;

EWRAM_DATA u8 gLastViewedMonIndex = 0;
static EWRAM_DATA u8 sMoveSlotToReplace = 0;
ALIGNED(4) static EWRAM_DATA u8 sAnimDelayTaskId = 0;

// forward declarations
static bool8 LoadGraphics(void);
static void CB2_InitSummaryScreen(void);

static void CopyMonToSummaryStruct(struct Pokemon *);
static bool8 ExtractMonDataToSummaryStruct(struct Pokemon *);

static void CloseSummaryScreen(u8);

static void ChangeSummaryPokemon(u8, s8);

static s8 AdvanceMonIndex(s8);
static s8 AdvanceMultiBattleMonIndex(s8);
static bool8 IsValidToViewInMulti(struct Pokemon *);
static bool8 CanExpandInfoAbility(void);

static void DisableSummaryScrollClip(void);

static bool8 HasMoreThanOneMove(void);

static void SwapMonMoves(struct Pokemon *, u8, u8);
static void SwapBoxMonMoves(struct BoxPokemon *, u8, u8);

static bool8 CanReplaceMove(void);

static void BufferWrappedSummaryAbilityDescription(u8 *, const u8 *, u32, u8);
static const u8 *GetAbilitySummaryExpandedDescription(u16 ability);

static void BufferMonEvolutionMemo(void);

static bool8 DoesMonOTMatchOwner(void);
static bool8 DidMonComeFromGBAGames(void);



static void ResetSpriteIds(void);
static void SetSpriteInvisibility(u8, bool8);

static u16 GetSummaryUniqueAbility(void);
static u8 LoadMonGfxAndSprite(struct Pokemon *, s16 *);
static u8 CreateMonSprite(struct Pokemon *);
static void SpriteCB_Pokemon(struct Sprite *);
static void StopPokemonAnimations(void);
static void CreateMonMarkingsSprite(struct Pokemon *);

static void CreateCaughtBallSprite(struct Pokemon *);
static void CreateSetStatusSprite(void);

static void SummaryScreen_DestroyAnimDelayTask(void);
static bool8 ShouldShowMoveRemember(void);
static bool8 ShouldUsePartyMovesPrompts(void);

// const rom data
#include "data/text/move_descriptions.h"
#include "data/text/special_move_descriptions.h"
#include "data/text/nature_names.h"

const u8 *GetMoveDescription(u16 move)
{
    const u8 *description;

    if (move == MOVE_NONE || move >= MOVES_COUNT_ALL)
        return gNotDoneYetDescription;

    if (move < MOVES_COUNT)
    {
        if (gBattleMoves[move].effect == EFFECT_PLACEHOLDER)
            return gNotDoneYetDescription;
        description = gMoveDescriptionPointers[move - 1];
    }
    else
    {
        description = sSpecialMoveDescriptionPointers[move - MOVES_COUNT];
    }

    return description != NULL ? description : gNotDoneYetDescription;
}

static const struct BgTemplate sBgTemplates[] =
{
    {
        .bg = 0,
        .charBaseIndex = 0,
        .mapBaseIndex = 31,
        .screenSize = 0,
        .paletteMode = 0,
        .priority = 0,
        .baseTile = 0,
    },

};

struct TilemapCtrl
{
    const u16 *gfx;
    u16 field_4;
    u8 field_6;
    u8 field_7;
    u8 field_8;
    u8 field_9;
};

static const s8 sMultiBattleOrder[] = {0, 2, 3, 1, 4, 5};

#define INFO_ABILITY_DESC_TEXT_WIDTH (18 * 8)
#define UNIQUE_ABILITY_DESC_TEXT_WIDTH (18 * 8)
#define INFO_ABILITY_DESC_MAX_LINES 4
#define UNIQUE_ABILITY_DESC_MAX_LINES 4

#define SUMMARY_TEXT_COLOR_RED 2
#define SUMMARY_TEXT_COLOR_BLUE 3

 // This is also affected by palettes, apparently

static const u8 sText_AnomalousHeader[] = _("ANOMALOUS");
static const u8 sText_NoUniqueAbility[] = _("No unique ability.");

static const u8 sText_MemoNoFurtherEvolution[] = _("Does not evolve further.");
static const u8 sText_MemoCannotCurrentlyEvolve[] = _("Cannot currently evolve.");
static const u8 sText_MemoEvolvesAtLevel[] = _("Evolves at {LV}{STR_VAR_1}.");
static const u8 sText_MemoEvolvesAtLevelByStats[] = _("Evolves at {LV}{STR_VAR_1}\nbased on its stats.");
static const u8 sText_MemoEvolvesAtLevelByTime[] = _("Evolves at {LV}{STR_VAR_1}\ndepending on the time.");
static const u8 sText_MemoEvolvesAtLevelByNature[] = _("Evolves at {LV}{STR_VAR_1}\nbased on its nature.");
static const u8 sText_MemoEvolvesUsingItem[] = _("Evolves using\n{STR_VAR_1}.");
static const u8 sText_MemoEvolvesKnowingMove[] = _("Evolves knowing\n{STR_VAR_1}.");
static const u8 sText_MemoEvolvesKnowingMoveType[] = _("Evolves knowing a\n{STR_VAR_1}-type move.");
static const u8 sText_MemoEvolvesWithFriendship[] = _("Evolves with high\nfriendship.");
static const u8 sText_MemoEvolvesWithBeauty[] = _("Evolves with high\nBeauty.");
static const u8 sText_MemoEvolvesWhenTraded[] = _("Evolves when traded.");
static const u8 sText_MemoTwoItems[] = _("Use {STR_VAR_1} or\n{STR_VAR_2} to evolve.");
static const u8 sText_MemoLevelOrItem[] = _("Reach {LV}{STR_VAR_1} or use\n{STR_VAR_2} to evolve.");
static const u8 sText_MemoTwoLevels[] = _("Evolves at {LV}{STR_VAR_1} or {LV}{STR_VAR_2}.");
static const u8 sText_MemoManyMethods[] = _("Many evolution methods.\nSee Pokédex for details.");
static const u8 sText_MemoSeePokedex[] = _("See Pokédex evolutions.");

enum SummaryEvolutionKind
{
    SUMMARY_EVO_LEVEL,
    SUMMARY_EVO_LEVEL_STATS,
    SUMMARY_EVO_LEVEL_TIME,
    SUMMARY_EVO_LEVEL_NATURE,
    SUMMARY_EVO_ITEM,
    SUMMARY_EVO_MOVE,
    SUMMARY_EVO_MOVE_TYPE,
    SUMMARY_EVO_FRIENDSHIP,
    SUMMARY_EVO_BEAUTY,
    SUMMARY_EVO_TRADE,
    SUMMARY_EVO_OTHER,
};

struct SummaryEvolutionRoute
{
    u16 kind;
    u16 param;
};

#define TAG_MOVE_SELECTOR 30000
#define TAG_MON_STATUS 30001
#define TAG_MOVE_TYPES 30002
#define TAG_MON_MARKINGS 30003
#define TAG_SPLIT_ICONS 30004
#define TAG_COMPACT_MOVE_BADGES 30005

static const u16 sSplitIcons_Pal[] = INCBIN_U16("graphics/interface/split_icons.gbapal");
static const u32 sSplitIcons_Gfx[] = INCBIN_U32("graphics/interface/split_icons.4bpp.lz");

static const struct OamData sOamData_SplitIcons =
{
    .size = SPRITE_SIZE(16x16),
    .shape = SPRITE_SHAPE(16x16),
    .priority = 0,
};

static const struct CompressedSpriteSheet sSpriteSheet_SplitIcons =
{
    .data = sSplitIcons_Gfx,
    .size = 16*16*3/2,
    .tag = TAG_SPLIT_ICONS,
};

static const struct SpritePalette sSpritePal_SplitIcons =
{
    .data = sSplitIcons_Pal,
    .tag = TAG_SPLIT_ICONS
};

static const union AnimCmd sSpriteAnim_SplitIcon0[] =
{
    ANIMCMD_FRAME(0, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_SplitIcon1[] =
{
    ANIMCMD_FRAME(4, 0),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_SplitIcon2[] =
{
    ANIMCMD_FRAME(8, 0),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_SplitIcons[] =
{
    sSpriteAnim_SplitIcon0,
    sSpriteAnim_SplitIcon1,
    sSpriteAnim_SplitIcon2,
};

static const struct SpriteTemplate sSpriteTemplate_SplitIcons =
{
    .tileTag = TAG_SPLIT_ICONS,
    .paletteTag = TAG_SPLIT_ICONS,
    .oam = &sOamData_SplitIcons,
    .anims = sSpriteAnimTable_SplitIcons,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const struct OamData sOamData_MoveTypes =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x16),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x16),
    .tileNum = 0,
    .priority = 1,
    .paletteNum = 0,
    .affineParam = 0,
};

#define TERA_TYPE_OFFSET (NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_TOUGH + 1)

static const union AnimCmd sSpriteAnim_TypeNormal[] = {
    ANIMCMD_FRAME(TYPE_NORMAL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFighting[] = {
    ANIMCMD_FRAME(TYPE_FIGHTING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFlying[] = {
    ANIMCMD_FRAME(TYPE_FLYING * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePoison[] = {
    ANIMCMD_FRAME(TYPE_POISON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGround[] = {
    ANIMCMD_FRAME(TYPE_GROUND * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeRock[] = {
    ANIMCMD_FRAME(TYPE_ROCK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeBug[] = {
    ANIMCMD_FRAME(TYPE_BUG * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGhost[] = {
    ANIMCMD_FRAME(TYPE_GHOST * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeSteel[] = {
    ANIMCMD_FRAME(TYPE_STEEL * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeMystery[] = {
    ANIMCMD_FRAME(TYPE_MYSTERY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFire[] = {
    ANIMCMD_FRAME(TYPE_FIRE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeWater[] = {
    ANIMCMD_FRAME(TYPE_WATER * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeGrass[] = {
    ANIMCMD_FRAME(TYPE_GRASS * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeElectric[] = {
    ANIMCMD_FRAME(TYPE_ELECTRIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypePsychic[] = {
    ANIMCMD_FRAME(TYPE_PSYCHIC * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeIce[] = {
    ANIMCMD_FRAME(TYPE_ICE * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDragon[] = {
    ANIMCMD_FRAME(TYPE_DRAGON * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeDark[] = {
    ANIMCMD_FRAME(TYPE_DARK * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeFairy[] = {
    ANIMCMD_FRAME(TYPE_FAIRY * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TypeStellar[] = {
    ANIMCMD_FRAME(TYPE_STELLAR * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCool[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_COOL + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryBeauty[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_BEAUTY + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryCute[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_CUTE + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategorySmart[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_SMART + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_CategoryTough[] = {
    ANIMCMD_FRAME((CONTEST_CATEGORY_TOUGH + NUMBER_OF_MON_TYPES) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd sSpriteAnim_TeraTypeNormal[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_NORMAL) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFighting[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_FIGHTING) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFlying[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_FLYING) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypePoison[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_POISON) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGround[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_GROUND) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeRock[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_ROCK) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeBug[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_BUG) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGhost[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_GHOST) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeSteel[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_STEEL) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeMystery[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_MYSTERY) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFire[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_FIRE) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeWater[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_WATER) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeGrass[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_GRASS) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeElectric[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_ELECTRIC) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypePsychic[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_PSYCHIC) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeIce[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_ICE) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeDragon[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_DRAGON) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeDark[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_DARK) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeFairy[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_FAIRY) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_TeraTypeStellar[] = {
    ANIMCMD_FRAME((TERA_TYPE_OFFSET + TYPE_STELLAR) * 8, 0, FALSE, FALSE),
    ANIMCMD_END
};

static const union AnimCmd *const sSpriteAnimTable_MoveTypes[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT + NUMBER_OF_MON_TYPES] = {
    sSpriteAnim_TypeNormal,
    sSpriteAnim_TypeFighting,
    sSpriteAnim_TypeFlying,
    sSpriteAnim_TypePoison,
    sSpriteAnim_TypeGround,
    sSpriteAnim_TypeRock,
    sSpriteAnim_TypeBug,
    sSpriteAnim_TypeGhost,
    sSpriteAnim_TypeSteel,
    sSpriteAnim_TypeMystery,
    sSpriteAnim_TypeFire,
    sSpriteAnim_TypeWater,
    sSpriteAnim_TypeGrass,
    sSpriteAnim_TypeElectric,
    sSpriteAnim_TypePsychic,
    sSpriteAnim_TypeIce,
    sSpriteAnim_TypeDragon,
    sSpriteAnim_TypeDark,
    sSpriteAnim_TypeFairy,
    sSpriteAnim_TypeStellar,
    sSpriteAnim_CategoryCool,
    sSpriteAnim_CategoryBeauty,
    sSpriteAnim_CategoryCute,
    sSpriteAnim_CategorySmart,
    sSpriteAnim_CategoryTough,
    sSpriteAnim_TeraTypeNormal,
    sSpriteAnim_TeraTypeFighting,
    sSpriteAnim_TeraTypeFlying,
    sSpriteAnim_TeraTypePoison,
    sSpriteAnim_TeraTypeGround,
    sSpriteAnim_TeraTypeRock,
    sSpriteAnim_TeraTypeBug,
    sSpriteAnim_TeraTypeGhost,
    sSpriteAnim_TeraTypeSteel,
    sSpriteAnim_TeraTypeMystery,
    sSpriteAnim_TeraTypeFire,
    sSpriteAnim_TeraTypeWater,
    sSpriteAnim_TeraTypeGrass,
    sSpriteAnim_TeraTypeElectric,
    sSpriteAnim_TeraTypePsychic,
    sSpriteAnim_TeraTypeIce,
    sSpriteAnim_TeraTypeDragon,
    sSpriteAnim_TeraTypeDark,
    sSpriteAnim_TeraTypeFairy,
    sSpriteAnim_TeraTypeStellar,
};

const struct CompressedSpriteSheet gSpriteSheet_MoveTypes =
{
    .data = gMoveTypes_Gfx,
    .size = (NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT + NUMBER_OF_MON_TYPES) * 0x100,
    .tag = TAG_MOVE_TYPES
};
const struct SpriteTemplate gSpriteTemplate_MoveTypes =
{
    .tileTag = TAG_MOVE_TYPES,
    .paletteTag = TAG_MOVE_TYPES,
    .oam = &sOamData_MoveTypes,
    .anims = sSpriteAnimTable_MoveTypes,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};

static const u8 sMoveTypeToOamPaletteNum[NUMBER_OF_MON_TYPES + CONTEST_CATEGORIES_COUNT + NUMBER_OF_MON_TYPES] =
{
    [TYPE_NORMAL] = 13,
    [TYPE_FIGHTING] = 13,
    [TYPE_FLYING] = 14,
    [TYPE_POISON] = 14,
    [TYPE_GROUND] = 13,
    [TYPE_ROCK] = 13,
    [TYPE_BUG] = 15,
    [TYPE_GHOST] = 14,
    [TYPE_STEEL] = 13,
    [TYPE_MYSTERY] = 15,
    [TYPE_FIRE] = 13,
    [TYPE_WATER] = 14,
    [TYPE_GRASS] = 15,
    [TYPE_ELECTRIC] = 13,
    [TYPE_PSYCHIC] = 14,
    [TYPE_ICE] = 14,
    [TYPE_DRAGON] = 15,
    [TYPE_DARK] = 13,
    [TYPE_FAIRY] = 14,
    [TYPE_STELLAR] = 15,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_COOL] = 13,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_BEAUTY] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_CUTE] = 14,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_SMART] = 15,
    [NUMBER_OF_MON_TYPES + CONTEST_CATEGORY_TOUGH] = 13,
    [TERA_TYPE_OFFSET + TYPE_NORMAL] = 13,
    [TERA_TYPE_OFFSET + TYPE_FIGHTING] = 13,
    [TERA_TYPE_OFFSET + TYPE_FLYING] = 14,
    [TERA_TYPE_OFFSET + TYPE_POISON] = 14,
    [TERA_TYPE_OFFSET + TYPE_GROUND] = 13,
    [TERA_TYPE_OFFSET + TYPE_ROCK] = 13,
    [TERA_TYPE_OFFSET + TYPE_BUG] = 15,
    [TERA_TYPE_OFFSET + TYPE_GHOST] = 14,
    [TERA_TYPE_OFFSET + TYPE_STEEL] = 13,
    [TERA_TYPE_OFFSET + TYPE_MYSTERY] = 15,
    [TERA_TYPE_OFFSET + TYPE_FIRE] = 13,
    [TERA_TYPE_OFFSET + TYPE_WATER] = 14,
    [TERA_TYPE_OFFSET + TYPE_GRASS] = 15,
    [TERA_TYPE_OFFSET + TYPE_ELECTRIC] = 13,
    [TERA_TYPE_OFFSET + TYPE_PSYCHIC] = 14,
    [TERA_TYPE_OFFSET + TYPE_ICE] = 14,
    [TERA_TYPE_OFFSET + TYPE_DRAGON] = 15,
    [TERA_TYPE_OFFSET + TYPE_DARK] = 13,
    [TERA_TYPE_OFFSET + TYPE_FAIRY] = 14,
    [TERA_TYPE_OFFSET + TYPE_STELLAR] = 15,
};

// All except left, middle and right are unused

static const struct OamData sOamData_StatusCondition =
{
    .y = 0,
    .affineMode = ST_OAM_AFFINE_OFF,
    .objMode = ST_OAM_OBJ_NORMAL,
    .mosaic = FALSE,
    .bpp = ST_OAM_4BPP,
    .shape = SPRITE_SHAPE(32x8),
    .x = 0,
    .matrixNum = 0,
    .size = SPRITE_SIZE(32x8),
    .tileNum = 0,
    .priority = 3,
    .paletteNum = 0,
    .affineParam = 0,
};
static const union AnimCmd sSpriteAnim_StatusPoison[] = {
    ANIMCMD_FRAME(0, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusParalyzed[] = {
    ANIMCMD_FRAME(4, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusSleep[] = {
    ANIMCMD_FRAME(8, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFrozen[] = {
    ANIMCMD_FRAME(12, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusBurn[] = {
    ANIMCMD_FRAME(16, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusPokerus[] = {
    ANIMCMD_FRAME(20, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFaint[] = {
    ANIMCMD_FRAME(24, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd sSpriteAnim_StatusFrostbite[] = {
    ANIMCMD_FRAME(28, 0, FALSE, FALSE),
    ANIMCMD_END
};
static const union AnimCmd *const sSpriteAnimTable_StatusCondition[] = {
    sSpriteAnim_StatusPoison,
    sSpriteAnim_StatusParalyzed,
    sSpriteAnim_StatusSleep,
    sSpriteAnim_StatusFrozen,
    sSpriteAnim_StatusBurn,
    sSpriteAnim_StatusPokerus,
    sSpriteAnim_StatusFaint,
    sSpriteAnim_StatusFrostbite,
};
static const struct CompressedSpriteSheet sStatusIconsSpriteSheet =
{
    .data = gStatusGfx_Icons,
    .size = 0x400,
    .tag = TAG_MON_STATUS
};
static const struct CompressedSpritePalette sStatusIconsSpritePalette =
{
    .data = gStatusPal_Icons,
    .tag = TAG_MON_STATUS
};
static const struct SpriteTemplate sSpriteTemplate_StatusCondition =
{
    .tileTag = TAG_MON_STATUS,
    .paletteTag = TAG_MON_STATUS,
    .oam = &sOamData_StatusCondition,
    .anims = sSpriteAnimTable_StatusCondition,
    .images = NULL,
    .affineAnims = gDummySpriteAffineAnimTable,
    .callback = SpriteCallbackDummy
};
static const u16 sMarkings_Pal[] = INCBIN_U16("graphics/summary_screen/markings.gbapal");

// code
// Callbacks for rogue_pokedex

void LoadMoveSplitSpritesheetAndPalette()
{
    if (GetSpriteTileStartByTag(TAG_SPLIT_ICONS) == 0xFFFF)
        LoadCompressedSpriteSheet(&sSpriteSheet_SplitIcons);
    if (IndexOfSpritePaletteTag(TAG_SPLIT_ICONS) == 0xFF)
        LoadSpritePalette(&sSpritePal_SplitIcons);
}

u8 CreateMoveSplitIcon(u32 split, u8 x, u8 y)
{
    u8 spriteId = CreateSprite(&sSpriteTemplate_SplitIcons, x + 8, y + 8, 0);
    if(spriteId != SPRITE_NONE)
    {
        StartSpriteAnim(&gSprites[spriteId], split);
    }

    return spriteId;
}

void DestroyMoveSplitIcon(u8 spriteId)
{
    if (spriteId < MAX_SPRITES && gSprites[spriteId].inUse)
        DestroySprite(&gSprites[spriteId]);
}

void ShowPokemonSummaryScreen(u8 mode, void *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void))
{
    sMonSummaryScreen = AllocZeroed(sizeof(*sMonSummaryScreen));
    sMonSummaryScreen->mode = mode;
    sMonSummaryScreen->monList.mons = mons;
    sMonSummaryScreen->curMonIndex = monIndex;
    sMonSummaryScreen->maxMonIndex = maxMonIndex;
    sMonSummaryScreen->callback = callback;

    if (mode == SUMMARY_MODE_BOX)
        sMonSummaryScreen->isBoxMon = TRUE;
    else
        sMonSummaryScreen->isBoxMon = FALSE;

    sMonSummaryScreen->currPageIndex  = PSS_PAGE_COUNT;

    switch (mode)
    {
    case SUMMARY_MODE_NORMAL:
    case SUMMARY_MODE_BOX:
        sMonSummaryScreen->minPageIndex = PSS_PAGE_UNIQUE_ABILITY;
        sMonSummaryScreen->maxPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->currPageIndex = PSS_PAGE_INFO;
        break;
    case SUMMARY_MODE_LOCK_MOVES:
        sMonSummaryScreen->minPageIndex = PSS_PAGE_UNIQUE_ABILITY;
        sMonSummaryScreen->maxPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->currPageIndex = PSS_PAGE_INFO;
        sMonSummaryScreen->lockMovesFlag = TRUE;
        break;
    case SUMMARY_MODE_SELECT_MOVE:
        sMonSummaryScreen->minPageIndex = PSS_PAGE_UNIQUE_ABILITY;
        sMonSummaryScreen->maxPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->currPageIndex = PSS_PAGE_BATTLE_MOVES;
        sMonSummaryScreen->lockMonFlag = TRUE;
        break;
    }


    if(sMonSummaryScreen->currPageIndex == PSS_PAGE_COUNT)
        sMonSummaryScreen->currPageIndex = sMonSummaryScreen->minPageIndex;
    sMonSummaryScreen->currTabIndex = 0;

    SummaryScreen_SetAnimDelayTaskId(TASK_NONE);

    if (gMonSpritesGfxPtr == NULL)
        CreateMonSpritesGfxManager(MON_SPR_GFX_MANAGER_A, MON_SPR_GFX_MODE_NORMAL);

    SetMainCallback2(CB2_InitSummaryScreen);
}

void ShowSelectMovePokemonSummaryScreen(struct Pokemon *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void), u16 newMove)
{
    ShowPokemonSummaryScreen(SUMMARY_MODE_SELECT_MOVE, mons, monIndex, maxMonIndex, callback);
    sMonSummaryScreen->newMove = newMove;
}

void ShowPartyPokemonSummaryScreenOnMovesPage(u8 monIndex, MainCallback callback)
{
    ShowPokemonSummaryScreen(SUMMARY_MODE_NORMAL, gPlayerParty, monIndex, gPlayerPartyCount - 1, callback);
    sMonSummaryScreen->currPageIndex = PSS_PAGE_BATTLE_MOVES;
}

void ShowPokemonSummaryScreenHandleDeoxys(u8 mode, struct BoxPokemon *mons, u8 monIndex, u8 maxMonIndex, void (*callback)(void))
{
    ShowPokemonSummaryScreen(mode, mons, monIndex, maxMonIndex, callback);
    sMonSummaryScreen->handleDeoxys = TRUE;
}

static void MainCB2(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    DoScheduledBgTilemapCopiesToVram();
    UpdatePaletteFade();
}

static void VBlank(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

static void CB2_InitSummaryScreen(void)
{
    while (MenuHelpers_ShouldWaitForLinkRecv() != TRUE && LoadGraphics() != TRUE && MenuHelpers_IsLinkActive() != TRUE);
}

static void DrawSummary(u8 previousPage, bool8 previousDetail);
static void Task_SummaryInput(u8 taskId);
static void LoadSummaryPokemon(void);

static bool8 LoadGraphics(void)
{
    static const struct WindowTemplate windows[] = {
        { .bg = 0, .width = 30, .height = 20, .paletteNum = 1, .baseBlock = 1 },
        DUMMY_WIN_TEMPLATE,
    };
    SetVBlankHBlankCallbacksToNull();
    ResetVramOamAndBgCntRegs();
    ClearScheduledBgCopiesToVram();
    ScanlineEffect_Stop();
    ResetPaletteFade();
    ResetSpriteData();
    FreeAllSpritePalettes();
    ResetBgsAndClearDma3BusyFlags(0);
    InitBgsFromTemplates(0, sBgTemplates, ARRAY_COUNT(sBgTemplates));
    ResetAllBgsCoordinates();
    InitWindows(windows);
    DeactivateAllTextPrinters();
    DisableSummaryScrollClip();
    SetGpuReg(REG_OFFSET_BLDCNT, 0);
    SetGpuReg(REG_OFFSET_DISPCNT, DISPCNT_OBJ_ON | DISPCNT_OBJ_1D_MAP);
    ShowBg(0);
    LoadSummaryPokemon();
    PutWindowTilemap(0);
    CopyWindowToVram(0, COPYWIN_MAP);
    DrawSummary(PSS_PAGE_COUNT, FALSE);
    BeginNormalPaletteFade(PALETTES_ALL, 0, 16, 0, RGB_BLACK);
    SetVBlankCallback(VBlank);
    SetMainCallback2(MainCB2);
    CreateTask(Task_SummaryInput, 0);
    return TRUE;
}

static void CopyMonToSummaryStruct(struct Pokemon *mon)
{
    if (!sMonSummaryScreen->isBoxMon)
    {
        struct Pokemon *partyMon = sMonSummaryScreen->monList.mons;
        Rogue_CorrectMonDetails(&partyMon[sMonSummaryScreen->curMonIndex], 1);
        *mon = partyMon[sMonSummaryScreen->curMonIndex];
    }
    else
    {
        struct BoxPokemon *boxMon = sMonSummaryScreen->monList.boxMons;
        Rogue_CorrectBoxMonDetails(&boxMon[sMonSummaryScreen->curMonIndex]);
        BoxMonToMon(&boxMon[sMonSummaryScreen->curMonIndex], mon);
    }
}

static bool8 ExtractMonDataToSummaryStruct(struct Pokemon *mon)
{
    u32 i;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;

    // Spread the data extraction over multiple frames.
    switch (sMonSummaryScreen->switchCounter)
    {
    case 0:
        sum->species = GetMonData(mon, MON_DATA_SPECIES);
        sum->species2 = GetMonData(mon, MON_DATA_SPECIES_OR_EGG);
        sum->isShiny = GetMonData(mon, MON_DATA_IS_SHINY);
        sum->genderFlag = GetMonData(mon, MON_DATA_GENDER_FLAG);
        sum->exp = GetMonData(mon, MON_DATA_EXP);
        sum->level = GetMonData(mon, MON_DATA_LEVEL);
        // Run Ability overrides belong to the original party mon, not this
        // screen's copy. Box and preview mons keep their native Ability.
        sum->ability = GetMonAbility(sMonSummaryScreen->isBoxMon
            ? mon : &sMonSummaryScreen->monList.mons[sMonSummaryScreen->curMonIndex]);
        sum->item = GetMonData(mon, MON_DATA_HELD_ITEM);
        sum->pid = GetMonData(mon, MON_DATA_PERSONALITY);
        sum->sanity = GetMonData(mon, MON_DATA_SANITY_IS_BAD_EGG);
        sum->hiddenPowerType = CalcMonHiddenPowerType(mon);

        if (sum->sanity)
            sum->isEgg = TRUE;
        else
            sum->isEgg = GetMonData(mon, MON_DATA_IS_EGG);

        break;
    case 1:
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            sum->moves[i] = GetMonData(mon, MON_DATA_MOVE1+i);
            sum->pp[i] = GetMonData(mon, MON_DATA_PP1+i);
        }
        sum->ppBonuses = GetMonData(mon, MON_DATA_PP_BONUSES);
        break;
    case 2:
        if (sMonSummaryScreen->monList.mons == gPlayerParty || sMonSummaryScreen->mode == SUMMARY_MODE_BOX || sMonSummaryScreen->handleDeoxys == TRUE)
        {
            sum->nature = GetNature(mon);
            sum->currentHP = GetMonData(mon, MON_DATA_HP);
            sum->maxHP = GetMonData(mon, MON_DATA_MAX_HP);
            sum->atk = GetMonData(mon, MON_DATA_ATK);
            sum->def = GetMonData(mon, MON_DATA_DEF);
            sum->spatk = GetMonData(mon, MON_DATA_SPATK);
            sum->spdef = GetMonData(mon, MON_DATA_SPDEF);
            sum->speed = GetMonData(mon, MON_DATA_SPEED);
        }
        else
        {
            sum->nature = GetNature(mon);
            sum->currentHP = GetMonData(mon, MON_DATA_HP);
            sum->maxHP = GetMonData(mon, MON_DATA_MAX_HP);
            sum->atk = GetMonData(mon, MON_DATA_ATK2);
            sum->def = GetMonData(mon, MON_DATA_DEF2);
            sum->spatk = GetMonData(mon, MON_DATA_SPATK2);
            sum->spdef = GetMonData(mon, MON_DATA_SPDEF2);
            sum->speed = GetMonData(mon, MON_DATA_SPEED2);
        }

        sum->HPIV = GetMonData(mon, MON_DATA_HP_IV);
        sum->atkIV = GetMonData(mon, MON_DATA_ATK_IV);
        sum->defIV = GetMonData(mon, MON_DATA_DEF_IV);
        sum->spatkIV = GetMonData(mon, MON_DATA_SPATK_IV);
        sum->spdefIV = GetMonData(mon, MON_DATA_SPDEF_IV);
        sum->speedIV = GetMonData(mon, MON_DATA_SPEED_IV);

        sum->HPEV = GetMonData(mon, MON_DATA_HP_EV);
        sum->atkEV = GetMonData(mon, MON_DATA_ATK_EV);
        sum->defEV = GetMonData(mon, MON_DATA_DEF_EV);
        sum->spatkEV = GetMonData(mon, MON_DATA_SPATK_EV);
        sum->spdefEV = GetMonData(mon, MON_DATA_SPDEF_EV);
        sum->speedEV = GetMonData(mon, MON_DATA_SPEED_EV);
        break;
    case 3:
        GetMonData(mon, MON_DATA_OT_NAME, sum->OTName);
        ConvertInternationalString(sum->OTName, GetMonData(mon, MON_DATA_LANGUAGE));
        sum->ailment = GetMonAilment(mon);
        sum->OTGender = GetMonData(mon, MON_DATA_OT_GENDER);
        sum->OTID = GetMonData(mon, MON_DATA_OT_ID);
        sum->metLocation = GetMonData(mon, MON_DATA_MET_LOCATION);
        sum->metLevel = GetMonData(mon, MON_DATA_MET_LEVEL);
        sum->metGame = GetMonData(mon, MON_DATA_MET_GAME);
        sum->friendship = GetMonData(mon, MON_DATA_FRIENDSHIP);
        break;
    default:
        sum->teraType = GetMonData(mon, MON_DATA_TERA_TYPE);
        sum->gigatamaxFactor = GetMonData(mon, MON_DATA_GIGANTAMAX_FACTOR);
        return TRUE;
    }
    sMonSummaryScreen->switchCounter++;
    return FALSE;
}

static void FreeSummaryScreen(void)
{
    FreeAllWindowBuffers();
    Free(sMonSummaryScreen);
}

static void BeginCloseSummaryScreen(u8 taskId)
{
    DisableSummaryScrollClip();
    BeginNormalPaletteFade(PALETTES_ALL, 0, 0, 16, RGB_BLACK);
    gTasks[taskId].func = CloseSummaryScreen;
}

static void CloseSummaryScreen(u8 taskId)
{
    if (MenuHelpers_ShouldWaitForLinkRecv() != TRUE && !gPaletteFade.active)
    {
        SetMainCallback2(sMonSummaryScreen->callback);
        gLastViewedMonIndex = sMonSummaryScreen->curMonIndex;
        SummaryScreen_DestroyAnimDelayTask();
        ResetSpriteData();
        FreeAllSpritePalettes();
        StopCryAndClearCrySongs();
        m4aMPlayVolumeControl(&gMPlayInfo_BGM, TRACKS_ALL, 0x100);
        if (gMonSpritesGfxPtr == NULL)
            DestroyMonSpritesGfxManager(MON_SPR_GFX_MANAGER_A);
        FreeSummaryScreen();
        DestroyTask(taskId);
    }
}

static void ChangeSummaryPokemon(u8 taskId, s8 delta)
{
    s8 monId;

    if (!sMonSummaryScreen->lockMonFlag)
    {
        if (sMonSummaryScreen->isBoxMon == TRUE)
        {
            if (sMonSummaryScreen->currPageIndex != PSS_PAGE_INFO)
            {
                if (delta == 1)
                    delta = 0;
                else
                    delta = 2;
            }
            else
            {
                if (delta == 1)
                    delta = 1;
                else
                    delta = 3;
            }
            monId = AdvanceStorageMonIndex(sMonSummaryScreen->monList.boxMons, sMonSummaryScreen->curMonIndex, sMonSummaryScreen->maxMonIndex, delta);
        }
        else if (IsMultiBattle() == TRUE)
        {
            monId = AdvanceMultiBattleMonIndex(delta);
        }
        else
        {
            monId = AdvanceMonIndex(delta);
        }

        if (monId != -1)
        {
            PlaySE(SE_SELECT);
            SummaryScreen_DestroyAnimDelayTask();
            ResetSpriteData();
            FreeAllSpritePalettes();
            sMonSummaryScreen->curMonIndex = monId;
            LoadSummaryPokemon();
            DrawSummary(PSS_PAGE_COUNT, FALSE);
        }
    }
}

static s8 AdvanceMonIndex(s8 delta)
{
    struct Pokemon *mon = sMonSummaryScreen->monList.mons;

    if (sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO)
    {
        if (delta == -1 && sMonSummaryScreen->curMonIndex == 0)
            return -1;
        else if (delta == 1 && sMonSummaryScreen->curMonIndex >= sMonSummaryScreen->maxMonIndex)
            return -1;
        else
            return sMonSummaryScreen->curMonIndex + delta;
    }
    else
    {
        s8 index = sMonSummaryScreen->curMonIndex;

        do
        {
            index += delta;
            if (index < 0 || index > sMonSummaryScreen->maxMonIndex)
                return -1;
        } while (GetMonData(&mon[index], MON_DATA_IS_EGG));
        return index;
    }
}

static s8 AdvanceMultiBattleMonIndex(s8 delta)
{
    struct Pokemon *mons = sMonSummaryScreen->monList.mons;
    s8 index, arrId = 0;
    u8 i;

    for (i = 0; i < PARTY_SIZE; i++)
    {
        if (sMultiBattleOrder[i] == sMonSummaryScreen->curMonIndex)
        {
            arrId = i;
            break;
        }
    }

    while (TRUE)
    {
        const s8 *order = sMultiBattleOrder;

        arrId += delta;
        if (arrId < 0 || arrId >= PARTY_SIZE)
            return -1;
        index = order[arrId];
        if (IsValidToViewInMulti(&mons[index]) == TRUE)
            return index;
    }
}

static bool8 IsValidToViewInMulti(struct Pokemon *mon)
{
    if (GetMonData(mon, MON_DATA_SPECIES) == SPECIES_NONE)
        return FALSE;
    else if (sMonSummaryScreen->curMonIndex != 0 || !GetMonData(mon, MON_DATA_IS_EGG))
        return TRUE;
    else
        return FALSE;
}

static bool8 CanExpandInfoAbility(void)
{
    return sMonSummaryScreen->currPageIndex == PSS_PAGE_INFO
        && !sMonSummaryScreen->summary.isEgg;
}

static void DisableSummaryScrollClip(void)
{
    ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
    SetGpuReg(REG_OFFSET_WININ, 0);
    SetGpuReg(REG_OFFSET_WINOUT, 0);
}

static bool8 HasMoreThanOneMove(void)
{
    u8 i;
    for (i = 1; i < MAX_MON_MOVES; i++)
    {
        if (sMonSummaryScreen->summary.moves[i] != 0)
            return TRUE;
    }
    return FALSE;
}

static void SwapMonMoves(struct Pokemon *mon, u8 moveIndex1, u8 moveIndex2)
{
    struct PokeSummary* summary = &sMonSummaryScreen->summary;

    u16 move1 = summary->moves[moveIndex1];
    u16 move2 = summary->moves[moveIndex2];
    u8 move1pp = summary->pp[moveIndex1];
    u8 move2pp = summary->pp[moveIndex2];
    u8 ppBonuses = summary->ppBonuses;

    // Calculate PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetMonData(mon, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetMonData(mon, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetMonData(mon, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetMonData(mon, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);

    summary->moves[moveIndex1] = move2;
    summary->moves[moveIndex2] = move1;

    summary->pp[moveIndex1] = move2pp;
    summary->pp[moveIndex2] = move1pp;

    summary->ppBonuses = ppBonuses;
}

static void SwapBoxMonMoves(struct BoxPokemon *mon, u8 moveIndex1, u8 moveIndex2)
{
    struct PokeSummary* summary = &sMonSummaryScreen->summary;

    u16 move1 = summary->moves[moveIndex1];
    u16 move2 = summary->moves[moveIndex2];
    u8 move1pp = summary->pp[moveIndex1];
    u8 move2pp = summary->pp[moveIndex2];
    u8 ppBonuses = summary->ppBonuses;

    // Calculate PP bonuses
    u8 ppUpMask1 = gPPUpGetMask[moveIndex1];
    u8 ppBonusMove1 = (ppBonuses & ppUpMask1) >> (moveIndex1 * 2);
    u8 ppUpMask2 = gPPUpGetMask[moveIndex2];
    u8 ppBonusMove2 = (ppBonuses & ppUpMask2) >> (moveIndex2 * 2);
    ppBonuses &= ~ppUpMask1;
    ppBonuses &= ~ppUpMask2;
    ppBonuses |= (ppBonusMove1 << (moveIndex2 * 2)) + (ppBonusMove2 << (moveIndex1 * 2));

    // Swap the moves
    SetBoxMonData(mon, MON_DATA_MOVE1 + moveIndex1, &move2);
    SetBoxMonData(mon, MON_DATA_MOVE1 + moveIndex2, &move1);
    SetBoxMonData(mon, MON_DATA_PP1 + moveIndex1, &move2pp);
    SetBoxMonData(mon, MON_DATA_PP1 + moveIndex2, &move1pp);
    SetBoxMonData(mon, MON_DATA_PP_BONUSES, &ppBonuses);

    summary->moves[moveIndex1] = move2;
    summary->moves[moveIndex2] = move1;

    summary->pp[moveIndex1] = move2pp;
    summary->pp[moveIndex2] = move1pp;

    summary->ppBonuses = ppBonuses;
}

static bool8 CanReplaceMove(void)
{
    if (sMonSummaryScreen->firstMoveIndex == MAX_MON_MOVES
        || sMonSummaryScreen->newMove == MOVE_NONE
        || IsMoveHM(sMonSummaryScreen->summary.moves[sMonSummaryScreen->firstMoveIndex]) != TRUE)
        return TRUE;
    else
        return FALSE;
}

// This redraws the power/accuracy window when the player scrolls out of the "HM Moves can't be forgotten" message

u8 GetMoveSlotToReplace(void)
{
    return sMoveSlotToReplace;
}

// Toggles the "Cancel" window that appears when selecting a move

static u8 SelectStatColor(u16 statType)
{
    u8 result = ModifyStatByNature(sMonSummaryScreen->summary.nature, 100, statType);
    return result > 100 ? SUMMARY_TEXT_COLOR_RED : (result < 100 ? SUMMARY_TEXT_COLOR_BLUE : 1);
}

static bool8 ShouldUsePartyMovesPrompts(void)
{
    return sMonSummaryScreen->mode == SUMMARY_MODE_NORMAL
        && !sMonSummaryScreen->isBoxMon
        && !sMonSummaryScreen->lockMovesFlag
        && sMonSummaryScreen->monList.mons == gPlayerParty
        && !sMonSummaryScreen->summary.isEgg
        && !InBattleFactory()
        && !InSlateportBattleTent()
        && !Rogue_IsCatchingContestActive();
}

static bool8 ShouldShowMoveRemember(void)
{
    return ShouldUsePartyMovesPrompts()
        && HasLevelUpMovesToRelearn(&sMonSummaryScreen->currentMon);
}

static u8 const sText_UniqueMon[] = _("Unique {PKMN}");
static u8 const sText_HoFMarker[] = _("HoF");

static u8 const sText_GmaxFactor[] = _("GMAX");

static bool32 HasAccessToGmaxForm(u16 species)
{
    u32 i;
    struct FormChange formChange;

    for (i = 0; TRUE; i++)
    {
        Rogue_ModifyFormChange(species, i, &formChange);

        if(formChange.method == FORM_CHANGE_TERMINATOR)
            break;

        if(formChange.method == FORM_CHANGE_BATTLE_GIGANTAMAX)
            return TRUE;
    }

    return FALSE;
}

static u16 GetSummaryUniqueAbility(void)
{
    if (sMonSummaryScreen->summary.isEgg)
        return ABILITY_NONE;

    return GetUniqueAbilityBySpeciesAndOtId(sMonSummaryScreen->summary.species, sMonSummaryScreen->summary.OTID);
}

static const u8 *GetAbilitySummaryExpandedDescription(u16 ability)
{
    if (ability < ABILITIES_COUNT && gAbilityLongDescriptionPointers[ability] != NULL)
        return gAbilityLongDescriptionPointers[ability];

    return gAbilityDescriptionPointers[ability];
}

static void AppendSummaryAbilityDescriptionEllipsis(u8 *text, u32 maxWidth)
{
    static const u8 sEllipsis[] = {CHAR_ELLIPSIS, EOS};
    u8 *cursor;
    u8 *lineStart = text;
    u8 *lastSpace = NULL;

    for (cursor = text; *cursor != EOS; cursor++)
    {
        if (*cursor == CHAR_NEWLINE)
            lineStart = cursor + 1;
    }

    if (GetStringWidth(FONT_NORMAL, lineStart, 0) + GetStringWidth(FONT_NORMAL, sEllipsis, 0) <= maxWidth)
    {
        StringAppend(text, sEllipsis);
        return;
    }

    for (cursor = lineStart; *cursor != EOS; cursor++)
    {
        if (*cursor == CHAR_SPACE)
            lastSpace = cursor;
    }

    while (lastSpace != NULL)
    {
        *lastSpace = EOS;
        if (GetStringWidth(FONT_NORMAL, lineStart, 0) + GetStringWidth(FONT_NORMAL, sEllipsis, 0) <= maxWidth)
        {
            StringAppend(text, sEllipsis);
            return;
        }

        lastSpace = NULL;
        for (cursor = lineStart; *cursor != EOS; cursor++)
        {
            if (*cursor == CHAR_SPACE)
                lastSpace = cursor;
        }
    }
}

static void LimitSummaryAbilityDescriptionLines(u8 *text, u8 maxLines, u32 maxWidth)
{
    u8 lineCount = 1;
    u8 *cursor = text;

    if (maxLines == 0)
    {
        text[0] = EOS;
        return;
    }

    while (*cursor != EOS)
    {
        if (*cursor == CHAR_NEWLINE)
        {
            lineCount++;
            if (lineCount > maxLines)
            {
                *cursor = EOS;
                AppendSummaryAbilityDescriptionEllipsis(text, maxWidth);
                return;
            }
        }
        cursor++;
    }
}

static void BufferWrappedSummaryAbilityDescription(u8 *dst, const u8 *src, u32 maxWidth, u8 maxLines)
{
    u8 currentLine[256];
    u8 word[128];
    u8 candidate[256];
    const u8 *cursor = src;
    int len;

    dst[0] = EOS;
    currentLine[0] = EOS;

    while (*cursor != EOS)
    {
        while (*cursor == CHAR_SPACE)
            cursor++;

        if (*cursor == EOS)
            break;

        if (*cursor == CHAR_NEWLINE)
        {
            if (dst[0] != EOS)
            {
                len = StringLength(dst);
                dst[len++] = CHAR_NEWLINE;
                dst[len] = EOS;
            }
            StringAppend(dst, currentLine);
            currentLine[0] = EOS;
            cursor++;
            continue;
        }

        len = 0;
        while (*cursor != EOS && *cursor != CHAR_SPACE && *cursor != CHAR_NEWLINE)
            word[len++] = *cursor++;
        word[len] = EOS;

        if (currentLine[0] == EOS)
        {
            StringCopy(currentLine, word);
        }
        else
        {
            StringCopy(candidate, currentLine);
            len = StringLength(candidate);
            candidate[len++] = CHAR_SPACE;
            candidate[len] = EOS;
            StringAppend(candidate, word);

            if (GetStringWidth(FONT_NORMAL, candidate, 0) <= maxWidth)
            {
                StringCopy(currentLine, candidate);
            }
            else
            {
                len = StringLength(dst);
                if (dst[0] != EOS)
                    dst[len++] = CHAR_NEWLINE;
                dst[len] = EOS;
                StringAppend(dst, currentLine);
                StringCopy(currentLine, word);
            }
        }

        if (GetStringWidth(FONT_NORMAL, currentLine, 0) > maxWidth)
        {
            u8 chunk[256];
            int chunkLen = 0;
            const u8 *wordCursor = currentLine;

            currentLine[0] = EOS;
            while (*wordCursor != EOS)
            {
                chunk[chunkLen++] = *wordCursor++;
                chunk[chunkLen] = EOS;
                if (GetStringWidth(FONT_NORMAL, chunk, 0) > maxWidth)
                {
                    chunk[--chunkLen] = EOS;
                    len = StringLength(dst);
                    if (dst[0] != EOS)
                        dst[len++] = CHAR_NEWLINE;
                    dst[len] = EOS;
                    StringAppend(dst, chunk);

                    chunk[0] = wordCursor[-1];
                    chunk[1] = EOS;
                    chunkLen = 1;
                }
            }
            StringCopy(currentLine, chunk);
        }
    }

    if (currentLine[0] != EOS)
    {
        len = StringLength(dst);
        if (dst[0] != EOS)
            dst[len++] = CHAR_NEWLINE;
        dst[len] = EOS;
        StringAppend(dst, currentLine);
    }

    LimitSummaryAbilityDescriptionLines(dst, maxLines, maxWidth);
}

static bool8 IsEvolutionApplicableToSummaryMon(const struct Evolution *evo)
{
    u8 gender = GetGenderForSpecies(sMonSummaryScreen->summary.species, sMonSummaryScreen->summary.genderFlag);

    switch (evo->method)
    {
    case EVO_LEVEL_MALE:
    case EVO_ITEM_MALE:
        return gender == MON_MALE;
    case EVO_LEVEL_FEMALE:
    case EVO_ITEM_FEMALE:
        return gender == MON_FEMALE;
    default:
        return TRUE;
    }
}

static struct SummaryEvolutionRoute GetSummaryEvolutionRoute(const struct Evolution *evo)
{
    struct SummaryEvolutionRoute route = { SUMMARY_EVO_OTHER, evo->method };

    switch (evo->method)
    {
    case EVO_LEVEL:
    case EVO_LEVEL_MALE:
    case EVO_LEVEL_FEMALE:
    case EVO_LEVEL_SILCOON:
    case EVO_LEVEL_CASCOON:
    case EVO_LEVEL_NINJASK:
    case EVO_LEVEL_SHEDINJA:
#ifdef ROGUE_EXPANSION
    case EVO_LEVEL_TWO_SEGMENT:
    case EVO_LEVEL_THREE_SEGMENT:
    case EVO_LEVEL_FAMILY_OF_THREE:
    case EVO_LEVEL_FAMILY_OF_FOUR:
#endif
        route.kind = SUMMARY_EVO_LEVEL;
        route.param = evo->param;
        break;
    case EVO_LEVEL_ATK_GT_DEF:
    case EVO_LEVEL_ATK_EQ_DEF:
    case EVO_LEVEL_ATK_LT_DEF:
        route.kind = SUMMARY_EVO_LEVEL_STATS;
        route.param = evo->param;
        break;
    case EVO_LEVEL_DAY:
    case EVO_LEVEL_NIGHT:
#ifdef ROGUE_EXPANSION
    case EVO_LEVEL_DUSK:
#endif
        route.kind = SUMMARY_EVO_LEVEL_TIME;
        route.param = evo->param;
        break;
#ifdef ROGUE_EXPANSION
    case EVO_LEVEL_NATURE_AMPED:
    case EVO_LEVEL_NATURE_LOW_KEY:
        route.kind = SUMMARY_EVO_LEVEL_NATURE;
        route.param = evo->param;
        break;
    case EVO_LEVEL_30_NATURE:
        route.kind = SUMMARY_EVO_LEVEL_NATURE;
        route.param = 30;
        break;
#endif
    case EVO_ITEM:
    case EVO_ITEM_MALE:
    case EVO_ITEM_FEMALE:
        route.kind = SUMMARY_EVO_ITEM;
        route.param = evo->param;
        break;
    case EVO_MOVE:
        route.kind = SUMMARY_EVO_MOVE;
        route.param = evo->param;
        break;
#ifdef ROGUE_EXPANSION
    case EVO_MOVE_TYPE:
        route.kind = SUMMARY_EVO_MOVE_TYPE;
        route.param = evo->param;
        break;
#endif
    case EVO_FRIENDSHIP:
    case EVO_FRIENDSHIP_DAY:
    case EVO_FRIENDSHIP_NIGHT:
        route.kind = SUMMARY_EVO_FRIENDSHIP;
        route.param = 0;
        break;
    case EVO_BEAUTY:
        route.kind = SUMMARY_EVO_BEAUTY;
        route.param = 0;
        break;
    case EVO_TRADE:
    case EVO_TRADE_ITEM:
        route.kind = SUMMARY_EVO_TRADE;
        route.param = 0;
        break;
    }

    return route;
}

static bool8 AreSummaryEvolutionRoutesEqual(const struct SummaryEvolutionRoute *left, const struct SummaryEvolutionRoute *right)
{
    return left->kind == right->kind && left->param == right->param;
}

static void BufferSingleSummaryEvolution(const struct SummaryEvolutionRoute *route)
{
    const u8 *text;

    switch (route->kind)
    {
    case SUMMARY_EVO_LEVEL:
        ConvertUIntToDecimalStringN(gStringVar1, route->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        text = sText_MemoEvolvesAtLevel;
        break;
    case SUMMARY_EVO_LEVEL_STATS:
        ConvertUIntToDecimalStringN(gStringVar1, route->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        text = sText_MemoEvolvesAtLevelByStats;
        break;
    case SUMMARY_EVO_LEVEL_TIME:
        ConvertUIntToDecimalStringN(gStringVar1, route->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        text = sText_MemoEvolvesAtLevelByTime;
        break;
    case SUMMARY_EVO_LEVEL_NATURE:
        ConvertUIntToDecimalStringN(gStringVar1, route->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        text = sText_MemoEvolvesAtLevelByNature;
        break;
    case SUMMARY_EVO_ITEM:
        StringCopy(gStringVar1, Rogue_GetItemName(route->param));
        text = sText_MemoEvolvesUsingItem;
        break;
    case SUMMARY_EVO_MOVE:
        StringCopy(gStringVar1, gMoveNames[route->param]);
        text = sText_MemoEvolvesKnowingMove;
        break;
    case SUMMARY_EVO_MOVE_TYPE:
        StringCopy(gStringVar1, gTypeNames[route->param]);
        text = sText_MemoEvolvesKnowingMoveType;
        break;
    case SUMMARY_EVO_FRIENDSHIP:
        text = sText_MemoEvolvesWithFriendship;
        break;
    case SUMMARY_EVO_BEAUTY:
        text = sText_MemoEvolvesWithBeauty;
        break;
    case SUMMARY_EVO_TRADE:
        text = sText_MemoEvolvesWhenTraded;
        break;
    default:
        text = sText_MemoSeePokedex;
        break;
    }

    StringExpandPlaceholders(gStringVar3, text);
    StringAppend(gStringVar4, gStringVar3);
}

static bool8 BufferTwoSummaryEvolutions(const struct SummaryEvolutionRoute *first, const struct SummaryEvolutionRoute *second)
{
    const struct SummaryEvolutionRoute *levelRoute;
    const struct SummaryEvolutionRoute *itemRoute;
    const u8 *text;

    if (first->kind == SUMMARY_EVO_ITEM && second->kind == SUMMARY_EVO_ITEM)
    {
        StringCopy(gStringVar1, Rogue_GetItemName(first->param));
        StringCopy(gStringVar2, Rogue_GetItemName(second->param));
        text = sText_MemoTwoItems;
    }
    else if ((first->kind == SUMMARY_EVO_LEVEL && second->kind == SUMMARY_EVO_ITEM)
          || (first->kind == SUMMARY_EVO_ITEM && second->kind == SUMMARY_EVO_LEVEL))
    {
        levelRoute = first->kind == SUMMARY_EVO_LEVEL ? first : second;
        itemRoute = first->kind == SUMMARY_EVO_ITEM ? first : second;
        ConvertUIntToDecimalStringN(gStringVar1, levelRoute->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        StringCopy(gStringVar2, Rogue_GetItemName(itemRoute->param));
        text = sText_MemoLevelOrItem;
    }
    else if (first->kind == SUMMARY_EVO_LEVEL && second->kind == SUMMARY_EVO_LEVEL)
    {
        ConvertUIntToDecimalStringN(gStringVar1, first->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        ConvertUIntToDecimalStringN(gStringVar2, second->param, STR_CONV_MODE_LEFT_ALIGN, 3);
        text = sText_MemoTwoLevels;
    }
    else
    {
        return FALSE;
    }

    StringExpandPlaceholders(gStringVar3, text);
    StringAppend(gStringVar4, gStringVar3);
    return TRUE;
}

static void BufferMonEvolutionMemo(void)
{
    struct SummaryEvolutionRoute routes[3];
    struct Evolution evo;
    u8 evoCount = Rogue_GetMaxEvolutionCount(sMonSummaryScreen->summary.species);
    u8 routeCount = 0;
    u8 i;
    u8 j;

    for (i = 0; i < evoCount; ++i)
    {
        struct SummaryEvolutionRoute route;

        Rogue_ModifyEvolution(sMonSummaryScreen->summary.species, i, &evo);
        Rogue_ModifyEvolution_ApplyCurses(sMonSummaryScreen->summary.species, i, &evo);

        if (evo.targetSpecies == SPECIES_NONE || !IsEvolutionApplicableToSummaryMon(&evo))
            continue;

        route = GetSummaryEvolutionRoute(&evo);
        for (j = 0; j < min(routeCount, ARRAY_COUNT(routes)); ++j)
        {
            if (AreSummaryEvolutionRoutesEqual(&routes[j], &route))
                break;
        }

        if (j != min(routeCount, ARRAY_COUNT(routes)))
            continue;

        if (routeCount < ARRAY_COUNT(routes))
            routes[routeCount] = route;
        ++routeCount;
    }

    if (routeCount == 0)
    {
        StringAppend(gStringVar4, evoCount == 0 ? sText_MemoNoFurtherEvolution : sText_MemoCannotCurrentlyEvolve);
    }
    else if (routeCount == 1)
    {
        BufferSingleSummaryEvolution(&routes[0]);
    }
    else if (routeCount == 2 && BufferTwoSummaryEvolutions(&routes[0], &routes[1]))
    {
        // The supported pair was written directly to the memo.
    }
    else
    {
        StringAppend(gStringVar4, sText_MemoManyMethods);
    }
}

static bool8 DoesMonOTMatchOwner(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    u32 trainerId;

    if (sMonSummaryScreen->monList.mons == gEnemyParty)
    {
        u8 multiID = GetMultiplayerId() ^ 1;
        trainerId = gLinkPlayers[multiID].trainerId & 0xFFFF;
        StringCopy(gStringVar1, gLinkPlayers[multiID].name);
    }
    else
    {
        trainerId = GetPlayerIDAsU32() & 0xFFFF;
        StringCopy(gStringVar1, gSaveBlock2Ptr->playerName);
    }

    if (trainerId != (sum->OTID & 0xFFFF) || StringCompareWithoutExtCtrlCodes(gStringVar1, sum->OTName))
        return FALSE;
    else
        return TRUE;
}

static bool8 DidMonComeFromGBAGames(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    if (sum->metGame > 0 && sum->metGame <= VERSION_LEAF_GREEN)
        return TRUE;
    return FALSE;
}

bool8 DidMonComeFromRSE(void)
{
    struct PokeSummary *sum = &sMonSummaryScreen->summary;
    if (sum->metGame > 0 && sum->metGame <= VERSION_EMERALD)
        return TRUE;
    return FALSE;
}

static const u8 *GetEggStateText(void)
{
    const u8 *text;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;

    if (RogueRouteEvents_IsMysteryEggCourierEgg(&sMonSummaryScreen->currentMon))
        text = gText_MysteryEggBringToDayCare;
    else if (sMonSummaryScreen->summary.sanity == TRUE)
        text = gText_EggWillTakeALongTime;
    else if (sum->friendship <= 5)
        text = gText_EggAboutToHatch;
    else if (sum->friendship <= 10)
        text = gText_EggWillHatchSoon;
    else if (sum->friendship <= 40)
        text = gText_EggWillTakeSomeTime;
    else
        text = gText_EggWillTakeALongTime;

    return text;
}

static const u8 *GetEggMemoText(void)
{
    const u8 *text;
    struct PokeSummary *sum = &sMonSummaryScreen->summary;

    if (sMonSummaryScreen->summary.sanity != 1)
    {
        if (sum->metLocation == METLOC_FATEFUL_ENCOUNTER)
            text = gText_PeculiarEggNicePlace;
        else if (DidMonComeFromGBAGames() == FALSE || DoesMonOTMatchOwner() == FALSE)
            text = gText_PeculiarEggTrade;
        else if (sum->metLocation == METLOC_SPECIAL_EGG)
            text = (DidMonComeFromRSE() == TRUE) ? gText_EggFromHotSprings : gText_EggFromTraveler;
        else
            text = gText_OddEggFoundByCouple;
    }
    else
    {
        text = gText_OddEggFoundByCouple;
    }

    return text;
}


static void ResetSpriteIds(void)
{
    u8 i;

    for (i = 0; i < ARRAY_COUNT(sMonSummaryScreen->spriteIds); i++)
        sMonSummaryScreen->spriteIds[i] = SPRITE_NONE;
}

static void SetSpriteInvisibility(u8 spriteArrayId, bool8 invisible)
{
    gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]].invisible = invisible;
}

// Callbacks for rogue_pokedex

void LoadMoveTypesSpritesheetAndPalette()
{
    LoadCompressedPalette(gMoveTypes_Pal, 0x1D0, 0x60);
    LoadCompressedSpriteSheet(&gSpriteSheet_MoveTypes);
}

u8 CreateMonTypeIcon(u16 typeId, u8 x, u8 y)
{
    u8 spriteId = CreateSprite(&gSpriteTemplate_MoveTypes, 0, 0, 2);
    if(spriteId != SPRITE_NONE)
    {
        struct Sprite *sprite = &gSprites[spriteId];
        StartSpriteAnim(sprite, typeId);
        sprite->oam.paletteNum = sMoveTypeToOamPaletteNum[typeId];
        sprite->x = x + 16;
        sprite->y = y + 8;
    }

    return spriteId;
}

void DestroyMonTypIcon(u8 spriteId)
{
    DestroySprite(&gSprites[spriteId]);
}

void SetTypeSpritePosAndPal(u8 typeId, u8 x, u8 y, u8 spriteArrayId)
{
    struct Sprite *sprite;
    if (sMonSummaryScreen->spriteIds[spriteArrayId] >= MAX_SPRITES)
        return;
    sprite = &gSprites[sMonSummaryScreen->spriteIds[spriteArrayId]];
    sprite->animPaused = FALSE;
    StartSpriteAnim(sprite, typeId);
    sprite->oam.paletteNum = sMoveTypeToOamPaletteNum[typeId];
    sprite->x = x + 16;
    sprite->y = y + 8;
    SetSpriteInvisibility(spriteArrayId, FALSE);
}

static u8 LoadMonGfxAndSprite(struct Pokemon *mon, s16 *state)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    switch (*state)
    {
    default:
        return CreateMonSprite(mon);
    case 0:
        if (gMain.inBattle)
        {
            HandleLoadSpecialPokePic(TRUE,
                                     gMonSpritesGfxPtr->sprites.ptr[B_POSITION_OPPONENT_LEFT],
                                     summary->species2,
                                     summary->pid,
                                     GetGenderForSpecies(summary->species2, summary->genderFlag));
        }
        else
        {
            if (gMonSpritesGfxPtr != NULL)
            {
                HandleLoadSpecialPokePic(TRUE,
                                         gMonSpritesGfxPtr->sprites.ptr[B_POSITION_OPPONENT_LEFT],
                                         summary->species2,
                                         summary->pid,
                                         GetGenderForSpecies(summary->species2, summary->genderFlag));
            }
            else
            {
                HandleLoadSpecialPokePic(TRUE,
                                         MonSpritesGfxManager_GetSpritePtr(MON_SPR_GFX_MANAGER_A, B_POSITION_OPPONENT_LEFT),
                                         summary->species2,
                                         summary->pid,
                                         GetGenderForSpecies(summary->species2, summary->genderFlag));
            }
        }
        (*state)++;
        return 0xFF;
    case 1:
        LoadCompressedSpritePaletteWithTag(GetMonSpritePalFromSpecies(summary->species2, GetGenderForSpecies(summary->species2, summary->genderFlag), summary->isShiny, summary->OTID), summary->species2);
        SetMultiuseSpriteTemplateToPokemon(summary->species2, B_POSITION_OPPONENT_LEFT);
        (*state)++;
        return 0xFF;
    }
}

static void PlayMonCry(void)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    if (!summary->isEgg)
    {
        if (ShouldPlayNormalMonCry(&sMonSummaryScreen->currentMon) == TRUE)
            PlayCry_ByMode(summary->species2, 0, CRY_MODE_NORMAL);
        else
            PlayCry_ByMode(summary->species2, 0, CRY_MODE_WEAK);
    }
}

static u8 CreateMonSprite(struct Pokemon *unused)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;
    u8 spriteId = CreateSprite(&gMultiuseSpriteTemplate, 40, 64, 5);

    FreeSpriteOamMatrix(&gSprites[spriteId]);
    gSprites[spriteId].data[0] = summary->species2;
    gSprites[spriteId].data[2] = 0;
    gSprites[spriteId].callback = SpriteCB_Pokemon;
    gSprites[spriteId].oam.priority = 0;

    if (!IsMonSpriteNotFlipped(summary->species2))
        gSprites[spriteId].hFlip = TRUE;
    else
        gSprites[spriteId].hFlip = FALSE;

    return spriteId;
}

static void SpriteCB_Pokemon(struct Sprite *sprite)
{
    struct PokeSummary *summary = &sMonSummaryScreen->summary;

    if (!gPaletteFade.active && sprite->data[2] != 1)
    {
        sprite->data[1] = IsMonSpriteNotFlipped(sprite->data[0]);
        PlayMonCry();
        PokemonSummaryDoMonAnimation(sprite, sprite->data[0], summary->isEgg);
    }
}

// Track and then destroy Task_PokemonSummaryAnimateAfterDelay
// Normally destroys itself but it can be interrupted before the animation starts
void SummaryScreen_SetAnimDelayTaskId(u8 taskId)
{
    sAnimDelayTaskId = taskId;
}

static void SummaryScreen_DestroyAnimDelayTask(void)
{
    if (sAnimDelayTaskId != TASK_NONE)
    {
        DestroyTask(sAnimDelayTaskId);
        sAnimDelayTaskId = TASK_NONE;
    }
}

static void StopPokemonAnimations(void)  // A subtle effect, this function stops pokemon animations when leaving the PSS
{
    u16 i;
    u16 paletteIndex;

    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].animPaused = TRUE;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].callback = SpriteCallbackDummy;
    StopPokemonAnimationDelayTask();

    paletteIndex = OBJ_PLTT_ID(gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON]].oam.paletteNum);

    for (i = 0; i < 16; i++)
    {
        u16 id = i + paletteIndex;
        gPlttBufferUnfaded[id] = gPlttBufferFaded[id];
    }
}

static void CreateMonMarkingsSprite(struct Pokemon *mon)
{
    struct Sprite *sprite = CreateMonMarkingAllCombosSprite(TAG_MON_MARKINGS, TAG_MON_MARKINGS, sMarkings_Pal);

    sMonSummaryScreen->markingsSprite = sprite;
    if (sprite != NULL)
    {
        StartSpriteAnim(sprite, GetMonData(mon, MON_DATA_MARKINGS));
        sMonSummaryScreen->markingsSprite->x = 60;
        sMonSummaryScreen->markingsSprite->y = 26;
        sMonSummaryScreen->markingsSprite->oam.priority = 1;
    }
}

static void CreateCaughtBallSprite(struct Pokemon *mon)
{
    u8 ball = ItemIdToBallId(GetMonData(mon, MON_DATA_POKEBALL));

    LoadBallGfx(ball);
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL] = CreateSprite(&gBallSpriteTemplates[ball], 16, 136, 0);
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].callback = SpriteCallbackDummy;
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].oam.priority = 3;
    if (sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE)
        gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].invisible = TRUE;
}

static void CreateSetStatusSprite(void)
{
    u8 *spriteId = &sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_STATUS];
    u8 statusAnim;

    if (*spriteId == SPRITE_NONE)
        *spriteId = CreateSprite(&sSpriteTemplate_StatusCondition, 64, 152, 0);

    statusAnim = GetMonAilment(&sMonSummaryScreen->currentMon);
    if (statusAnim != 0)
    {
        StartSpriteAnim(&gSprites[*spriteId], statusAnim - 1);
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, FALSE);
    }
    else
    {
        SetSpriteInvisibility(SPRITE_ARR_ID_STATUS, TRUE);
    }
}

enum SummaryInk {
    INK_BG = 1, INK_PANEL, INK_EDGE, INK_RULE, INK_PAPER, INK_MUTED, INK_GOLD,
    INK_BLUE, INK_PORTRAIT, INK_SELECTED, INK_GREEN, INK_RED,
};

static const u16 sSummaryPalette[16] = {
    RGB(0,0,0), RGB(4,5,5), RGB(6,7,8), RGB(2,3,3), RGB(10,11,12),
    RGB(30,30,29), RGB(23,24,25), RGB(30,24,14), RGB(21,25,28), RGB(24,25,26),
    RGB(9,8,7), RGB(20,26,20), RGB(27,18,19),
};
enum { MOVE_BROWSE, MOVE_DETAILS, MOVE_REORDER, MOVE_HM_MESSAGE };

static const struct SpriteSheet sCompactMoveBadgeSheet =
{
    .data = gCompactMoveBadges,
    .size = NUMBER_OF_MON_TYPES * COMPACT_MOVE_BADGE_BYTES,
    .tag = TAG_COMPACT_MOVE_BADGES,
};

static const struct SpritePalette sCompactMoveBadgePalette =
{
    .data = gCompactMoveBadgePalette,
    .tag = TAG_COMPACT_MOVE_BADGES,
};

static void ShowCompactTypeBadge(u8 row, u8 type, u8 x, u8 y)
{
    u8 id = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_TYPE + row];
    u16 start = GetSpriteTileStartByTag(TAG_COMPACT_MOVE_BADGES);
    u8 palette = IndexOfSpritePaletteTag(TAG_COMPACT_MOVE_BADGES);
    struct Sprite *sprite;

    if (id >= MAX_SPRITES || type >= NUMBER_OF_MON_TYPES || start == 0xFFFF || palette == 0xFF)
        return;
    sprite = &gSprites[id];
    sprite->animPaused = TRUE;
    sprite->animBeginning = FALSE;
    sprite->oam.tileNum = start + type * (COMPACT_MOVE_BADGE_BYTES / TILE_SIZE_4BPP);
    sprite->oam.paletteNum = palette;
    sprite->x = x + 16;
    sprite->y = y + 8;
    sprite->invisible = FALSE;
}

static void LoadSummaryPokemon(void)
{
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    s16 state = 0;
    u8 id;
    CopyMonToSummaryStruct(mon);
    sMonSummaryScreen->switchCounter = 0;
    while (!ExtractMonDataToSummaryStruct(mon))
        ;
    if (sMonSummaryScreen->summary.isEgg)
    {
        sMonSummaryScreen->currPageIndex = PSS_PAGE_INFO;
        sMonSummaryScreen->infoAbilityExpanded = FALSE;
    }
    ResetSpriteIds();
    do {
        id = LoadMonGfxAndSprite(mon, &state);
    } while (id == SPRITE_NONE);
    sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_MON] = id;
    gSprites[id].x = 41;
    gSprites[id].y = 68;
    CreateCaughtBallSprite(mon);
    id = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL];
    gSprites[id].x = 64;
    gSprites[id].y = 131;
    gSprites[id].oam.priority = 0;
    CreateMonMarkingsSprite(mon);
    if (sMonSummaryScreen->markingsSprite != NULL)
    {
        sMonSummaryScreen->markingsSprite->x = 56;
        sMonSummaryScreen->markingsSprite->y = 42;
        sMonSummaryScreen->markingsSprite->oam.priority = 0;
        sMonSummaryScreen->markingsSprite->invisible = !GetMonData(mon, MON_DATA_MARKINGS);
    }
    LoadCompressedSpriteSheet(&sStatusIconsSpriteSheet);
    LoadCompressedSpritePalette(&sStatusIconsSpritePalette);
    CreateSetStatusSprite();
    id = sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_STATUS];
    gSprites[id].x = 24;
    gSprites[id].y = 43;
    gSprites[id].oam.priority = 0;
    sMonSummaryScreen->itemSprite = MAX_SPRITES;
    if (sMonSummaryScreen->summary.item != ITEM_NONE)
    {
        id = AddItemIconSprite(0xD790, 0xD790, sMonSummaryScreen->summary.item);
        sMonSummaryScreen->itemSprite = id;
        if (id < MAX_SPRITES)
        {
            gSprites[id].x = 97;
            gSprites[id].y = 47;
            gSprites[id].oam.priority = 0;
        }
    }
    LoadMoveTypesSpritesheetAndPalette();
    LoadSpriteSheet(&sCompactMoveBadgeSheet);
    LoadSpritePalette(&sCompactMoveBadgePalette);
    for (u8 i = SPRITE_ARR_ID_TYPE; i < SPRITE_ARR_ID_COUNT; i++)
    {
        id = CreateSprite(&gSpriteTemplate_MoveTypes, 0, 0, 2);
        sMonSummaryScreen->spriteIds[i] = id;
        if (id < MAX_SPRITES)
        {
            gSprites[id].invisible = TRUE;
            gSprites[id].oam.priority = 0;
        }
    }
    LoadPalette(sSummaryPalette, BG_PLTT_ID(1), sizeof(sSummaryPalette));
}

static void SummaryRect(u8 x, u8 y, u8 w, u8 h, u8 color)
{
    // A 4bpp tile row fits in one word. Only the end tiles need masking.
    u32 *tiles = (u32 *)GetWindowAttribute(0, WINDOW_TILE_DATA);
    u32 fill = color * 0x11111111u;
    u32 right = min(x + w, DISPLAY_WIDTH);
    u32 bottom = min(y + h, DISPLAY_HEIGHT);
    u32 tileX, tileY;
    for (tileY = y / 8; tileY * 8 < bottom; tileY++)
    {
        u32 firstRow = max(y, tileY * 8) - tileY * 8;
        u32 endRow = min(bottom - tileY * 8, 8);
        for (tileX = x / 8; tileX * 8 < right; tileX++)
        {
            // Clear complete tiles as one BIOS fill per horizontal strip.
            if (firstRow == 0 && endRow == 8 && tileX * 8 >= x && tileX < right / 8)
            {
                u32 count = right / 8 - tileX;
                CpuFastFill(fill, tiles + (tileY * (DISPLAY_WIDTH / 8) + tileX) * 8, count * 32);
                tileX += count - 1;
                continue;
            }
            u32 firstPixel = max(x, tileX * 8) - tileX * 8;
            u32 endPixel = min(right - tileX * 8, 8);
            u32 mask = (~0u << (4 * firstPixel)) & (~0u >> (4 * (8 - endPixel)));
            u32 *tile = tiles + (tileY * (DISPLAY_WIDTH / 8) + tileX) * 8;
            u32 row;
            if (mask == ~0u)
                for (row = firstRow; row < endRow; row++)
                    tile[row] = fill;
            else
                for (row = firstRow; row < endRow; row++)
                    tile[row] = (tile[row] & ~mask) | (fill & mask);
        }
    }
}

static void SummaryFrame(u8 x, u8 y, u8 w, u8 h)
{
    SummaryRect(x, y, w, 1, INK_EDGE);
    SummaryRect(x, y+h-1, w, 1, INK_EDGE);
    SummaryRect(x, y+1, 1, h-2, INK_EDGE);
    SummaryRect(x+w-1, y+1, 1, h-2, INK_EDGE);
    SummaryRect(x+1, y+2, w-2, h-3, INK_PANEL);
    SummaryRect(x+1, y+1, w-2, 1, INK_RULE);
}

static void SummaryText(u8 x, u8 y, const u8 *text, u8 ink, u8 font)
{
    u8 colors[] = {TEXT_COLOR_TRANSPARENT, ink, INK_BG};
    AddTextPrinterParameterized4(0, font, x, y, 0, 0, colors, TEXT_SKIP_DRAW, text);
}

static void Label(u8 x, u8 y, const u8 *text, u8 ink)
{
    SummaryText(x, y, text, ink, FONT_SMALL);
}

static void RightLabel(u8 x, u8 y, const u8 *text, u8 ink)
{
    Label(x - GetStringWidth(FONT_SMALL, text, 0), y, text, ink);
}

static u8 *SummaryNumber(u8 *dst, u32 n)
{
    return ConvertUIntToDecimalStringN(dst, n, STR_CONV_MODE_LEFT_ALIGN, 7);
}

static void SummaryParagraph(u8 x, u8 y, const u8 *text, u8 width, u8 lines)
{
    u8 wrapped[512];
    BufferWrappedSummaryAbilityDescription(wrapped, text, width, lines);
    Label(x, y, wrapped, INK_PAPER);
}

static void DrawSummaryIdentity(bool8 detail)
{
    struct PokeSummary *s = &sMonSummaryScreen->summary;
    struct Pokemon *mon = &sMonSummaryScreen->currentMon;
    u16 dex = SpeciesToPokedexNum(s->species);
    SummaryFrame(3,33,76,112);
    SummaryRect(7,37,68,63,INK_PORTRAIT);
    gSprites[sMonSummaryScreen->spriteIds[SPRITE_ARR_ID_BALL]].invisible = detail || s->isEgg;
    if (detail)
        return;
    if (!s->isEgg && dex != 0xFFFF)
    {
        ConvertIntToDecimalStringN(StringCopy(gStringVar1, COMPOUND_STRING("No. ")), dex, STR_CONV_MODE_LEADING_ZEROS, dex > 999 ? 4 : 3);
        Label(9,98,gStringVar1,s->isShiny ? INK_GOLD : INK_MUTED);
    }
    if (!s->isEgg && RogueQuest_GetMonMasteryFlag(s->species))
        RightLabel(73,98,sText_HoFMarker,INK_GOLD);
    GetMonNickname(mon,gStringVar1);
    SummaryText(8,110,gStringVar1,INK_PAPER,FONT_NORMAL);
    if (!s->isEgg)
    {
        StringCopy(StringCopy(gStringVar1, COMPOUND_STRING("/")),GetSpeciesName(s->species2));
        Label(7,124,gStringVar1,INK_MUTED);
        if (!CheckPartyPokerus(mon,0) && CheckPartyHasHadPokerus(mon,0))
            Label(25,132,COMPOUND_STRING("PkRS"),INK_RED);
    }
}

static void DrawSummaryInfo(void)
{
    struct PokeSummary *s = &sMonSummaryScreen->summary;
    bool8 expanded = sMonSummaryScreen->infoAbilityExpanded;
    u8 type1, type2;
    u32 gift;
    if (s->isEgg)
    {
        Label(88,36,COMPOUND_STRING("OT  -----"),INK_MUTED);
        RightLabel(230,36,COMPOUND_STRING("ID -----"),INK_MUTED);
        SummaryText(88,56,COMPOUND_STRING("Egg watch"),INK_GOLD,FONT_NORMAL);
        SummaryParagraph(88,74,GetEggStateText(),143,2);
        SummaryRect(88,105,142,1,INK_RULE);
        SummaryParagraph(88,107,GetEggMemoText(),143,3);
        return;
    }
    gift = RogueGift_GetCustomMonIdBySpecies(s->species,s->OTID);
    if (InBattleFactory() || InSlateportBattleTent())
        Label(88,36,gText_RentalPkmn,INK_MUTED);
    else if (gift && RogueGift_DisplayCustomMonRarity(gift))
    {
        Label(88,36,sText_UniqueMon,INK_GOLD);
        RightLabel(230,36,s->OTName,INK_BLUE);
    }
    else
    {
        StringCopy(StringCopy(gStringVar1,COMPOUND_STRING("OT  ")),s->OTName);
        Label(88,36,gStringVar1,s->OTGender ? INK_RED : INK_BLUE);
        ConvertIntToDecimalStringN(StringCopy(gStringVar1,COMPOUND_STRING("ID ")),(u16)s->OTID,STR_CONV_MODE_LEADING_ZEROS,5);
        RightLabel(230,36,gStringVar1,INK_MUTED);
    }
    type1 = GetTypeBySpecies(s->species,0,s->OTID);
    type2 = GetTypeBySpecies(s->species,1,s->OTID);
    ShowCompactTypeBadge(0, type1, 88, 51);
    if (type1 != type2)
        ShowCompactTypeBadge(1, type2, 116, 51);
    if (IsTerastallizeEnabled())
    {
        Label(184, 49, COMPOUND_STRING("Tera"), INK_MUTED);
        ShowCompactTypeBadge(2, s->teraType, 206, 51);
    }
    SummaryRect(88,67,142,1,INK_RULE);
    Label(88,69,COMPOUND_STRING("Ability"),INK_MUTED);
    if (IsDynamaxEnabled() && HasAccessToGmaxForm(s->species) && (s->gigatamaxFactor || RogueQuest_GetMonMasteryFlag(s->species)))
        Label(119,69,sText_GmaxFactor,INK_RED);
    RightLabel(230,69,gAbilityNames[s->ability],INK_GOLD);
    SummaryParagraph(88,82,expanded ? GetAbilitySummaryExpandedDescription(s->ability) : gAbilityDescriptionPointers[s->ability],144,expanded ? 4 : 2);
    if (!expanded)
    {
        SummaryRect(88,105,142,1,INK_RULE);
        Label(88,106,gNatureNamePointers[s->nature],INK_BLUE);
        Label(90+GetStringWidth(FONT_SMALL,gNatureNamePointers[s->nature],0),106,COMPOUND_STRING("nature"),INK_MUTED);
        gStringVar4[0] = EOS;
        BufferMonEvolutionMemo();
        Label(88,117,gStringVar4,INK_PAPER);
    }
}

static void DrawSummarySkills(void)
{
    struct PokeSummary *s = &sMonSummaryScreen->summary;
    u8 tab = sMonSummaryScreen->currTabIndex, i;
    const u8 *const views[] = {COMPOUND_STRING("Stats"),COMPOUND_STRING("IVs"),COMPOUND_STRING("EVs"),COMPOUND_STRING("Base")};
    const u8 *const labels[] = {COMPOUND_STRING("HP"),COMPOUND_STRING("Atk"),COMPOUND_STRING("Def"),COMPOUND_STRING("Sp.Atk"),COMPOUND_STRING("Sp.Def"),COMPOUND_STRING("Speed")};
    const u8 order[] = {STAT_HP,STAT_ATK,STAT_DEF,STAT_SPATK,STAT_SPDEF,STAT_SPEED};
    const u16 stats[][6] = {{s->maxHP,s->atk,s->def,s->spatk,s->spdef,s->speed},{s->HPIV,s->atkIV,s->defIV,s->spatkIV,s->spdefIV,s->speedIV},{s->HPEV,s->atkEV,s->defEV,s->spatkEV,s->spdefEV,s->speedEV}};
    u16 base[NUM_STATS];
    u32 start = Rogue_ModifyExperienceTables(gSpeciesInfo[s->species].growthRate,s->level);
    u32 end = s->level == MAX_LEVEL ? start : Rogue_ModifyExperienceTables(gSpeciesInfo[s->species].growthRate,s->level+1);
    const u8 thresholds[] = {80,130,180,220,255};
    CopyItemName(s->item,gStringVar1);
    Label(108,36,s->item ? gStringVar1 : gText_None,INK_PAPER);
    for (i = 0; i < ARRAY_COUNT(thresholds); i++)
    {
        static const u8 star[] = {0x08,0x08,0x7F,0x3E,0x1C,0x36,0x22};
        u8 row, col;
        for (row = 0; row < ARRAY_COUNT(star); row++)
            for (col = 0; col < 7; col++)
                if (star[row] & (1 << col))
                    SummaryRect(185 + 9*i + col, 41 + row, 1, 1, s->friendship >= thresholds[i] ? INK_GOLD : INK_RULE);
    }
    SummaryRect(88,59,142,1,INK_RULE);
    SummaryText(88,60,views[tab],INK_GOLD,FONT_NORMAL);
    RightLabel(230,62,COMPOUND_STRING("L / R"),INK_MUTED);
    if (tab == 3)
        RogueTrial_GetEffectiveSpeciesBaseStats(s->species,base,NUM_STATS);
    for (i=0;i<6;i++)
    {
        u8 x=88+(i/3)*72, y=76+(i%3)*13;
        u8 color=SelectStatColor(order[i]);
        u8 *dst=gStringVar1;
        Label(x,y,labels[i],color==SUMMARY_TEXT_COLOR_RED ? INK_GREEN : color==SUMMARY_TEXT_COLOR_BLUE ? INK_RED : INK_MUTED);
        if (tab==0 && i==0)
            dst=StringCopy(SummaryNumber(dst,s->currentHP),COMPOUND_STRING("/"));
        SummaryNumber(dst,tab==3 ? base[order[i]] : stats[tab][i]);
        RightLabel(x+68,y,gStringVar1,INK_PAPER);
    }
    SummaryRect(88,118,142,1,INK_RULE);
    SummaryNumber(StringCopy(gStringVar1,COMPOUND_STRING("EXP  ")),s->exp);
    Label(88,120,gStringVar1,INK_PAPER);
    SummaryNumber(StringCopy(gStringVar1,COMPOUND_STRING("Next  ")),end>s->exp ? end-s->exp : 0);
    RightLabel(230,120,gStringVar1,INK_MUTED);
    SummaryRect(88,136,141,4,INK_EDGE);
    i=end==start ? 139 : 139*(min(max(s->exp,start),end)-start)/(end-start);
    if (i)
        SummaryRect(89,137,i,2,INK_BLUE);
}

static bool8 HasSummaryMoveDetails(void)
{
    return sMonSummaryScreen->currPageIndex == PSS_PAGE_BATTLE_MOVES
        && (sMonSummaryScreen->moveView != MOVE_BROWSE || sMonSummaryScreen->mode == SUMMARY_MODE_SELECT_MOVE);
}

static void DrawSummaryMoves(bool8 detail)
{
    struct PokemonSummaryScreenData *screen = sMonSummaryScreen;
    bool8 swap = screen->moveView == MOVE_REORDER;
    u8 index = swap ? screen->secondMoveIndex : screen->firstMoveIndex;
    u8 i, rows = detail && !swap ? 5 : 4;
    u8 step = detail ? (swap ? 16 : 13) : 24;
    for (i = 0; i < rows; i++)
    {
        u8 y = 35 + i*step;
        u16 move = i == MAX_MON_MOVES ? screen->newMove : screen->summary.moves[i];
        u8 color = INK_PAPER;
        if (detail && i == index)
        {
            SummaryRect(84,y,150,step,INK_SELECTED);
            SummaryRect(84,y,2,step,INK_GOLD);
            color = INK_GOLD;
        }
        if (swap && i == screen->firstMoveIndex)
            SummaryRect(84,y,150,1,INK_BLUE);
        if (move == MOVE_NONE)
        {
            Label(95,y-1,i == MAX_MON_MOVES ? gText_Cancel : gText_Dash,INK_MUTED);
            continue;
        }
        if (detail)
        {
            ShowCompactTypeBadge(i, GetMonMoveType(move, &screen->currentMon), 89, y + 1);
        }
        else
        {
            SummaryRect(89, y + 4, 3, 6, INK_BLUE);
            Label(95,y+10,gTypeNames[GetMonMoveType(move,&screen->currentMon)],INK_BLUE);
            RightLabel(230,y+10,COMPOUND_STRING("PP"),INK_MUTED);
        }
        SummaryText(detail ? 116 : 95, y-1, gMoveNames[move], color,
                    GetFontIdToFit(gMoveNames[move], FONT_SMALL, 0, detail ? 76 : 102));
        if (i == MAX_MON_MOVES)
        {
            Label(179,y-1,COMPOUND_STRING("New"),INK_GOLD);
            SummaryNumber(StringCopy(SummaryNumber(gStringVar1,gBattleMoves[move].pp),COMPOUND_STRING("/")),gBattleMoves[move].pp);
        }
        else
            SummaryNumber(StringCopy(SummaryNumber(gStringVar1,screen->summary.pp[i]),COMPOUND_STRING("/")),CalculatePPWithBonus(move,screen->summary.ppBonuses,i));
        RightLabel(230,y-1,gStringVar1,INK_MUTED);
    }
    if (detail)
    {
        u16 move = index == MAX_MON_MOVES ? screen->newMove : screen->summary.moves[index];
        SummaryFrame(3,105,234,40);
        if (screen->moveView == MOVE_HM_MESSAGE)
            Label(8,116,COMPOUND_STRING("HM moves cannot be forgotten."),INK_GOLD);
        else if (move)
        {
            u8 split = GetBattleMoveSplit(move);
            Label(8,105,split==SPLIT_SPECIAL ? COMPOUND_STRING("Special") : split==SPLIT_STATUS ? COMPOUND_STRING("Status") : COMPOUND_STRING("Physical"),INK_GOLD);
            Label(85,105,COMPOUND_STRING("Power"),INK_MUTED);
            SummaryNumber(gStringVar1,gBattleMoves[move].power);
            Label(116,105,gBattleMoves[move].power<2 ? gText_ThreeDashes : gStringVar1,INK_PAPER);
            Label(163,105,COMPOUND_STRING("Acc."),INK_MUTED);
            SummaryNumber(gStringVar1,gBattleMoves[move].accuracy);
            Label(186,105,gBattleMoves[move].accuracy ? gStringVar1 : gText_ThreeDashes,INK_PAPER);
            SummaryRect(8,119,224,1,INK_RULE);
            Label(8,119,GetMoveDescription(move),INK_PAPER);
        }
    }
}

static void DrawSummary(u8 previousPage, bool8 previousDetail)
{
    struct PokemonSummaryScreenData *s = sMonSummaryScreen;
    const u8 *const tabs[] = {COMPOUND_STRING("Unique"),COMPOUND_STRING("Info"),COMPOUND_STRING("Skills"),COMPOUND_STRING("Moves")};
    const u8 *hint = COMPOUND_STRING("Left/Right  Page     Up/Down  Pokémon");
    bool8 detail = HasSummaryMoveDetails();
    u8 i;
    for (i = SPRITE_ARR_ID_TYPE; i < SPRITE_ARR_ID_COUNT; i++)
        SetSpriteInvisibility(i, TRUE);
    if (previousPage == PSS_PAGE_COUNT)
    {
        FillWindowPixelBuffer(0,PIXEL_FILL(INK_BG));
        SummaryText(6,0,COMPOUND_STRING("Pokémon Summary"),INK_PAPER,FONT_NORMAL);
        if (!s->summary.isEgg)
        {
            u8 gender = GetMonGender(&s->currentMon);
            SummaryNumber(StringCopy(gStringVar1,COMPOUND_STRING("Lv. ")),s->summary.level);
            if (gender != MON_GENDERLESS)
                RightLabel(228 - GetStringWidth(FONT_SMALL, gStringVar1, 0), 0, gender == MON_MALE ? gText_MaleSymbol : gText_FemaleSymbol, gender == MON_MALE ? INK_BLUE : INK_RED);
            RightLabel(233,0,gStringVar1,INK_MUTED);
        }
    }
    for (i=0;i<PSS_PAGE_COUNT;i++)
    {
        u8 x=4+i*58;
        bool8 active = i == s->currPageIndex;
        if (previousPage != PSS_PAGE_COUNT && (previousPage == s->currPageIndex || (!active && i != previousPage)))
            continue;
        SummaryRect(x,16,58,15,active ? INK_SELECTED : INK_PANEL);
        Label(x+(58-GetStringWidth(FONT_SMALL,tabs[i],0))/2,16,tabs[i],active ? INK_GOLD : s->summary.isEgg ? INK_RULE : INK_MUTED);
        if (active)
            SummaryRect(x+1,30,56,1,INK_GOLD);
    }
    if (previousPage == PSS_PAGE_COUNT || detail != previousDetail)
    {
        // Move details replace the lower half of both panels with one frame.
        if (previousDetail)
            SummaryRect(3,105,234,40,INK_BG);
        DrawSummaryIdentity(detail);
        SummaryFrame(81,33,156,112);
    }
    else
        SummaryRect(82,35,154,detail ? 70 : 109,INK_PANEL);
    if (s->itemSprite < MAX_SPRITES)
        gSprites[s->itemSprite].invisible = s->currPageIndex != PSS_PAGE_SKILLS;
    switch (s->currPageIndex)
    {
    case PSS_PAGE_UNIQUE_ABILITY:
    {
        u16 ability = GetSummaryUniqueAbility();
        Label(88,38,RogueGift_IsAnomalousUniqueAbility(ability) ? sText_AnomalousHeader : COMPOUND_STRING("Unique Ability"),INK_MUTED);
        if (ability)
        {
            SummaryText(88,55,gAbilityNames[ability],INK_GOLD,FONT_NORMAL);
            SummaryRect(88,76,142,1,INK_RULE);
            SummaryParagraph(88,81,gAbilityDescriptionPointers[ability],143,4);
        }
        else
            Label(88,58,sText_NoUniqueAbility,INK_PAPER);
        break;
    }
    case PSS_PAGE_INFO:
        DrawSummaryInfo();
        hint = s->summary.isEgg ? COMPOUND_STRING("B  Back") : s->infoAbilityExpanded ? COMPOUND_STRING("A  Collapse ability     B  Back") : COMPOUND_STRING("A  Expand ability     B  Back");
        break;
    case PSS_PAGE_SKILLS:
        DrawSummarySkills();
        hint = COMPOUND_STRING("A  Cycle stats     L/R  View     B  Back");
        break;
    case PSS_PAGE_BATTLE_MOVES:
        DrawSummaryMoves(detail);
        if (s->mode == SUMMARY_MODE_SELECT_MOVE)
            hint = COMPOUND_STRING("Choose a move     A  Select     B  Cancel");
        else if (s->moveView == MOVE_REORDER)
            hint = COMPOUND_STRING("Up/Down  Destination     A  Swap     B  Cancel");
        else if (detail)
            hint = s->lockMovesFlag ? COMPOUND_STRING("Up/Down  Move     B  Back") : COMPOUND_STRING("Up/Down  Move     A  Reorder     B  Back");
        else
            hint = ShouldShowMoveRemember() ? COMPOUND_STRING("A  Move info     START  Remember     B  Back") : COMPOUND_STRING("A  Move info     B  Back");
        break;
    }
    if (previousPage == PSS_PAGE_COUNT)
        SummaryRect(4,146,232,1,INK_RULE);
    else
        SummaryRect(4,147,232,13,INK_BG);
    Label(7,146,hint,INK_MUTED);
    if (previousPage == PSS_PAGE_COUNT)
        CopyWindowToVram(0, COPYWIN_GFX);
    else
    {
        if (previousPage != s->currPageIndex)
            CopyWindowRectToVram(0, COPYWIN_GFX, 0, 2, 30, 2);
        for (i = 4; i < 18; i++)
        {
            if (detail != previousDetail || (detail && i >= 13))
                CopyWindowRectToVram(0, COPYWIN_GFX, 0, i, 30, 1);
            else
                CopyWindowRectToVram(0, COPYWIN_GFX, 10, i, 20, 1);
        }
        CopyWindowRectToVram(0, COPYWIN_GFX, 0, 18, 30, 2);
    }
}

static void SummaryFinishMove(u8 taskId, u8 slot)
{
    sMoveSlotToReplace = slot;
    gSpecialVar_0x8005 = slot;
    StopPokemonAnimations();
    PlaySE(SE_SELECT);
    BeginCloseSummaryScreen(taskId);
}

static void Task_SummaryInput(u8 taskId)
{
    struct PokemonSummaryScreenData *s = sMonSummaryScreen;
    u8 previousPage = s->currPageIndex;
    bool8 selecting = s->mode == SUMMARY_MODE_SELECT_MOVE;
    bool8 detail = HasSummaryMoveDetails();
    bool8 swap = s->moveView == MOVE_REORDER;
    u8 lr = JOY_NEW(L_BUTTON) ? MENU_L_PRESSED : JOY_NEW(R_BUTTON) ? MENU_R_PRESSED : 0;
    if (MenuHelpers_ShouldWaitForLinkRecv() || gPaletteFade.active || IsDma3ManagerBusyWithBgCopy())
        return;
    if (JOY_NEW(B_BUTTON))
    {
        if (s->moveView == MOVE_HM_MESSAGE)
            s->moveView = MOVE_DETAILS;
        else if (selecting)
            return SummaryFinishMove(taskId,MAX_MON_MOVES);
        else if (detail)
            s->moveView = swap ? MOVE_DETAILS : MOVE_BROWSE;
        else
        {
            PlaySE(SE_SELECT);
            StopPokemonAnimations();
            BeginCloseSummaryScreen(taskId);
            return;
        }
    }
    else if (JOY_NEW(DPAD_UP | DPAD_DOWN))
    {
        s8 delta = JOY_NEW(DPAD_DOWN) ? 1 : -1;
        if (!detail)
        {
            ChangeSummaryPokemon(taskId,delta);
            return;
        }
        else
        {
            u8 *idx = swap ? &s->secondMoveIndex : &s->firstMoveIndex;
            u8 count = swap ? 4 : 5;
            do {
                *idx = (*idx + count + delta) % count;
            } while (*idx < MAX_MON_MOVES && !s->summary.moves[*idx]);
            if (s->moveView == MOVE_HM_MESSAGE)
                s->moveView = MOVE_DETAILS;
        }
    }
    else if ((!detail || selecting) && (JOY_NEW(DPAD_LEFT | DPAD_RIGHT) || (selecting && lr)))
    {
        s8 page = s->currPageIndex + ((JOY_NEW(DPAD_RIGHT) || lr == MENU_R_PRESSED) ? 1 : -1);
        if (s->summary.isEgg || page < s->minPageIndex || page > s->maxPageIndex)
            return;
        s->currPageIndex = page;
        s->currTabIndex = 0;
        s->infoAbilityExpanded = FALSE;
        s->moveView = MOVE_BROWSE;
    }
    else if (s->currPageIndex == PSS_PAGE_SKILLS && (lr || JOY_NEW(A_BUTTON)))
    {
        if (lr == MENU_L_PRESSED && !s->currTabIndex)
            return;
        s->currTabIndex = (s->currTabIndex + (lr == MENU_L_PRESSED ? 3 : 1)) % 4;
    }
    else if (JOY_NEW(START_BUTTON) && !detail && s->currPageIndex == PSS_PAGE_BATTLE_MOVES && ShouldShowMoveRemember())
    {
        gSpecialVar_0x8004 = s->curMonIndex;
        TeachMoveSetContextRelearnMove();
        TeachMoveFromSummaryScreen(s->callback);
        s->callback = CB2_InitLearnMove;
        StopPokemonAnimations();
        BeginCloseSummaryScreen(taskId);
        return;
    }
#if DEBUG_POKEMON_MENU == TRUE
    else if (JOY_NEW(SELECT_BUTTON) && !selecting && !detail && !gMain.inBattle)
    {
        s->callback = CB2_Debug_Pokemon;
        StopPokemonAnimations();
        CloseSummaryScreen(taskId);
        return;
    }
#endif
    else if (JOY_NEW(A_BUTTON))
    {
        if (CanExpandInfoAbility())
            s->infoAbilityExpanded ^= TRUE;
        else if (s->currPageIndex == PSS_PAGE_INFO && s->summary.isEgg && !selecting)
        {
            PlaySE(SE_SELECT);
            StopPokemonAnimations();
            BeginCloseSummaryScreen(taskId);
            return;
        }
        else if (s->currPageIndex != PSS_PAGE_BATTLE_MOVES)
            return;
        else if (s->moveView == MOVE_HM_MESSAGE)
            s->moveView = MOVE_DETAILS;
        else if (selecting)
        {
            if (CanReplaceMove())
                return SummaryFinishMove(taskId,s->firstMoveIndex);
            s->moveView = MOVE_HM_MESSAGE;
        }
        else if (!detail)
        {
            s->firstMoveIndex = 0;
            s->moveView = MOVE_DETAILS;
        }
        else if (swap)
        {
            if (s->isBoxMon)
                SwapBoxMonMoves(&s->monList.boxMons[s->curMonIndex],s->firstMoveIndex,s->secondMoveIndex);
            else
                SwapMonMoves(&s->monList.mons[s->curMonIndex],s->firstMoveIndex,s->secondMoveIndex);
            CopyMonToSummaryStruct(&s->currentMon);
            s->firstMoveIndex = s->secondMoveIndex;
            s->moveView = MOVE_DETAILS;
        }
        else if (s->lockMovesFlag || s->firstMoveIndex == MAX_MON_MOVES)
            s->moveView = MOVE_BROWSE;
        else if (HasMoreThanOneMove())
        {
            s->secondMoveIndex = s->firstMoveIndex;
            s->moveView = MOVE_REORDER;
        }
        else
            return;
    }
    else
        return;
    PlaySE(SE_SELECT);
    DrawSummary(previousPage, detail);
}
