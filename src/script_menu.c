#include "global.h"
#include "bg.h"
#include "main.h"
#include "battle_main.h"
#include "data.h"
#include "decompress.h"
#include "event_data.h"
#include "field_effect.h"
#include "field_specials.h"
#include "item.h"
#include "item_icon.h"
#include "list_menu.h"
#include "malloc.h"
#include "menu.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_summary_screen.h"
#include "script.h"
#include "script_menu.h"
#include "sound.h"
#include "sprite.h"
#include "string_util.h"
#include "strings.h"
#include "task.h"
#include "text.h"
#include "trainer_pokemon_sprites.h"
#include "constants/abilities.h"
#include "constants/field_specials.h"
#include "constants/items.h"
#include "constants/rgb.h"
#include "constants/script_menu.h"
#include "constants/songs.h"

#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_hub.h"
#include "rogue_pokedex.h"
#include "rogue_script.h"
#include "rogue_trials.h"

#include "data/script_menu.h"

static EWRAM_DATA u8 sProcessInputDelay = 0;

static u8 sLilycoveSSTidalSelections[SSTIDAL_SELECTION_COUNT];

static void Task_HandleMultichoiceInput(u8 taskId);
static void Task_HandleYesNoInput(u8 taskId);
static void Task_HandleMultichoiceGridInput(u8 taskId);
static void DrawMultichoiceMenu(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 cursorPos);
static void InitMultichoiceCheckWrap(bool8 ignoreBPress, u8 count, u8 windowId, u8 multichoiceId);
static void DrawLinkServicesMultichoiceMenu(u8 multichoiceId);
static void CreatePCMultichoice(void);
static void CreateLilycoveSSTidalMultichoice(void);
static bool8 IsPicboxClosed(void);
static void CreateStartMenuForPokenavTutorial(void);
static void InitMultichoiceNoWrap(bool8 ignoreBPress, u8 unusedCount, u8 windowId, u8 multichoiceId);
static void Task_ShowTrialOverviewInput(u8 taskId);
static void Task_SafariOfferDetailsInput(u8 taskId);

bool8 ScriptMenu_Multichoice(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenu(left, top, multichoiceId, ignoreBPress, 0);
        return TRUE;
    }
}

static void MultichoiceLists_GetList(u8 list, struct MenuAction* dest, u8* outCount)
{
    if(list >= MULTI_DYNAMIC_CALLBACK_START)
    {
        sMultichoiceCallback[list](dest, outCount, MULTICHOICE_LIST_CAPACITY);
    }
    else
    {
        u8 count;
        count = sMultichoiceLists[list].count;
        memcpy(dest, sMultichoiceLists[list].list, sizeof(struct MenuAction) * count);

        *outCount = count;
    }
}

u8 ScriptMenu_MultichoiceLength(u8 multichoiceId)
{
    u8 count;
    struct MenuAction actions[MULTICHOICE_LIST_CAPACITY];
    MultichoiceLists_GetList(multichoiceId, &actions[0], &count);
    return count;
}

bool8 ScriptMenu_MultichoiceWithDefault(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 defaultChoice)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DrawMultichoiceMenu(left, top, multichoiceId, ignoreBPress, defaultChoice);
        return TRUE;
    }
}

static u16 UNUSED GetLengthWithExpandedPlayerName(const u8 *str)
{
    u16 length = 0;

    while (*str != EOS)
    {
        if (*str == PLACEHOLDER_BEGIN)
        {
            str++;
            if (*str == PLACEHOLDER_ID_PLAYER)
            {
                length += StringLength(gSaveBlock2Ptr->playerName);
                str++;
            }
        }
        else
        {
            str++;
            length++;
        }
    }

    return length;
}

static void DrawMultichoiceMenu(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 cursorPos)
{
    int i;
    u8 windowId;
    u8 count;
    struct MenuAction actions[MULTICHOICE_LIST_CAPACITY];
    int width = 0;
    u8 newWidth;

    MultichoiceLists_GetList(multichoiceId, &actions[0], &count);

    for (i = 0; i < count; i++)
    {
        StringExpandPlaceholders(gStringVar4, actions[i].text);
        width = DisplayTextAndGetWidth(gStringVar4, width);
    }

    newWidth = ConvertPixelWidthToTileWidth(width);
    left = ScriptMenu_AdjustLeftCoordFromWidth(left, newWidth);
    windowId = CreateWindowFromRect(left, top, newWidth, count * 2);
    SetStandardWindowBorderStyle(windowId, FALSE);
    PrintMenuTable(windowId, count, &actions[0]);
    InitMenuInUpperLeftCornerNormal(windowId, count, cursorPos);
    ScheduleBgCopyTilemapToVram(0);
    InitMultichoiceCheckWrap(ignoreBPress, count, windowId, multichoiceId);
}

#define tLeft           data[0]
#define tTop            data[1]
#define tRight          data[2]
#define tBottom         data[3]
#define tIgnoreBPress   data[4]
#define tDoWrap         data[5]
#define tWindowId       data[6]
#define tMultichoiceId  data[7]

static void InitMultichoiceCheckWrap(bool8 ignoreBPress, u8 count, u8 windowId, u8 multichoiceId)
{
    u8 i;
    u8 taskId;
    sProcessInputDelay = 2;

    for (i = 0; i < ARRAY_COUNT(sLinkServicesMultichoiceIds); i++)
    {
        if (sLinkServicesMultichoiceIds[i] == multichoiceId)
        {
            sProcessInputDelay = 12;
        }
    }

    taskId = CreateTask(Task_HandleMultichoiceInput, 80);

    gTasks[taskId].tIgnoreBPress = ignoreBPress;

    if (count > 3)
        gTasks[taskId].tDoWrap = TRUE;
    else
        gTasks[taskId].tDoWrap = FALSE;

    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tMultichoiceId = multichoiceId;

    DrawLinkServicesMultichoiceMenu(multichoiceId);
}

static void Task_HandleMultichoiceInput(u8 taskId)
{
    s8 selection;
    s16 *data = gTasks[taskId].data;

    if (!gPaletteFade.active)
    {
        if (sProcessInputDelay)
        {
            sProcessInputDelay--;
        }
        else
        {
            if (!tDoWrap)
                selection = Menu_ProcessInputNoWrap();
            else
                selection = Menu_ProcessInput();

            if (JOY_NEW(DPAD_UP | DPAD_DOWN))
            {
                DrawLinkServicesMultichoiceMenu(tMultichoiceId);
            }

            if (selection != MENU_NOTHING_CHOSEN)
            {
                if (selection == MENU_B_PRESSED)
                {
                    if (tIgnoreBPress)
                        return;
                    PlaySE(SE_SELECT);
                    gSpecialVar_Result = MULTI_B_PRESSED;
                }
                else
                {
                    gSpecialVar_Result = selection;
                }
                ClearToTransparentAndRemoveWindow(tWindowId);
                DestroyTask(taskId);
                ScriptContext_Enable();
            }
        }
    }
}

bool8 ScriptMenu_YesNo(u8 left, u8 top)
{
    if (FuncIsActiveTask(Task_HandleYesNoInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        DisplayYesNoMenuDefaultYes();
        CreateTask(Task_HandleYesNoInput, 0x50);
        return TRUE;
    }
}

// Unused
bool8 IsScriptActive(void)
{
    if (gSpecialVar_Result == 0xFF)
        return FALSE;
    else
        return TRUE;
}

static void Task_HandleYesNoInput(u8 taskId)
{
    if (gTasks[taskId].tRight < 5)
    {
        gTasks[taskId].tRight++;
        return;
    }

    switch (Menu_ProcessInputNoWrapClearOnChoose())
    {
    case MENU_NOTHING_CHOSEN:
        return;
    case MENU_B_PRESSED:
    case 1:
        PlaySE(SE_SELECT);
        gSpecialVar_Result = 0;
        break;
    case 0:
        gSpecialVar_Result = 1;
        break;
    }

    DestroyTask(taskId);
    ScriptContext_Enable();
}

bool8 ScriptMenu_MultichoiceGrid(u8 left, u8 top, u8 multichoiceId, bool8 ignoreBPress, u8 columnCount)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceGridInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        u8 taskId;
        u8 rowCount, newWidth;
        int i, width;
        u8 listCount;
        struct MenuAction actions[MULTICHOICE_LIST_CAPACITY];

        gSpecialVar_Result = 0xFF;
        width = 0;
        
        MultichoiceLists_GetList(multichoiceId, &actions[0], &listCount);

        for (i = 0; i < listCount; i++)
        {
            StringExpandPlaceholders(gStringVar4, actions[i].text);
            width = DisplayTextAndGetWidth(gStringVar4, width);
        }

        newWidth = ConvertPixelWidthToTileWidth(width);

        left = ScriptMenu_AdjustLeftCoordFromWidth(left, columnCount * newWidth);
        rowCount = listCount / columnCount;

        taskId = CreateTask(Task_HandleMultichoiceGridInput, 80);

        gTasks[taskId].tIgnoreBPress = ignoreBPress;
        gTasks[taskId].tWindowId = CreateWindowFromRect(left, top, columnCount * newWidth, rowCount * 2);
        SetStandardWindowBorderStyle(gTasks[taskId].tWindowId, FALSE);
        PrintMenuGridTable(gTasks[taskId].tWindowId, newWidth * 8, columnCount, rowCount, &actions[0]);
        InitMenuActionGrid(gTasks[taskId].tWindowId, newWidth * 8, columnCount, rowCount, 0);
        CopyWindowToVram(gTasks[taskId].tWindowId, COPYWIN_FULL);
        return TRUE;
    }
}

static void Task_HandleMultichoiceGridInput(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s8 selection = Menu_ProcessGridInput();

    switch (selection)
    {
    case MENU_NOTHING_CHOSEN:
        return;
    case MENU_B_PRESSED:
        if (tIgnoreBPress)
            return;
        PlaySE(SE_SELECT);
        gSpecialVar_Result = MULTI_B_PRESSED;
        break;
    default:
        gSpecialVar_Result = selection;
        break;
    }

    ClearToTransparentAndRemoveWindow(tWindowId);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

#undef tWindowId

bool16 ScriptMenu_CreatePCMultichoice(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreatePCMultichoice();
        return TRUE;
    }
}

static void CreatePCMultichoice(void)
{
    u8 x = 8;
    u32 pixelWidth = 0;
    u8 width;
    u8 numChoices;
    u8 windowId;
    int i;

    for (i = 0; i < ARRAY_COUNT(sPCNameStrings); i++)
    {
        pixelWidth = DisplayTextAndGetWidth(sPCNameStrings[i], pixelWidth);
    }

    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        pixelWidth = DisplayTextAndGetWidth(gText_HallOfFame, pixelWidth);
    }

    width = ConvertPixelWidthToTileWidth(pixelWidth);

    // Include Hall of Fame option if player is champion
    if (FlagGet(FLAG_SYS_GAME_CLEAR))
    {
        numChoices = 4;
        windowId = CreateWindowFromRect(0, 0, width, 8);
        SetDarkStandardWindowBorderStyle(windowId, FALSE);
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_HallOfFame, x, 33, TEXT_SKIP_DRAW, NULL);
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_LogOff, x, 49, TEXT_SKIP_DRAW, NULL);
    }
    else
    {
        numChoices = 3;
        windowId = CreateWindowFromRect(0, 0, width, 6);
        SetDarkStandardWindowBorderStyle(windowId, FALSE);
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_LogOff, x, 33, TEXT_SKIP_DRAW, NULL);
    }

    // Change PC name if player has met Lanette
    //if (FlagGet(FLAG_SYS_PC_LANETTE))
    //    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_LanettesPC, x, 1, TEXT_SKIP_DRAW, NULL);
    //else
        AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_SomeonesPC, x, 1, TEXT_SKIP_DRAW, NULL);

    StringExpandPlaceholders(gStringVar4, gText_PlayersPC);
    PrintPlayerNameOnWindow(windowId, gStringVar4, x, 17);
    InitMenuInUpperLeftCornerNormal(windowId, numChoices, 0);
    CopyWindowToVram(windowId, COPYWIN_FULL);
    InitMultichoiceCheckWrap(FALSE, numChoices, windowId, MULTI_PC);
}

void ScriptMenu_DisplayPCStartupPrompt(void)
{
    LoadMessageBoxAndFrameGfx(0, TRUE);
    AddTextPrinterParameterized2(0, FONT_NORMAL, gText_WhichPCShouldBeAccessed, 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
}

bool8 ScriptMenu_CreateLilycoveSSTidalMultichoice(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreateLilycoveSSTidalMultichoice();
        return TRUE;
    }
}

// gSpecialVar_0x8004 is 1 if the Sailor was shown multiple event tickets at the same time
// otherwise gSpecialVar_0x8004 is 0
static void CreateLilycoveSSTidalMultichoice(void)
{
    u8 selectionCount = 0;
    u8 count;
    u32 pixelWidth;
    u8 width;
    u8 windowId;
    u8 i;
    u32 j;

    for (i = 0; i < SSTIDAL_SELECTION_COUNT; i++)
    {
        sLilycoveSSTidalSelections[i] = 0xFF;
    }

    GetFontAttribute(FONT_NORMAL, FONTATTR_MAX_LETTER_WIDTH);

    if (gSpecialVar_0x8004 == 0)
    {
        sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_SLATEPORT;
        selectionCount++;

        if (FlagGet(FLAG_MET_SCOTT_ON_SS_TIDAL) == TRUE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_BATTLE_FRONTIER;
            selectionCount++;
        }
    }

    if (CheckBagHasItem(ITEM_EON_TICKET, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_SOUTHERN_ISLAND) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_SOUTHERN_ISLAND;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_EON_TICKET) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_SOUTHERN_ISLAND;
            selectionCount++;
            FlagSet(FLAG_SHOWN_EON_TICKET);
        }
    }

    if (CheckBagHasItem(ITEM_MYSTIC_TICKET, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_NAVEL_ROCK) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_NAVEL_ROCK;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_MYSTIC_TICKET) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_NAVEL_ROCK;
            selectionCount++;
            FlagSet(FLAG_SHOWN_MYSTIC_TICKET);
        }
    }

    if (CheckBagHasItem(ITEM_AURORA_TICKET, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_BIRTH_ISLAND) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_BIRTH_ISLAND;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_AURORA_TICKET) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_BIRTH_ISLAND;
            selectionCount++;
            FlagSet(FLAG_SHOWN_AURORA_TICKET);
        }
    }

    if (CheckBagHasItem(ITEM_OLD_SEA_MAP, 1) == TRUE && FlagGet(FLAG_ENABLE_SHIP_FARAWAY_ISLAND) == TRUE)
    {
        if (gSpecialVar_0x8004 == 0)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_FARAWAY_ISLAND;
            selectionCount++;
        }

        if (gSpecialVar_0x8004 == 1 && FlagGet(FLAG_SHOWN_OLD_SEA_MAP) == FALSE)
        {
            sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_FARAWAY_ISLAND;
            selectionCount++;
            FlagSet(FLAG_SHOWN_OLD_SEA_MAP);
        }
    }

    sLilycoveSSTidalSelections[selectionCount] = SSTIDAL_SELECTION_EXIT;
    selectionCount++;

    if (gSpecialVar_0x8004 == 0 && FlagGet(FLAG_MET_SCOTT_ON_SS_TIDAL) == TRUE)
    {
        count = selectionCount;
    }

    count = selectionCount;
    if (count == SSTIDAL_SELECTION_COUNT)
    {
        gSpecialVar_0x8004 = SCROLL_MULTI_SS_TIDAL_DESTINATION;
        ShowScrollableMultichoice();
    }
    else
    {
        pixelWidth = 0;

        for (j = 0; j < SSTIDAL_SELECTION_COUNT; j++)
        {
            u8 selection = sLilycoveSSTidalSelections[j];
            if (selection != 0xFF)
            {
                pixelWidth = DisplayTextAndGetWidth(sLilycoveSSTidalDestinations[selection], pixelWidth);
            }
        }

        width = ConvertPixelWidthToTileWidth(pixelWidth);
        windowId = CreateWindowFromRect(MAX_MULTICHOICE_WIDTH - width, (6 - count) * 2, width, count * 2);
        SetStandardWindowBorderStyle(windowId, FALSE);

        for (selectionCount = 0, i = 0; i < SSTIDAL_SELECTION_COUNT; i++)
        {
            if (sLilycoveSSTidalSelections[i] != 0xFF)
            {
                AddTextPrinterParameterized(windowId, FONT_NORMAL, sLilycoveSSTidalDestinations[sLilycoveSSTidalSelections[i]], 8, selectionCount * 16 + 1, TEXT_SKIP_DRAW, NULL);
                selectionCount++;
            }
        }

        InitMenuInUpperLeftCornerNormal(windowId, count, count - 1);
        CopyWindowToVram(windowId, COPYWIN_FULL);
        InitMultichoiceCheckWrap(FALSE, count, windowId, MULTI_SSTIDAL_LILYCOVE);
    }
}

void GetLilycoveSSTidalSelection(void)
{
    if (gSpecialVar_Result != MULTI_B_PRESSED)
    {
        gSpecialVar_Result = sLilycoveSSTidalSelections[gSpecialVar_Result];
    }
}

#define tState       data[0]
#define tMonSpecies  data[1]
#define tMonSpriteId data[2]
#define tWindowX     data[3]
#define tWindowY     data[4]
#define tWindowId    data[5]

static void Task_PokemonPicWindow(u8 taskId)
{
    struct Task *task = &gTasks[taskId];

    switch (task->tState)
    {
    case 0:
        task->tState++;
        break;
    case 1:
        // Wait until state is advanced by ScriptMenu_HidePokemonPic
        break;
    case 2:
        FreeResourcesAndDestroySprite(&gSprites[task->tMonSpriteId], task->tMonSpriteId);
        task->tState++;
        break;
    case 3:
        ClearToTransparentAndRemoveWindow(task->tWindowId);
        DestroyTask(taskId);
        break;
    }
}

bool8 ScriptMenu_ShowPokemonPicCustom(u16 species, u32 otId, u8 x, u8 y, bool8 isObscured)
{
    u8 taskId;
    u8 spriteId;

    if (FindTaskIdByFunc(Task_PokemonPicWindow) != TASK_NONE)
    {
        return FALSE;
    }
    else
    {
        spriteId = CreateMonSprite_PicBoxCustom(species, otId, x * 8 + 40, y * 8 + 40, 0);
        if(spriteId == MAX_SPRITES)
            return FALSE;

        taskId = CreateTask(Task_PokemonPicWindow, 0x50);
        gTasks[taskId].tWindowId = CreateWindowFromRect(x, y, 8, 8);
        gTasks[taskId].tState = 0;
        gTasks[taskId].tMonSpecies = species;
        gTasks[taskId].tMonSpriteId = spriteId;
        gSprites[spriteId].callback = SpriteCallbackDummy;
        gSprites[spriteId].oam.priority = 0;

        if(isObscured)
        {
            // black out palette
            TintPalette_StompColour(&gPlttBufferUnfaded[OBJ_PLTT_ID(gSprites[spriteId].oam.paletteNum)], 16, RGB(1, 1, 1));
            TintPalette_StompColour(&gPlttBufferFaded[OBJ_PLTT_ID(gSprites[spriteId].oam.paletteNum)], 16, RGB(1, 1, 1));
        }

        SetStandardWindowBorderStyle(gTasks[taskId].tWindowId, TRUE);
        ScheduleBgCopyTilemapToVram(0);
        return TRUE;
    }
}

bool8 ScriptMenu_ShowPokemonPic(u16 species, u8 x, u8 y, bool8 isObscured)
{
    return ScriptMenu_ShowPokemonPicCustom(species, 0, x, y, isObscured);
}

void ScriptMenu_ShowDynamicUniqueMonPic(void)
{
    AGB_ASSERT(gSpecialVar_0x8004 < DYNAMIC_UNIQUE_MON_COUNT);

    if(RogueGift_IsDynamicMonSlotEnabled(gSpecialVar_0x8004))
    {
        struct UniqueMon* uniqueMon = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004);
        ScriptMenu_ShowPokemonPicCustom(uniqueMon->species, uniqueMon->customMonId, 2, 1, FALSE);
    }
}

bool8 (*ScriptMenu_HidePokemonPic(void))(void)
{
    u8 taskId = FindTaskIdByFunc(Task_PokemonPicWindow);

    if (taskId == TASK_NONE)
        return NULL;
    gTasks[taskId].tState++;
    return IsPicboxClosed;
}

static bool8 IsPicboxClosed(void)
{
    if (FindTaskIdByFunc(Task_PokemonPicWindow) == TASK_NONE)
        return TRUE;
    else
        return FALSE;
}

#undef tState
#undef tMonSpecies
#undef tMonSpriteId
#undef tWindowX
#undef tWindowY
#undef tWindowId

u8 CreateWindowFromRect(u8 x, u8 y, u8 width, u8 height)
{
    struct WindowTemplate template = CreateWindowTemplate(0, x + 1, y + 1, width, height, 15, 100);
    u8 windowId = AddWindow(&template);
    PutWindowTilemap(windowId);
    return windowId;
}

void ClearToTransparentAndRemoveWindow(u8 windowId)
{
    ClearStdWindowAndFrameToTransparent(windowId, TRUE);
    RemoveWindow(windowId);
}

static void DrawLinkServicesMultichoiceMenu(u8 multichoiceId)
{
    switch (multichoiceId)
    {
    case MULTI_WIRELESS_NO_BERRY:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptionsNoBerryCrush[Menu_GetCursorPos()], 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
        break;
    case MULTI_CABLE_CLUB_WITH_RECORD_MIX:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sCableClubOptions_WithRecordMix[Menu_GetCursorPos()], 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
        break;
    case MULTI_WIRELESS_NO_RECORD:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptions_NoRecordMix[Menu_GetCursorPos()], 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
        break;
    case MULTI_WIRELESS_ALL_SERVICES:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptions_AllServices[Menu_GetCursorPos()], 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
        break;
    case MULTI_WIRELESS_NO_RECORD_BERRY:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sWirelessOptions_NoRecordMixBerryCrush[Menu_GetCursorPos()], 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
        break;
    case MULTI_CABLE_CLUB_NO_RECORD_MIX:
        FillWindowPixelBuffer(0, PIXEL_FILL(1));
        AddTextPrinterParameterized2(0, FONT_NORMAL, sCableClubOptions_NoRecordMix[Menu_GetCursorPos()], 0, NULL, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_WHITE, TEXT_COLOR_LIGHT_GRAY);
        break;
    }
}

bool16 ScriptMenu_CreateStartMenuForPokenavTutorial(void)
{
    if (FuncIsActiveTask(Task_HandleMultichoiceInput) == TRUE)
    {
        return FALSE;
    }
    else
    {
        gSpecialVar_Result = 0xFF;
        CreateStartMenuForPokenavTutorial();
        return TRUE;
    }
}

static void CreateStartMenuForPokenavTutorial(void)
{
    u8 windowId = CreateWindowFromRect(21, 0, 7, 18);
    SetStandardWindowBorderStyle(windowId, FALSE);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionPokedex, 8, 9, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionPokemon, 8, 25, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionBag, 8, 41, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionPokenav, 8, 57, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gSaveBlock2Ptr->playerName, 8, 73, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionSave, 8, 89, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionOption, 8, 105, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gText_MenuOptionExit, 8, 121, TEXT_SKIP_DRAW, NULL);
    InitMenuNormal(windowId, FONT_NORMAL, 0, 9, 16, ARRAY_COUNT(MultichoiceList_ForcedStartMenu), 0);
    InitMultichoiceNoWrap(FALSE, ARRAY_COUNT(MultichoiceList_ForcedStartMenu), windowId, MULTI_FORCED_START_MENU);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

#define tWindowId       data[6]

static void InitMultichoiceNoWrap(bool8 ignoreBPress, u8 unusedCount, u8 windowId, u8 multichoiceId)
{
    u8 taskId;
    sProcessInputDelay = 2;
    taskId = CreateTask(Task_HandleMultichoiceInput, 80);
    gTasks[taskId].tIgnoreBPress = ignoreBPress;
    gTasks[taskId].tDoWrap = 0;
    gTasks[taskId].tWindowId = windowId;
    gTasks[taskId].tMultichoiceId = multichoiceId;
}

#undef tLeft
#undef tTop
#undef tRight
#undef tBottom
#undef tIgnoreBPress
#undef tDoWrap
#undef tWindowId
#undef tMultichoiceId

static int DisplayTextAndGetWidthInternal(const u8 *str)
{
    u8 temp[64];
    StringExpandPlaceholders(temp, str);
    return GetStringWidth(FONT_NORMAL, temp, 0);
}

int DisplayTextAndGetWidth(const u8 *str, int prevWidth)
{
    int width = DisplayTextAndGetWidthInternal(str);
    if (width < prevWidth)
    {
        width = prevWidth;
    }
    return width;
}

int ConvertPixelWidthToTileWidth(int width)
{
    return (((width + 9) / 8) + 1) > MAX_MULTICHOICE_WIDTH ? MAX_MULTICHOICE_WIDTH : (((width + 9) / 8) + 1);
}

int ScriptMenu_AdjustLeftCoordFromWidth(int left, int width)
{
    int adjustedLeft = left;

    if (left + width > MAX_MULTICHOICE_WIDTH)
    {
        if (MAX_MULTICHOICE_WIDTH - width < 0)
        {
            adjustedLeft = 0;
        }
        else
        {
            adjustedLeft = MAX_MULTICHOICE_WIDTH - width;
        }
    }

    return adjustedLeft;
}

// Multichoice lists
//
static void Task_ScrollingMultichoiceInput(u8 taskId);

static const struct ListMenuTemplate sMultichoiceListTemplate =
{
    .header_X = 0,
    .item_X = 8,
    .cursor_X = 0,
    .upText_Y = 1,
    .cursorPal = 2,
    .fillValue = 1,
    .cursorShadowPal = 3,
    .lettersSpacing = 1,
    .itemVerticalPadding = 0,
    .scrollMultiple = LIST_MULTIPLE_SCROLL_L_R,
    .fontId = 1,
    .cursorKind = 0
};

static EWRAM_DATA struct ListMenuItem* sDynamicScrollingMultichoiceList = NULL;
static EWRAM_DATA u16 sDynamicScrollingMultichoiceCount = 0;
static EWRAM_DATA u16 sDynamicScrollingMultichoiceDefault = 0;
#ifdef ROGUE_DEBUG
static EWRAM_DATA u16 sDynamicScrollingMultichoiceCapacity = 0;
#endif

// 0x8004 = set id
// 0x8005 = window X
// 0x8006 = window y
// 0x8007 = showed at once
// 0x8008 = Allow B press
static void ScriptMenu_ScrollingMultichoiceInternal(const struct ListMenuItem *list, u16 listCount, bool8 hasSetSize, u16 defaultIndex)
{
    int i, windowId, taskId, width = 0;
    int left = gSpecialVar_0x8005;
    int top = gSpecialVar_0x8006;
    int maxShowed = gSpecialVar_0x8007;

    if(!hasSetSize)
    {
        maxShowed = min(maxShowed, listCount);
    }

    for (i = 0; i < listCount; i++)
        width = DisplayTextAndGetWidth(list[i].name, width);

    width = ConvertPixelWidthToTileWidth(width);
    left = ScriptMenu_AdjustLeftCoordFromWidth(left, width);
    windowId = CreateWindowFromRect(left, top, width, maxShowed * 2);
    SetStandardWindowBorderStyle(windowId, 0);
    CopyWindowToVram(windowId, 3);

    gMultiuseListMenuTemplate = sMultichoiceListTemplate;
    gMultiuseListMenuTemplate.windowId = windowId;
    gMultiuseListMenuTemplate.items = list;
    gMultiuseListMenuTemplate.totalItems = listCount;
    gMultiuseListMenuTemplate.maxShowed = maxShowed;

    taskId = CreateTask(Task_ScrollingMultichoiceInput, 0);
    gTasks[taskId].data[0] = ListMenuInit(&gMultiuseListMenuTemplate, 0, defaultIndex);
    gTasks[taskId].data[1] = gSpecialVar_0x8008;
    gTasks[taskId].data[2] = windowId;
}

void ScriptMenu_ScrollingMultichoice(void)
{
    int setId = gSpecialVar_0x8004;
    ScriptMenu_ScrollingMultichoiceInternal(sScrollingMultichoiceLists[setId].list, sScrollingMultichoiceLists[setId].count, TRUE, 0);
}

void ScriptMenu_ScrollingMultichoiceDynamicBegin(u16 capacity)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList == NULL);
    sDynamicScrollingMultichoiceList = Alloc(sizeof(struct ListMenuItem) * capacity);
    sDynamicScrollingMultichoiceCount = 0;
    sDynamicScrollingMultichoiceDefault = 0;
#ifdef ROGUE_DEBUG
    sDynamicScrollingMultichoiceCapacity = capacity;
#endif
}

void ScriptMenu_ScrollingMultichoiceDynamicAppendOption(u8 const* str, u16 value)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList != NULL);
#ifdef ROGUE_DEBUG
    AGB_ASSERT(sDynamicScrollingMultichoiceCount < sDynamicScrollingMultichoiceCapacity);
#endif

    sDynamicScrollingMultichoiceList[sDynamicScrollingMultichoiceCount].name = str;
    sDynamicScrollingMultichoiceList[sDynamicScrollingMultichoiceCount].id = value;
    sDynamicScrollingMultichoiceCount++;
}

void ScriptMenu_ScrollingMultichoiceDynamicSetDefault(u16 index)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList != NULL);
    sDynamicScrollingMultichoiceDefault = index;
}

void ScriptMenu_ScrollingMultichoiceDynamicEnd(void)
{
    AGB_ASSERT(sDynamicScrollingMultichoiceList != NULL);
    AGB_ASSERT(sDynamicScrollingMultichoiceDefault < sDynamicScrollingMultichoiceCount);
    ScriptMenu_ScrollingMultichoiceInternal(sDynamicScrollingMultichoiceList, sDynamicScrollingMultichoiceCount, FALSE, sDynamicScrollingMultichoiceDefault);
}

static void Task_ScrollingMultichoiceInput(u8 taskId)
{
    bool32 done = FALSE;
    s32 input = ListMenu_ProcessInput(gTasks[taskId].data[0]);

    switch (input)
    {
    case LIST_HEADER:
    case LIST_NOTHING_CHOSEN:
        break;
    case LIST_CANCEL:
        if (gTasks[taskId].data[1])
        {
            gSpecialVar_Result = 0x7F;
            done = TRUE;
        }
        break;
    default:
        gSpecialVar_Result = input;
        done = TRUE;
        break;
    }

    if (done)
    {
        DestroyListMenuTask(gTasks[taskId].data[0], NULL, NULL);
        ClearStdWindowAndFrame(gTasks[taskId].data[2], TRUE);
        RemoveWindow(gTasks[taskId].data[2]);
        ScriptContext_Enable();
        DestroyTask(taskId);

        if(sDynamicScrollingMultichoiceList != NULL)
        {
            Free(sDynamicScrollingMultichoiceList);
            sDynamicScrollingMultichoiceList = NULL;
        }
    }
}

static u8 CreateWindowFromRectWithBaseBlockOffset(u8 x, u8 y, u8 width, u8 height, u16 baseBlockOffset)
{
    struct WindowTemplate template = CreateWindowTemplate(0, x + 1, y + 1, width, height, 15, 100 + baseBlockOffset);
    u8 windowId = AddWindow(&template);
    PutWindowTilemap(windowId);
    return windowId;
}

static void Task_DisplayTextInWindowInput(u8 taskId)
{
    if(JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        u8 windowId = gTasks[taskId].data[0];

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static void Task_DisplayUniqueMonInfoInput(u8 taskId)
{
    u16 input = JOY_NEW(A_BUTTON | B_BUTTON | DPAD_LEFT | DPAD_RIGHT);

    if(input != 0)
    {
        u8 windowId = gTasks[taskId].data[0];

        if(input & B_BUTTON)
            gSpecialVar_Result = UNIQUE_MON_PREVIEW_INPUT_CLOSE;
        else if(input & DPAD_LEFT)
            gSpecialVar_Result = UNIQUE_MON_PREVIEW_INPUT_PREV;
        else
            gSpecialVar_Result = UNIQUE_MON_PREVIEW_INPUT_NEXT;

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

void ScriptMenu_DisplayTextInWindow(const u8* str, u8 x, u8 y, u8 width, u8 height)
{
    u8 taskId;
    u8 windowId = CreateWindowFromRectWithBaseBlockOffset(x, y, width, height, 8 * 8);
    SetStandardWindowBorderStyle(windowId, 0);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, str, 2, 0, TEXT_SKIP_DRAW, NULL);
    CopyWindowToVram(windowId, COPYWIN_FULL);

    taskId = CreateTask(Task_DisplayTextInWindowInput, 0);
    gTasks[taskId].data[0] = windowId;
}

static u8 const sText_UniqueMonTitle[] = _("{STR_VAR_1} {FONT_SMALL_NARROW}{COLOR BLUE}({STR_VAR_2})");
static u8 const sText_UniqueMonTitleRare[] = _("{STR_VAR_1} {FONT_SMALL_NARROW}{COLOR RED}({STR_VAR_2})");
static u8 const sText_UniqueMonAbility[] = _("A/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_UniqueMonUniqueAbility[] = _("U/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_UniqueMonMove[] = _(" -{STR_VAR_1}");
static u8 const sText_UniqueMonOutOfDex[] = _("{COLOR RED}Not in current Dex");
static u8 const sText_UniqueMonType[] = _("Type/ {STR_VAR_1}");
static u8 const sText_UniqueMonTypes[] = _("Type/ {STR_VAR_1}/{STR_VAR_2}");

#define UNIQUE_MON_INFO_WINDOW_HEIGHT 12
#define UNIQUE_MON_INFO_TITLE_HEIGHT  13
#define UNIQUE_MON_INFO_LINE_HEIGHT   13
#define UNIQUE_MON_INFO_MAX_LINES     6
#define UNIQUE_MON_INFO_LINE_Y(line)  (UNIQUE_MON_INFO_TITLE_HEIGHT + UNIQUE_MON_INFO_LINE_HEIGHT * (line))

// Out-of-Dex warning, type, standard ability, unique ability, and two moves.
STATIC_ASSERT(UNIQUE_MON_INFO_LINE_Y(UNIQUE_MON_INFO_MAX_LINES) <= UNIQUE_MON_INFO_WINDOW_HEIGHT * 8, UniqueMonInfoWindowFitsAllDetailLines);

static void PrintUniqueMonInfoToWindow(u8 windowId)
{
    u8 i, line;
    u16 species = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->species;
    u32 customMonId = RogueGift_GetDynamicUniqueMon(gSpecialVar_0x8004)->customMonId;
    u8 rarity = RogueGift_GetCustomMonRarity(customMonId);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    // Title
    StringCopy(gStringVar1, RoguePokedex_GetSpeciesName(species));
    StringCopy(gStringVar2, RogueGift_GetRarityName(rarity));
    StringExpandPlaceholders(gStringVar4, rarity >= UNIQUE_RARITY_EPIC ? sText_UniqueMonTitleRare : sText_UniqueMonTitle);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gStringVar4, 2, 0, TEXT_SKIP_DRAW, NULL);

    line = 0;

    if(!RoguePokedex_IsSpeciesEnabled(species))
        AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, sText_UniqueMonOutOfDex, 2, UNIQUE_MON_INFO_LINE_Y(line++), TEXT_SKIP_DRAW, NULL);

    // Show the effective typing rather than making the player infer altered
    // types from the recolored sprite.
    {
        u8 type1 = RogueGift_GetCustomMonType(customMonId, 0);
        u8 type2 = RogueGift_GetCustomMonType(customMonId, 1);

        if(!IS_STANDARD_TYPE(type1))
            type1 = RoguePokedex_GetSpeciesType(species, 0);
        if(!IS_STANDARD_TYPE(type2))
            type2 = RoguePokedex_GetSpeciesType(species, 1);

        StringCopy(gStringVar1, gTypeNames[type1]);
        if(type1 == type2)
        {
            StringExpandPlaceholders(gStringVar4, sText_UniqueMonType);
        }
        else
        {
            StringCopy(gStringVar2, gTypeNames[type2]);
            StringExpandPlaceholders(gStringVar4, sText_UniqueMonTypes);
        }
        AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, UNIQUE_MON_INFO_LINE_Y(line++), TEXT_SKIP_DRAW, NULL);
    }

    // Ability
    if(RogueGift_GetCustomMonAbilityCount(customMonId) != 0)
    {
        u16 ability = RogueGift_GetCustomMonAbility(customMonId, 0);

        StringCopy(gStringVar1, gAbilityNames[ability]);
        StringExpandPlaceholders(gStringVar4, sText_UniqueMonAbility);
        AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 2, UNIQUE_MON_INFO_LINE_Y(line++), TEXT_SKIP_DRAW, NULL);
    }

    // Unique Ability
    if(RogueGift_GetCustomMonUniqueAbility(customMonId) != ABILITY_NONE)
    {
        u16 ability = RogueGift_GetCustomMonUniqueAbility(customMonId);

        StringCopy(gStringVar1, gAbilityNames[ability]);
        StringExpandPlaceholders(gStringVar4, sText_UniqueMonUniqueAbility);
        AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 2, UNIQUE_MON_INFO_LINE_Y(line++), TEXT_SKIP_DRAW, NULL);
    }

    // Moves
    for(i = 0; i < RogueGift_GetCustomMonMoveCount(customMonId); ++i)
    {
        u16 moveId = RogueGift_GetCustomMonMove(customMonId, i);
        
        StringCopy(gStringVar1, gMoveNames[moveId]);
        StringExpandPlaceholders(gStringVar4, sText_UniqueMonMove);
        AddTextPrinterParameterized(windowId, FONT_SMALL, gStringVar4, 2, UNIQUE_MON_INFO_LINE_Y(line++), TEXT_SKIP_DRAW, NULL);
    }

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

void ScriptMenu_DisplayUniqueMonInfo()
{
    u8 taskId;
    u8 windowId = CreateWindowFromRectWithBaseBlockOffset(12, 0, 14, UNIQUE_MON_INFO_WINDOW_HEIGHT, 8 * 8);

    PrintUniqueMonInfoToWindow(windowId);

    taskId = CreateTask(Task_DisplayUniqueMonInfoInput, 0);
    gTasks[taskId].data[0] = windowId;
}

static u8 const sText_PresetMonAbility_Has[] = _("Ability/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonAbility_Missing[] = _("Ability/ {COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonItem_Has[] = _("Item/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonItem_Missing[] = _("Item/ {COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonNature_Has[] = _("Nature/ {COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonNature_Missing[] = _("Nature/ {COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonMove_Has[] = _(" -{COLOR GREEN}{STR_VAR_1}");
static u8 const sText_PresetMonMove_Missing[] = _(" -{COLOR RED}{STR_VAR_1}");
static u8 const sText_PresetMonNoData[] = _("No recommendations for\nthis Pokémon.\n\n\n(This Pokémon may need\nto evolve in order to\nget recomendations)");

static void PrintRecommendedMonSetToWindow(u8 windowId, struct Pokemon* mon, struct RoguePokemonCompetitiveSet const* preset)
{
    u8 i, line;
    gSpecialVar_Result = GetMonData(mon, MON_DATA_SPECIES, NULL);

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    line = 0;

    if(preset != NULL)
    {
        // Ability
        if(preset->ability == ITEM_NONE)
        {
            StringCopyN(gStringVar1, gText_None, ABILITY_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, sText_PresetMonAbility_Has);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }
        else
        {
            StringCopyN(gStringVar1, gAbilityNames[preset->ability], ABILITY_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, GetMonAbility(mon) == preset->ability ? sText_PresetMonAbility_Has : sText_PresetMonAbility_Missing);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }

        // Item
        if(preset->heldItem == ITEM_NONE)
        {
            StringCopyN(gStringVar1, gText_None, ITEM_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, sText_PresetMonItem_Has);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }
        else
        {
            StringCopyN(gStringVar1, ItemId_GetName(preset->heldItem), ITEM_NAME_LENGTH);
            StringExpandPlaceholders(gStringVar4, GetMonData(mon, MON_DATA_HELD_ITEM) == preset->heldItem ? sText_PresetMonItem_Has : sText_PresetMonItem_Missing);
            AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
        }

        // Nature
        StringCopy(gStringVar1, gNatureNamePointers[preset->nature]);
        StringExpandPlaceholders(gStringVar4, GetNature(mon) == preset->nature ? sText_PresetMonNature_Has : sText_PresetMonNature_Missing);
        AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);

        // Moves
        for(i = 0; i < MAX_MON_MOVES; ++i)
        {
            u16 moveId = preset->moves[i];

            if(moveId != MOVE_NONE)
            {
                StringCopy(gStringVar1, gMoveNames[moveId]);
                StringExpandPlaceholders(gStringVar4, MonKnowsMove(mon, moveId) ? sText_PresetMonMove_Has : sText_PresetMonMove_Missing);
                AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
            }
        }
    }
    else
    {
        AddTextPrinterParameterized(windowId, FONT_SMALL, sText_PresetMonNoData, 2, 12 * (line++), TEXT_SKIP_DRAW, NULL);
    }

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void Task_DisplayRecommendedMonSetInput(u8 taskId)
{
    u8 windowId = gTasks[taskId].data[0];

    if(JOY_NEW(A_BUTTON) || JOY_NEW(B_BUTTON))
    {
        gSpecialVar_0x8004 = PARTY_SIZE;

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else if(JOY_NEW(DPAD_LEFT))
    {
        if(gSpecialVar_0x8004 == 0)
            gSpecialVar_0x8004 = gPlayerPartyCount - 1;
        else
            --gSpecialVar_0x8004; 

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
    else if(JOY_NEW(DPAD_RIGHT))
    {
        gSpecialVar_0x8004 = (gSpecialVar_0x8004 + 1 ) % gPlayerPartyCount;

        ClearStdWindowAndFrame(windowId, TRUE);
        RemoveWindow(windowId);

        ScriptContext_Enable();
        DestroyTask(taskId);
    }
}

static u32 CalculatePresetDisplayScore(struct Pokemon* mon, struct RoguePokemonCompetitiveSet const* preset)
{
    u8 i;
    u32 score = 0;
    u32 temp;

#ifdef ROGUE_EXPANSION
    if(GetNature(mon) == preset->nature)
        score += 3;

    if(GetMonAbility(mon) == preset->ability)
        score += 3;
#else
    // Rate much higher, as cannot change in Vanilla
    if(GetNature(mon) == preset->nature)
        score += 6;

    if(GetMonAbility(mon) == preset->ability)
        score += 6;
#endif

    temp = GetMonData(mon, MON_DATA_HELD_ITEM);
    if(temp == preset->heldItem)
        score += 1;

#ifdef ROGUE_EXPANSION
    if((IS_MEGA_STONE_ITEM(temp) || temp == ITEM_RED_ORB || temp == ITEM_BLUE_ORB) && !IsMegaEvolutionEnabled())
    {
        return 1;
    }

    if(temp >= ITEM_NORMALIUM_Z && temp <= ITEM_ULTRANECROZIUM_Z && !IsZMovesEnabled())
    {
        return 1;
    }
#endif

    for(i = 0; i < MAX_MON_MOVES; ++i)
    {
        u16 moveId = preset->moves[i];

        if(moveId != MOVE_NONE)
        {
            if(MonKnowsMove(mon, moveId))
                moveId += 2;
        }
    }

    return score * 5;
}

static struct RoguePokemonCompetitiveSet const* SelectMonPreset(struct Pokemon* mon)
{
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);

    if(gRoguePokemonProfiles[species].competitiveSetCount != 0)
    {
        u16 i;
        u16 bestIdx = 0;
        u32 bestScore = CalculatePresetDisplayScore(mon, &gRoguePokemonProfiles[species].competitiveSets[0]);

        for(i = 1; i < gRoguePokemonProfiles[species].competitiveSetCount; ++i)
        {
            u32 score = CalculatePresetDisplayScore(mon, &gRoguePokemonProfiles[species].competitiveSets[i]);

            if(score > bestScore)
            {
                bestIdx = i;
                bestScore = score;
            }
        }

        return &gRoguePokemonProfiles[species].competitiveSets[bestIdx];
    }

    return NULL;
}

void ScriptMenu_DisplayRecommendedMonSet()
{
    u8 taskId;
    struct Pokemon* mon = &gPlayerParty[gSpecialVar_0x8004];
    struct RoguePokemonCompetitiveSet const* preset = SelectMonPreset(mon);
    u8 windowId = CreateWindowFromRectWithBaseBlockOffset(12, 1, 14, 11, 8 * 8);

    PrintRecommendedMonSetToWindow(windowId, mon, preset);

    taskId = CreateTask(Task_DisplayRecommendedMonSetInput, 0);
    gTasks[taskId].data[0] = windowId;
}

static void Task_ShowItemDescriptionInput(u8 taskId)
{
}

static u8 const sText_ItemName[] = _("{COLOR BLUE}{STR_VAR_1}");

static void PrintItemDescriptionToWindow(u8 windowId, u16 itemId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    StringCopy(gStringVar1, ItemId_GetName(itemId));
    StringExpandPlaceholders(gStringVar4, sText_ItemName);

    gTextFlags.replaceScrollWithNewLine = TRUE;
    AddTextPrinterParameterized(windowId, FONT_NORMAL, gStringVar4, 0, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NORMAL, ItemId_GetDescription(itemId), 0, 14, TEXT_SKIP_DRAW, NULL);
    gTextFlags.replaceScrollWithNewLine = FALSE;

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

void ScriptMenu_ShowItemDescription()
{
    u8 taskId;
    u8 windowId = CreateWindowFromRect(1, 4, 13, 8);

    PrintItemDescriptionToWindow(windowId, gSpecialVar_0x8004);

    taskId = CreateTask(Task_ShowItemDescriptionInput, 0);
    gTasks[taskId].data[0] = windowId;
}

void ScriptMenu_HideItemDescription()
{
    u8 taskId = FindTaskIdByFunc(Task_ShowItemDescriptionInput);

    if (taskId == TASK_NONE)
        return;

    ClearStdWindowAndFrame(gTasks[taskId].data[0], TRUE);
    RemoveWindow(gTasks[taskId].data[0]);
    DestroyTask(taskId);
}

static u8 const sText_RogueAssistant[] = _("{COLOR BLUE}Rogue Assistant");
static u8 const sText_RogueAssistantInfo[] = _("Download from:\n{COLOR BLUE}https://rogue.assist.pokabbie.com\n\n{COLOR RED}Never download from other links!");

static void PrintRogueAssistantNoticToWindow(u8 windowId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetStandardWindowBorderStyle(windowId, 0);

    AddTextPrinterParameterized(windowId, FONT_NORMAL, sText_RogueAssistant, 0, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_NARROW, sText_RogueAssistantInfo, 0, 14, TEXT_SKIP_DRAW, NULL);

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void Task_ShowRogueAssistantNoticeInput(u8 taskId)
{
}

void ScriptMenu_ShowRogueAssistantNotice()
{
    u8 taskId;
    u8 windowId = CreateWindowFromRect(4, 1, 20, 10);

    PrintRogueAssistantNoticToWindow(windowId);

    taskId = CreateTask(Task_ShowRogueAssistantNoticeInput, 0);
    gTasks[taskId].data[0] = windowId;
}

void ScriptMenu_HideRogueAssistantNotice()
{
    u8 taskId = FindTaskIdByFunc(Task_ShowRogueAssistantNoticeInput);

    if (taskId == TASK_NONE)
        return;

    ClearStdWindowAndFrame(gTasks[taskId].data[0], TRUE);
    RemoveWindow(gTasks[taskId].data[0]);
    DestroyTask(taskId);
}

static const u8 sText_TrialOverviewDifficulty[] = _("{COLOR BLUE}Difficulty:");
static const u8 sText_TrialOverviewPokedex[] = _("{COLOR BLUE}Pokédex:");
static const u8 sText_TrialOverviewRules[] = _("{COLOR BLUE}Rules ");
static const u8 sText_TrialOverviewPageSeparator[] = _("/");
static const u8 sText_TrialOverviewPageControls[] = _("  L/R");

#define TRIAL_OVERVIEW_RULES_PER_PAGE 3
#define TRIAL_OVERVIEW_MAX_RULE_LINES 16
#define TRIAL_OVERVIEW_RULE_LINE_LENGTH 128
#define TRIAL_OVERVIEW_RULE_WIDTH 192
#define tTrialOverviewWindowId data[0]
#define tTrialOverviewDelay    data[1]
#define tTrialOverviewPage     data[2]
#define tTrialOverviewState    data[3]

enum
{
    TRIAL_OVERVIEW_STATE_WAIT_FOR_BG,
    TRIAL_OVERVIEW_STATE_WAIT_FOR_GFX,
    TRIAL_OVERVIEW_STATE_INPUT,
};

static EWRAM_DATA u8 sTrialOverviewRuleLines[TRIAL_OVERVIEW_MAX_RULE_LINES][TRIAL_OVERVIEW_RULE_LINE_LENGTH];

static u8 BufferTrialOverviewRuleLines(void)
{
    u8 ruleIndex;
    u8 lineCount = 0;
    u8 ruleCount = RogueTrial_GetRuleCount(gSpecialVar_0x8004, gSpecialVar_0x8006);

    for (ruleIndex = 0; ruleIndex < ruleCount && lineCount < TRIAL_OVERVIEW_MAX_RULE_LINES; ++ruleIndex)
    {
        const u8 *src = RogueTrial_GetRuleText(gSpecialVar_0x8004, gSpecialVar_0x8006, ruleIndex);
        u8 lineLength = 0;

        sTrialOverviewRuleLines[lineCount][0] = EOS;

        while (*src != EOS && lineCount < TRIAL_OVERVIEW_MAX_RULE_LINES)
        {
            u8 wordLength = 0;
            u8 candidate[TRIAL_OVERVIEW_RULE_LINE_LENGTH];

            while (src[wordLength] != EOS && src[wordLength] != CHAR_SPACE)
                ++wordLength;

            if (lineLength != 0)
            {
                memcpy(candidate, sTrialOverviewRuleLines[lineCount], lineLength);
                candidate[lineLength] = CHAR_SPACE;
                memcpy(&candidate[lineLength + 1], src, wordLength);
                candidate[lineLength + wordLength + 1] = EOS;
            }
            else
            {
                memcpy(candidate, src, wordLength);
                candidate[wordLength] = EOS;
            }

            if (lineLength != 0 && GetStringWidth(FONT_SMALL_NARROW, candidate, 0) > TRIAL_OVERVIEW_RULE_WIDTH)
            {
                ++lineCount;
                lineLength = 0;
                if (lineCount >= TRIAL_OVERVIEW_MAX_RULE_LINES)
                    break;
                continue;
            }

            StringCopy(sTrialOverviewRuleLines[lineCount], candidate);
            lineLength = StringLength(candidate);
            src += wordLength;
            if (*src == CHAR_SPACE)
                ++src;
        }

        if (lineCount < TRIAL_OVERVIEW_MAX_RULE_LINES && lineLength != 0)
            ++lineCount;
    }

    return lineCount;
}

static void PrintTrialOverview(u8 taskId)
{
    u8 i;
    u8 pageText[32];
    u8 *dest;
    u8 windowId = gTasks[taskId].tTrialOverviewWindowId;
    u8 lineCount = BufferTrialOverviewRuleLines();
    u8 pageCount = max(1, (lineCount + TRIAL_OVERVIEW_RULES_PER_PAGE - 1) / TRIAL_OVERVIEW_RULES_PER_PAGE);
    u8 firstLine = gTasks[taskId].tTrialOverviewPage * TRIAL_OVERVIEW_RULES_PER_PAGE;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));

    AddTextPrinterParameterized(windowId, FONT_NORMAL, gStringVar1, 4, 0, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, sText_TrialOverviewDifficulty, 4, 18, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar4, 72, 18, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, sText_TrialOverviewPokedex, 4, 32, TEXT_SKIP_DRAW, NULL);
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, gStringVar3, 72, 32, TEXT_SKIP_DRAW, NULL);

    dest = StringCopy(pageText, sText_TrialOverviewRules);
    dest = ConvertIntToDecimalStringN(dest, gTasks[taskId].tTrialOverviewPage + 1, STR_CONV_MODE_LEFT_ALIGN, 1);
    dest = StringAppend(dest, sText_TrialOverviewPageSeparator);
    dest = ConvertIntToDecimalStringN(dest, pageCount, STR_CONV_MODE_LEFT_ALIGN, 1);
    if (pageCount > 1)
        StringAppend(dest, sText_TrialOverviewPageControls);
    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, pageText, 4, 48, TEXT_SKIP_DRAW, NULL);

    for (i = 0; i < TRIAL_OVERVIEW_RULES_PER_PAGE && firstLine + i < lineCount; ++i)
    {
        AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, sTrialOverviewRuleLines[firstLine + i], 8, 64 + i * 14, TEXT_SKIP_DRAW, NULL);
    }

    CopyWindowToVram(windowId, COPYWIN_GFX);
}

static void CloseTrialOverview(u8 taskId, bool8 proceed)
{
    gSpecialVar_Result = proceed;
    PlaySE(SE_SELECT);
    ClearToTransparentAndRemoveWindow(gTasks[taskId].tTrialOverviewWindowId);
    DestroyTask(taskId);
    ScriptContext_Enable();
}

static void Task_ShowTrialOverviewInput(u8 taskId)
{
    u8 lineCount;
    u8 pageCount;

    switch (gTasks[taskId].tTrialOverviewState)
    {
    case TRIAL_OVERVIEW_STATE_WAIT_FOR_BG:
        // The preceding closemessage may still be copying this background's
        // tilemap. Wait before modifying it so the border cannot be revealed
        // before the window graphics are ready.
        if (IsDma3ManagerBusyWithBgCopy())
            return;

        gTasks[taskId].tTrialOverviewWindowId = CreateWindowFromRect(1, 2, 26, 14);
        SetDarkStandardWindowBorderStyle(gTasks[taskId].tTrialOverviewWindowId, FALSE);
        PrintTrialOverview(taskId);
        gTasks[taskId].tTrialOverviewState = TRIAL_OVERVIEW_STATE_WAIT_FOR_GFX;
        return;
    case TRIAL_OVERVIEW_STATE_WAIT_FOR_GFX:
        if (IsDma3ManagerBusyWithBgCopy())
            return;

        CopyWindowToVram(gTasks[taskId].tTrialOverviewWindowId, COPYWIN_MAP);
        gTasks[taskId].tTrialOverviewState = TRIAL_OVERVIEW_STATE_INPUT;
        return;
    }

    if (gTasks[taskId].tTrialOverviewDelay < 5)
    {
        ++gTasks[taskId].tTrialOverviewDelay;
        return;
    }

    lineCount = BufferTrialOverviewRuleLines();
    pageCount = max(1, (lineCount + TRIAL_OVERVIEW_RULES_PER_PAGE - 1) / TRIAL_OVERVIEW_RULES_PER_PAGE);

    if (pageCount > 1 && JOY_NEW(L_BUTTON | R_BUTTON))
    {
        PlaySE(SE_SELECT);
        if (JOY_NEW(R_BUTTON))
            gTasks[taskId].tTrialOverviewPage = (gTasks[taskId].tTrialOverviewPage + 1) % pageCount;
        else if (gTasks[taskId].tTrialOverviewPage == 0)
            gTasks[taskId].tTrialOverviewPage = pageCount - 1;
        else
            --gTasks[taskId].tTrialOverviewPage;
        PrintTrialOverview(taskId);
    }
    else if (JOY_NEW(A_BUTTON))
        CloseTrialOverview(taskId, TRUE);
    else if (JOY_NEW(B_BUTTON))
        CloseTrialOverview(taskId, FALSE);
}

void ScriptMenu_ShowTrialOverview(void)
{
    u8 taskId = CreateTask(Task_ShowTrialOverviewInput, 0);

    gTasks[taskId].tTrialOverviewWindowId = WINDOW_NONE;
    gTasks[taskId].tTrialOverviewDelay = 0;
    gTasks[taskId].tTrialOverviewPage = 0;
    gTasks[taskId].tTrialOverviewState = TRIAL_OVERVIEW_STATE_WAIT_FOR_BG;
}

#undef tTrialOverviewWindowId
#undef tTrialOverviewDelay
#undef tTrialOverviewPage
#undef tTrialOverviewState
#undef TRIAL_OVERVIEW_RULE_WIDTH
#undef TRIAL_OVERVIEW_RULE_LINE_LENGTH
#undef TRIAL_OVERVIEW_MAX_RULE_LINES
#undef TRIAL_OVERVIEW_RULES_PER_PAGE

#define SAFARI_OFFER_RESULT_BUY 10
#define SAFARI_OFFER_RESULT_DISMISS 11
#define SAFARI_OFFER_RESULT_BACK 12
#define SAFARI_OFFER_MON_PAL_TAG 0xF50F
#define SAFARI_OFFER_MON_PAL_NUM 12
#define SAFARI_OFFER_MAX_ITEM_ICONS ROGUE_SAFARI_OFFER_MAX_COST_ITEMS
#define SAFARI_OFFER_ITEM_ICON_TILE_TAG_BASE 0xF510
#define SAFARI_OFFER_ITEM_ICON_PAL_TAG 0xF520
#define SAFARI_OFFER_ITEM_ICON_BLACK_INDEX 1
#define SAFARI_OFFER_ITEM_ICON_SLOT_BASE(i) (2 + (i) * 4)
#define SAFARI_OFFER_PIC_WINDOW_TILES (8 * 8)
#define SAFARI_OFFER_DETAILS_WINDOW_TILES (17 * 11)
#define SAFARI_OFFER_ACTIONS_WINDOW_TILES (26 * 3)
#define SAFARI_OFFER_DETAILS_SCREEN_X 88
#define SAFARI_OFFER_DETAILS_SCREEN_Y 16
#define SAFARI_OFFER_DETAILS_WIDTH 136
#define SAFARI_OFFER_COST_LABEL_Y 30
#define SAFARI_OFFER_COST_ICON_Y 42
#define SAFARI_OFFER_COST_COUNT_Y 66
#define SAFARI_OFFER_ACTIONS_LEFT_SHIFT 8

#define tSafariOfferPicWindow    data[0]
#define tSafariOfferDetailsWindow data[1]
#define tSafariOfferActionsWindow data[2]
#define tSafariOfferMonSpriteId  data[3]
#define tSafariOfferItemSpriteId(i) data[4 + (i)]
#define tSafariOfferCanBuy       data[7]
#define tSafariOfferCanDismiss   data[8]
#define tSafariOfferSafariIndex  data[9]

static const u8 sSafariOfferTextColor[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_DARK_GRAY, TEXT_COLOR_LIGHT_GRAY};
static const u8 sSafariOfferRedColor[] = {TEXT_COLOR_TRANSPARENT, TEXT_COLOR_RED, TEXT_COLOR_LIGHT_GRAY};
static const u8 sText_SafariOfferNoCost[] = _("No offer is available.");
static const u8 sText_SafariOfferCanBuy[] = _("{A_BUTTON} Invite");
static const u8 sText_SafariOfferCannotBuy[] = _("{COLOR RED}{A_BUTTON} Invite");
static const u8 sText_SafariOfferDismiss[] = _("{START_BUTTON} Dismiss");
static const u8 sText_SafariOfferBack[] = _("{B_BUTTON} Back");
static const u8 sText_SafariOfferShinyCost[] = _("Shiny");

static bool8 TryCycleSafariOfferDetails(u8 taskId, bool8 forward);
static bool8 RefreshSafariOfferDetails(u8 taskId, u16 safariIndex);

static void DestroySafariOfferMonSprite(u8 taskId)
{
    if (gTasks[taskId].tSafariOfferMonSpriteId != SPRITE_NONE)
    {
        FreeSpriteOamMatrix(&gSprites[gTasks[taskId].tSafariOfferMonSpriteId]);
        FreeAndDestroyMonPicSprite(gTasks[taskId].tSafariOfferMonSpriteId);
        gTasks[taskId].tSafariOfferMonSpriteId = SPRITE_NONE;
    }
}

static void DestroySafariOfferItemIcons(u8 taskId)
{
    u8 i;

    for (i = 0; i < SAFARI_OFFER_MAX_ITEM_ICONS; ++i)
    {
        if (gTasks[taskId].tSafariOfferItemSpriteId(i) != SPRITE_NONE)
        {
            DestroySprite(&gSprites[gTasks[taskId].tSafariOfferItemSpriteId(i)]);
            FreeSpriteTilesByTag(SAFARI_OFFER_ITEM_ICON_TILE_TAG_BASE + i);
            gTasks[taskId].tSafariOfferItemSpriteId(i) = SPRITE_NONE;
        }
    }
    FreeSpritePaletteByTag(SAFARI_OFFER_ITEM_ICON_PAL_TAG);
}

static void CleanupSafariOfferDetails(u8 taskId)
{
    DestroySafariOfferMonSprite(taskId);
    DestroySafariOfferItemIcons(taskId);

    ClearToTransparentAndRemoveWindow(gTasks[taskId].tSafariOfferPicWindow);
    ClearToTransparentAndRemoveWindow(gTasks[taskId].tSafariOfferDetailsWindow);
    ClearToTransparentAndRemoveWindow(gTasks[taskId].tSafariOfferActionsWindow);

    ScriptContext_Enable();
    DestroyTask(taskId);
}

static void Task_SafariOfferDetailsInput(u8 taskId)
{
    if (JOY_REPEAT(DPAD_LEFT))
    {
        if (TryCycleSafariOfferDetails(taskId, FALSE))
            PlaySE(SE_DEX_SCROLL);
        else
            PlaySE(SE_FAILURE);
    }
    else if (JOY_REPEAT(DPAD_RIGHT))
    {
        if (TryCycleSafariOfferDetails(taskId, TRUE))
            PlaySE(SE_DEX_SCROLL);
        else
            PlaySE(SE_FAILURE);
    }
    else if (JOY_NEW(A_BUTTON) && gTasks[taskId].tSafariOfferCanBuy)
    {
        gSpecialVar_Result = SAFARI_OFFER_RESULT_BUY;
        CleanupSafariOfferDetails(taskId);
    }
    else if (JOY_NEW(START_BUTTON) && gTasks[taskId].tSafariOfferCanDismiss)
    {
        gSpecialVar_Result = SAFARI_OFFER_RESULT_DISMISS;
        CleanupSafariOfferDetails(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        gSpecialVar_Result = SAFARI_OFFER_RESULT_BACK;
        CleanupSafariOfferDetails(taskId);
    }
}

static void BufferSafariOfferCostCount(u8 *countText, struct RogueSafariOfferCostItem const *cost)
{
    u8 *dest = countText;

    dest = ConvertUIntToDecimalStringN(dest, cost->ownedCount, STR_CONV_MODE_LEFT_ALIGN, 3);
    dest = StringAppend(dest, gText_Slash);
    dest = ConvertUIntToDecimalStringN(dest, cost->requiredCount, STR_CONV_MODE_LEFT_ALIGN, 3);
    *dest = EOS;
}

static void PrintSafariOfferCostCount(u8 windowId, u8 iconCenterX, u8 y, struct RogueSafariOfferCostItem const *cost)
{
    u8 countText[12];
    s16 textWidth;
    s16 x;
    bool8 hasEnough = cost->ownedCount >= cost->requiredCount;

    BufferSafariOfferCostCount(countText, cost);
    textWidth = GetStringWidth(FONT_SMALL_NARROW, countText, 0);
    x = iconCenterX - (textWidth / 2);

    AddTextPrinterParameterized4(
        windowId,
        FONT_SMALL_NARROW,
        x,
        y,
        0,
        0,
        hasEnough ? sSafariOfferTextColor : sSafariOfferRedColor,
        TEXT_SKIP_DRAW,
        countText);
}

static u8 GetSafariOfferCostIconCenterX(u8 costCount, u8 index)
{
    static const u8 sCostColumnCenters[ROGUE_SAFARI_OFFER_MAX_COST_ITEMS][ROGUE_SAFARI_OFFER_MAX_COST_ITEMS] =
    {
        { 68, 0, 0 },
        { 48, 88, 0 },
        { 34, 68, 102 },
    };

    if (costCount == 0)
        return SAFARI_OFFER_DETAILS_WIDTH / 2;

    if (costCount > ROGUE_SAFARI_OFFER_MAX_COST_ITEMS)
        costCount = ROGUE_SAFARI_OFFER_MAX_COST_ITEMS;

    if (index >= costCount)
        index = costCount - 1;

    return sCostColumnCenters[costCount - 1][index];
}

static void PrintSafariOfferCenteredText(u8 windowId, u8 fontId, u8 y, const u8 *text)
{
    s16 width = GetStringWidth(fontId, text, 0);
    s16 x = (SAFARI_OFFER_DETAILS_WIDTH - width) / 2;

    if (x < 0)
        x = 0;

    AddTextPrinterParameterized4(windowId, fontId, x, y, 0, 0, sSafariOfferTextColor, TEXT_SKIP_DRAW, text);
}

static void PrintSafariOfferCenteredColumnText(u8 windowId, u8 iconCenterX, u8 y, const u8 *text)
{
    s16 width;
    s16 x;

    if (text == NULL)
        return;

    width = GetStringWidth(FONT_SMALL_NARROW, text, 0);
    x = iconCenterX - (width / 2);

    if (x < 0)
        x = 0;
    else if (x + width > SAFARI_OFFER_DETAILS_WIDTH)
        x = SAFARI_OFFER_DETAILS_WIDTH - width;

    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, x, y, 0, 0, sSafariOfferTextColor, TEXT_SKIP_DRAW, text);
}

static void PrintSafariOfferActionText(u8 windowId, const u8 *text, u8 centerX)
{
    s16 width = GetStringWidth(FONT_SMALL_NARROW, text, 0);
    s16 x = centerX - (width / 2);

    if (x < 0)
        x = 0;

    AddTextPrinterParameterized(windowId, FONT_SMALL_NARROW, text, x, 5, TEXT_SKIP_DRAW, NULL);
}

static const u8 *GetSafariOfferCostLabel(struct RogueSafariOfferCostItem const *cost)
{
    if (cost->isShinyCost)
        return sText_SafariOfferShinyCost;
    if (IS_STANDARD_TYPE(cost->type))
        return gTypeNames[cost->type];
    return NULL;
}

static void PrintSafariOfferActions(u8 windowId, bool8 hasOffer, bool8 canPurchase, bool8 canDismiss)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetDarkStandardWindowBorderStyle(windowId, FALSE);

    if (hasOffer)
        PrintSafariOfferActionText(windowId, canPurchase ? sText_SafariOfferCanBuy : sText_SafariOfferCannotBuy, 38 - SAFARI_OFFER_ACTIONS_LEFT_SHIFT);

    if (canDismiss)
        PrintSafariOfferActionText(windowId, sText_SafariOfferDismiss, 116 - SAFARI_OFFER_ACTIONS_LEFT_SHIFT);

    PrintSafariOfferActionText(windowId, sText_SafariOfferBack, 184 - SAFARI_OFFER_ACTIONS_LEFT_SHIFT);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void PrintSafariOfferDetails(u8 windowId, struct RogueSafariOfferDetails const *details)
{
    u8 i;

    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetDarkStandardWindowBorderStyle(windowId, FALSE);
    AddTextPrinterParameterized4(windowId, FONT_SMALL_NARROW, 2, 4, 0, 0, sSafariOfferTextColor, TEXT_SKIP_DRAW, details->displayName);

    if (details->costCount == 0)
    {
        PrintSafariOfferCenteredText(windowId, FONT_SMALL_NARROW, 40, sText_SafariOfferNoCost);
    }
    else
    {
        for (i = 0; i < details->costCount; ++i)
        {
            u8 iconCenterX = GetSafariOfferCostIconCenterX(details->costCount, i);

            PrintSafariOfferCenteredColumnText(
                windowId,
                iconCenterX,
                SAFARI_OFFER_COST_LABEL_Y,
                GetSafariOfferCostLabel(&details->costs[i]));
            PrintSafariOfferCostCount(
                windowId,
                iconCenterX,
                SAFARI_OFFER_COST_COUNT_Y,
                &details->costs[i]);
        }
    }

    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static void InitSafariOfferItemIconPaletteMap(u8 *paletteMap, u8 index, bool8 isShinyCost)
{
    u8 i;
    u8 slotBase = SAFARI_OFFER_ITEM_ICON_SLOT_BASE(index);

    for (i = 0; i < 16; ++i)
        paletteMap[i] = 0;

    if (isShinyCost)
    {
        paletteMap[5] = slotBase;
        paletteMap[6] = slotBase;
        paletteMap[7] = slotBase + 1;
        paletteMap[8] = slotBase + 1;
        paletteMap[9] = slotBase + 2;
        paletteMap[10] = slotBase + 1;
        paletteMap[11] = slotBase + 2;
        paletteMap[13] = slotBase + 3;
        paletteMap[15] = SAFARI_OFFER_ITEM_ICON_BLACK_INDEX;
    }
    else
    {
        paletteMap[10] = slotBase;
        paletteMap[11] = slotBase + 1;
        paletteMap[13] = slotBase + 2;
        paletteMap[15] = SAFARI_OFFER_ITEM_ICON_BLACK_INDEX;
    }
}

static bool8 LoadSafariOfferItemIconPalette(struct RogueSafariOfferDetails const *details)
{
    u8 i;
    u16 palette[16] = {0};
    struct SpritePalette spritePalette = {palette, SAFARI_OFFER_ITEM_ICON_PAL_TAG};

    palette[SAFARI_OFFER_ITEM_ICON_BLACK_INDEX] = RGB_BLACK;

    for (i = 0; i < details->costCount; ++i)
    {
        u8 slotBase = SAFARI_OFFER_ITEM_ICON_SLOT_BASE(i);
        u16 *itemPalette;

        LZDecompressWram(GetItemIconPicOrPalette(details->costs[i].itemId, 1), gPaletteDecompressionBuffer);
        itemPalette = (u16 *)gPaletteDecompressionBuffer;

        if (details->costs[i].isShinyCost)
        {
            palette[slotBase] = itemPalette[5];
            palette[slotBase + 1] = itemPalette[7];
            palette[slotBase + 2] = itemPalette[9];
            palette[slotBase + 3] = itemPalette[13];
        }
        else
        {
            palette[slotBase] = itemPalette[10];
            palette[slotBase + 1] = itemPalette[11];
            palette[slotBase + 2] = itemPalette[13];
            palette[slotBase + 3] = itemPalette[13];
        }
    }

    FreeSpritePaletteByTag(SAFARI_OFFER_ITEM_ICON_PAL_TAG);
    return LoadSpritePalette(&spritePalette) != 0xFF;
}

static void DrawSafariOfferItemIcons(u8 taskId, struct RogueSafariOfferDetails const *details)
{
    u8 i;

    for (i = 0; i < SAFARI_OFFER_MAX_ITEM_ICONS; ++i)
        gTasks[taskId].tSafariOfferItemSpriteId(i) = SPRITE_NONE;

    if (details->costCount == 0 || !LoadSafariOfferItemIconPalette(details))
        return;

    for (i = 0; i < details->costCount; ++i)
    {
        u8 spriteId;
        u8 paletteMap[16];
        u16 tileTag = SAFARI_OFFER_ITEM_ICON_TILE_TAG_BASE + i;

        InitSafariOfferItemIconPaletteMap(paletteMap, i, details->costs[i].isShinyCost);
        FreeSpriteTilesByTag(tileTag);
        spriteId = AddRemappedItemIconSprite(tileTag, SAFARI_OFFER_ITEM_ICON_PAL_TAG, details->costs[i].itemId, paletteMap);

        if (spriteId != MAX_SPRITES)
        {
            gSprites[spriteId].x = SAFARI_OFFER_DETAILS_SCREEN_X + GetSafariOfferCostIconCenterX(details->costCount, i) + 4;
            gSprites[spriteId].y = SAFARI_OFFER_DETAILS_SCREEN_Y + SAFARI_OFFER_COST_ICON_Y + 16;
            gSprites[spriteId].oam.priority = 0;
            gTasks[taskId].tSafariOfferItemSpriteId(i) = spriteId;
        }
        else
        {
            FreeSpriteTilesByTag(tileTag);
        }
    }
}

static void DrawSafariOfferPicWindow(u8 windowId)
{
    FillWindowPixelBuffer(windowId, PIXEL_FILL(1));
    SetDarkStandardWindowBorderStyle(windowId, FALSE);
    CopyWindowToVram(windowId, COPYWIN_FULL);
}

static u8 CreateSafariOfferMonSprite(struct RogueSafariOfferDetails const *details)
{
    u16 spriteId;

    FreeSpritePaletteByTag(SAFARI_OFFER_MON_PAL_TAG);
    spriteId = CreateMonPicSprite_Affine(
        details->picSpecies,
        details->otId,
        details->personality,
        details->gender,
        details->isShiny,
        MON_PIC_AFFINE_FRONT | F_MON_PIC_NO_AFFINE,
        48,
        48,
        SAFARI_OFFER_MON_PAL_NUM,
        SAFARI_OFFER_MON_PAL_TAG);

    if (spriteId == 0xFFFF)
        return SPRITE_NONE;

    if (spriteId != SPRITE_NONE)
    {
        gSprites[spriteId].callback = SpriteCallbackDummy;
        gSprites[spriteId].oam.priority = 0;
    }

    return (u8)spriteId;
}

static bool8 RefreshSafariOfferDetails(u8 taskId, u16 safariIndex)
{
    struct RogueSafariOfferDetails details;

    if (!Rogue_GetSafariMonOfferDetails(safariIndex, &details))
        return FALSE;

    gSpecialVar_0x8008 = safariIndex;
    VarSet(VAR_TEMP_A, safariIndex);
    gTasks[taskId].tSafariOfferSafariIndex = safariIndex;

    DestroySafariOfferMonSprite(taskId);
    DestroySafariOfferItemIcons(taskId);

    DrawSafariOfferPicWindow(gTasks[taskId].tSafariOfferPicWindow);
    PrintSafariOfferDetails(gTasks[taskId].tSafariOfferDetailsWindow, &details);
    PrintSafariOfferActions(gTasks[taskId].tSafariOfferActionsWindow, details.costCount != 0, details.canPurchase, TRUE);

    gTasks[taskId].tSafariOfferMonSpriteId = CreateSafariOfferMonSprite(&details);
    gTasks[taskId].tSafariOfferCanBuy = details.canPurchase;
    gTasks[taskId].tSafariOfferCanDismiss = TRUE;

    DrawSafariOfferItemIcons(taskId, &details);
    return TRUE;
}

static bool8 TryCycleSafariOfferDetails(u8 taskId, bool8 forward)
{
    u16 i;
    u16 index = gTasks[taskId].tSafariOfferSafariIndex;
    u16 start = (index >= ROGUE_SAFARI_LEGENDS_START_INDEX) ? ROGUE_SAFARI_LEGENDS_START_INDEX : 0;
    u16 end = (index >= ROGUE_SAFARI_LEGENDS_START_INDEX) ? ROGUE_SAFARI_TOTAL_MONS : ROGUE_SAFARI_LEGENDS_START_INDEX;
    u16 count = end - start;
    struct RogueSafariOfferDetails details;

    for (i = 1; i < count; ++i)
    {
        if (forward)
        {
            ++index;
            if (index >= end)
                index = start;
        }
        else
        {
            if (index == start)
                index = end - 1;
            else
                --index;
        }

        if (Rogue_GetSafariMonOfferDetails(index, &details))
            return RefreshSafariOfferDetails(taskId, index);
    }

    return FALSE;
}

void ScriptMenu_ShowSafariOfferDetails(void)
{
    u8 i;
    u8 taskId;
    u8 picWindowId;
    u8 detailsWindowId;
    u8 actionsWindowId;
    struct RogueSafariOfferDetails details;

    gSpecialVar_Result = MULTI_B_PRESSED;

    if (!Rogue_GetSafariMonOfferDetails(gSpecialVar_0x8008, &details))
    {
        ScriptContext_Enable();
        return;
    }

    picWindowId = CreateWindowFromRectWithBaseBlockOffset(1, 1, 8, 8, 0);
    detailsWindowId = CreateWindowFromRectWithBaseBlockOffset(10, 1, 17, 11, SAFARI_OFFER_PIC_WINDOW_TILES);
    actionsWindowId = CreateWindowFromRectWithBaseBlockOffset(1, 13, 26, 3, SAFARI_OFFER_PIC_WINDOW_TILES + SAFARI_OFFER_DETAILS_WINDOW_TILES);

    taskId = CreateTask(Task_SafariOfferDetailsInput, 0);
    gTasks[taskId].tSafariOfferPicWindow = picWindowId;
    gTasks[taskId].tSafariOfferDetailsWindow = detailsWindowId;
    gTasks[taskId].tSafariOfferActionsWindow = actionsWindowId;
    gTasks[taskId].tSafariOfferMonSpriteId = SPRITE_NONE;
    for (i = 0; i < SAFARI_OFFER_MAX_ITEM_ICONS; ++i)
        gTasks[taskId].tSafariOfferItemSpriteId(i) = SPRITE_NONE;
    gTasks[taskId].tSafariOfferSafariIndex = gSpecialVar_0x8008;
    gTasks[taskId].tSafariOfferCanBuy = FALSE;
    gTasks[taskId].tSafariOfferCanDismiss = TRUE;
    RefreshSafariOfferDetails(taskId, gSpecialVar_0x8008);
}

#undef tSafariOfferPicWindow
#undef tSafariOfferDetailsWindow
#undef tSafariOfferActionsWindow
#undef tSafariOfferMonSpriteId
#undef tSafariOfferItemSpriteId
#undef tSafariOfferCanBuy
#undef tSafariOfferCanDismiss
#undef tSafariOfferSafariIndex
#undef SAFARI_OFFER_ITEM_ICON_TILE_TAG_BASE
#undef SAFARI_OFFER_ITEM_ICON_PAL_TAG
#undef SAFARI_OFFER_ITEM_ICON_BLACK_INDEX
#undef SAFARI_OFFER_ITEM_ICON_SLOT_BASE
#undef SAFARI_OFFER_PIC_WINDOW_TILES
#undef SAFARI_OFFER_DETAILS_WINDOW_TILES
#undef SAFARI_OFFER_ACTIONS_WINDOW_TILES
#undef SAFARI_OFFER_DETAILS_SCREEN_X
#undef SAFARI_OFFER_DETAILS_SCREEN_Y
#undef SAFARI_OFFER_DETAILS_WIDTH
#undef SAFARI_OFFER_COST_LABEL_Y
#undef SAFARI_OFFER_COST_ICON_Y
#undef SAFARI_OFFER_COST_COUNT_Y
#undef SAFARI_OFFER_ACTIONS_LEFT_SHIFT
#undef SAFARI_OFFER_RESULT_BUY
#undef SAFARI_OFFER_RESULT_DISMISS
#undef SAFARI_OFFER_RESULT_BACK
#undef SAFARI_OFFER_MON_PAL_TAG
#undef SAFARI_OFFER_MON_PAL_NUM
#undef SAFARI_OFFER_MAX_ITEM_ICONS
