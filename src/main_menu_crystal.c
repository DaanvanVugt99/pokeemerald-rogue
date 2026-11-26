#include "global.h"
#include "bg.h"
#include "constants/rgb.h"
#include "constants/sound.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "gpu_regs.h"
#include "international_string_util.h"
#include "main_menu.h"
#include "main_menu_crystal.h"
#include "palette.h"
#include "pokedex.h"
#include "pokemon.h"
#include "pokemon_animation.h"
#include "rogue_controller.h"
#include "rogue_quest.h"
#include "sound.h"
#include "sprite.h"
#include "strings.h"
#include "string_util.h"
#include "text.h"
#include "trainer_pokemon_sprites.h"
#include "window.h"

#define CRYSTAL_TEXT_WINDOW 0
#define CRYSTAL_TEXT_PALETTE 15
#define CRYSTAL_ART_BG 1
#define CRYSTAL_ART_SCREEN_BLOCK 15
#define CRYSTAL_SABLEYE_PALETTE 14

static const u32 sCrystalBackdropTiles[] = INCBIN_U32("graphics/main_menu/crystal_backdrop.8bpp.lz");
static const u32 sCrystalSavedMap[] = INCBIN_U32("graphics/main_menu/crystal_saved.bin.lz");
static const u32 sCrystalEmptyMap[] = INCBIN_U32("graphics/main_menu/crystal_empty.bin.lz");
static const u16 sCrystalBackdropPalette[] = INCBIN_U16("graphics/main_menu/crystal_backdrop.gbapal");

static const struct WindowTemplate sCrystalWindows[] =
{
    { .bg = 0, .tilemapLeft = 0, .tilemapTop = 0, .width = 30, .height = 20, .paletteNum = CRYSTAL_TEXT_PALETTE, .baseBlock = 1 },
    DUMMY_WIN_TEMPLATE
};

static const u16 sCrystalTextPalette[] =
{
    RGB(0, 0, 0),
    RGB(30, 29, 31), // Text
    RGB(2, 1, 4), // Shadow
    RGB(23, 21, 26), // Labels
    RGB(20, 17, 22), // Footer
    RGB(13, 21, 22), // Selection jewel
    RGB(7, 12, 15),  // Jewel facet
    RGB(25, 30, 29), // Jewel highlight
};

static const u8 sValueColors[] = {0, 1, 2};
static const u8 sLabelColors[] = {0, 3, 2};
static const u8 sFooterColors[] = {0, 4, 0};
static const u8 sText_Options[] = _("Options");
static const u8 sText_Select[] = _("Select");
static const u8 sText_Back[] = _("Back");
static const u8 sText_A[] = _("A");
static const u8 sText_B[] = _("B");
static const u8 sText_Unavailable[] = _("--");

extern const u8 gText_RogueVersion[];
extern const u8 gText_RogueVersionSpacer[];
extern const u8 gText_RogueVersionSuffix[];

struct CrystalMenuItem
{
    u8 markerX;
    u8 centerY;
    u8 labelX;
    u8 markerRadius;
};

static const struct CrystalMenuItem sSavedItems[] =
{
    {18, 48, 33, 4}, // Continue
    {17, 126, 27, 3}, // New Game
    {80, 126, 90, 3}, // Options
};

static const struct CrystalMenuItem sEmptyItems[] =
{
    {18, 69, 31, 3}, // New Game
    {18, 100, 31, 3}, // Options
};

static EWRAM_DATA u16 sCrystalSableye = 0xFFFF;

static void PrintCrystalText(u8 font, const u8 *text, u8 x, u8 y, const u8 *colors)
{
    AddTextPrinterParameterized3(CRYSTAL_TEXT_WINDOW, font, x, y, colors, TEXT_SKIP_DRAW, text);
}

static void PrintSaveField(const u8 *label, const u8 *value, u8 left, u8 right, u8 y)
{
    PrintCrystalText(FONT_SMALL, label, left, y, sLabelColors);
    PrintCrystalText(FONT_NORMAL, value, GetStringRightAlignXOffset(FONT_NORMAL, value, right), y + 11, sValueColors);
}

static void PrintCrystalSaveInfo(void)
{
    const bool8 adventure = Rogue_IsRunActive();
    u8 value[16];
    u8 *minutes;

    PrintCrystalText(FONT_NORMAL, adventure ? gText_MainMenuContinueRun : gText_MainMenuContinue,
        sSavedItems[0].labelX, sSavedItems[0].centerY - 7, sValueColors);
    PrintSaveField(gText_ContinueMenuPlayer, gSaveBlock2Ptr->playerName, 21, 65, 60);

    minutes = ConvertIntToDecimalStringN(value, gSaveBlock2Ptr->playTimeHours, STR_CONV_MODE_LEFT_ALIGN, 4);
    *minutes++ = CHAR_COLON;
    ConvertIntToDecimalStringN(minutes, gSaveBlock2Ptr->playTimeMinutes, STR_CONV_MODE_LEADING_ZEROS, 2);
    PrintSaveField(gText_ContinueMenuTime, value, 79, 122, 60);

    StringCopy(value, sText_Unavailable);
    if (adventure)
        ConvertIntToDecimalStringN(value, Rogue_GetCurrentDifficulty(), STR_CONV_MODE_LEFT_ALIGN, 3);
    else if (FlagGet(FLAG_SYS_POKEDEX_GET))
        ConvertIntToDecimalStringN(value, IsNationalPokedexEnabled() ? GetNationalPokedexCount(FLAG_GET_CAUGHT)
                                                                    : GetHoennPokedexCount(FLAG_GET_CAUGHT),
                                   STR_CONV_MODE_LEFT_ALIGN, 4);
    PrintSaveField(adventure ? gText_ContinueMenuBadges : gText_ContinueMenuPokedex, value, 21, 65, 86);

    ConvertUIntToDecimalStringN(gStringVar1, RogueQuest_GetDisplayCompletePerc(), STR_CONV_MODE_LEFT_ALIGN, 6);
    StringExpandPlaceholders(value, gText_Var1Percent);
    PrintSaveField(gText_ContinueMenuQuests, value, 79, 122, 86);
}

void MainMenuCrystal_SetSelection(bool8 hasSave, u8 selection)
{
    const struct CrystalMenuItem *items = hasSave ? sSavedItems : sEmptyItems;
    const struct CrystalMenuItem *selected = &items[selection];
    u8 i, x = selected->markerX, y = selected->centerY - selected->markerRadius;
    u8 radius = selected->markerRadius;

    for (i = 0; i < (hasSave ? ARRAY_COUNT(sSavedItems) : ARRAY_COUNT(sEmptyItems)); ++i)
    {
        u8 size = items[i].markerRadius * 2 + 1;
        FillWindowPixelRect(CRYSTAL_TEXT_WINDOW, 0, items[i].markerX,
            items[i].centerY - items[i].markerRadius, size, size);
    }

    for (i = 0; i <= radius * 2; ++i)
    {
        u8 inset = abs(i - radius);
        FillWindowPixelRect(CRYSTAL_TEXT_WINDOW, 5, x + inset, y + i, (radius - inset) * 2 + 1, 1);
        FillWindowPixelRect(CRYSTAL_TEXT_WINDOW, 7, x + inset, y + i, 1, 1);
        if (inset < radius)
            FillWindowPixelRect(CRYSTAL_TEXT_WINDOW, 6, x + radius + 1, y + i, radius - inset, 1);
    }
    CopyWindowToVram(CRYSTAL_TEXT_WINDOW, COPYWIN_GFX);
}

void MainMenuCrystal_Load(bool8 hasSave, u8 selection)
{
    const struct CrystalMenuItem *newGame = hasSave ? &sSavedItems[1] : &sEmptyItems[0];
    const struct CrystalMenuItem *options = hasSave ? &sSavedItems[2] : &sEmptyItems[1];

    // Use character base 0 and screen block 15, below the BG0 text tiles.
    SetBgAttribute(CRYSTAL_ART_BG, BG_ATTR_PALETTEMODE, 1);
    SetBgAttribute(CRYSTAL_ART_BG, BG_ATTR_MAPBASEINDEX, CRYSTAL_ART_SCREEN_BLOCK);
    LZ77UnCompVram(sCrystalBackdropTiles, (void *)BG_CHAR_ADDR(0));
    LZ77UnCompVram(hasSave ? sCrystalSavedMap : sCrystalEmptyMap, (void *)BG_SCREEN_ADDR(CRYSTAL_ART_SCREEN_BLOCK));
    LoadPalette(sCrystalBackdropPalette, 0, PLTT_SIZE_4BPP * CRYSTAL_TEXT_PALETTE);
    LoadPalette(sCrystalTextPalette, BG_PLTT_ID(CRYSTAL_TEXT_PALETTE), sizeof(sCrystalTextPalette));
    ClearGpuRegBits(REG_OFFSET_DISPCNT, DISPCNT_WIN0_ON);
    SetGpuReg(REG_OFFSET_BLDCNT, 0);

    FreeAllWindowBuffers();
    InitWindows(sCrystalWindows);
    FillWindowPixelBuffer(CRYSTAL_TEXT_WINDOW, PIXEL_FILL(0));
    if (hasSave)
        PrintCrystalSaveInfo();
    PrintCrystalText(FONT_SMALL, gText_MainMenuNewGame, newGame->labelX, newGame->centerY - 7, sLabelColors);
    PrintCrystalText(FONT_SMALL, sText_Options, options->labelX, options->centerY - 7, sLabelColors);
    PrintCrystalText(FONT_SMALL, sText_A, 155, 145, sValueColors);
    PrintCrystalText(FONT_SMALL, sText_B, 200, 145, sValueColors);
    PrintCrystalText(FONT_SMALL, sText_Select, 166, 146, sLabelColors);
    PrintCrystalText(FONT_SMALL, sText_Back, 211, 146, sLabelColors);
    StringCopy(gStringVar1, gText_RogueVersion);
    StringAppend(gStringVar1, gText_RogueVersionSpacer);
    StringAppend(gStringVar1, gText_RogueVersionSuffix);
    PrintCrystalText(FONT_SMALL, gStringVar1, 13, 146, sFooterColors);

    PutWindowTilemap(CRYSTAL_TEXT_WINDOW);
    CopyWindowToVram(CRYSTAL_TEXT_WINDOW, COPYWIN_MAP);
    MainMenuCrystal_SetSelection(hasSave, selection);
    sCrystalSableye = CreateMonPicSprite_Affine(SPECIES_SABLEYE, NON_SHINY_PLACEHOLDER, 0, MON_MALE,
        FALSE, MON_PIC_AFFINE_FRONT | F_MON_PIC_NO_AFFINE, 187, 89, CRYSTAL_SABLEYE_PALETTE, TAG_NONE);
}

void MainMenuCrystal_PlayEntrance(void)
{
    if (sCrystalSableye < MAX_SPRITES)
    {
        struct Sprite *sprite = &gSprites[sCrystalSableye];
        StartSpriteAnim(sprite, 1);
        StartMonSummaryAnimation(sprite, gSpeciesInfo[SPECIES_SABLEYE].frontAnimId);
        PlayCry_NormalNoDucking(SPECIES_SABLEYE, 0, CRY_VOLUME, CRY_PRIORITY_NORMAL);
    }
}

void MainMenuCrystal_EndEntrance(void)
{
    if (sCrystalSableye < MAX_SPRITES)
        gSprites[sCrystalSableye].callback = SpriteCallbackDummy;
    if (IsCryPlaying())
        StopCryAndClearCrySongs();
}

void MainMenuCrystal_Destroy(void)
{
    if (sCrystalSableye < MAX_SPRITES)
        FreeAndDestroyMonPicSprite(sCrystalSableye);
    sCrystalSableye = 0xFFFF;
}
