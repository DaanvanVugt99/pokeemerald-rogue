#include "global.h"
#include "decompress.h"
#include "main.h"
#include "malloc.h"
#include "constants/items.h"
#include "constants/party_menu.h"
#include "constants/rogue.h"
#include "palette.h"
#include "party_menu.h"
#include "pokemon.h"
#include "pokemon_icon.h"
#include "pokemon_storage_system.h"
#include "sprite.h"
#include "test/test.h"

static u32 RunMenuFrame(MainCallback callback, u16 keys)
{
    u32 elapsed;

    VBlankIntrWait();
    REG_TM3CNT = (TIMER_ENABLE | TIMER_64CLK) << 16;
    gMain.heldKeys = keys;
    gMain.newKeys = keys;
    gMain.newAndRepeatedKeys = keys;
    callback();
    elapsed = REG_TM3CNT_L * 64;
    REG_TM3CNT_H = 0;
    return elapsed;
}

static u32 CountStorageIcons(void)
{
    u32 i, count = 0;

    for (i = 0; i < MAX_SPRITES; i++)
    {
        const struct Sprite *sprite = &gSprites[i];
        // The PC reserves the first 640 OBJ tiles for its 32x32 Pokémon icons.
        if (sprite->inUse && !sprite->invisible && sprite->oam.tileNum < 640
            && sprite->oam.shape == SPRITE_SHAPE(32x32) && sprite->oam.size == SPRITE_SIZE(32x32))
            count++;
    }
    return count;
}

static bool8 IsBoxPortraitDisplayed(u8 box, u8 slot)
{
    u16 species = GetBoxMonDataAt(box, slot, MON_DATA_SPECIES);
    u32 personality = GetBoxMonDataAt(box, slot, MON_DATA_PERSONALITY);
    u8 gender = GetGenderForSpecies(species, GetBoxMonDataAt(box, slot, MON_DATA_GENDER_FLAG));
    u32 i;

    LoadSpecialPokePic(gDecompressionBuffer, species, personality, gender, TRUE);
    for (i = 0; i < MAX_SPRITES; i++)
    {
        const struct Sprite *sprite = &gSprites[i];
        if (sprite->inUse && !sprite->invisible && sprite->x == 40 && sprite->y == 48
            && sprite->oam.shape == SPRITE_SHAPE(64x64) && sprite->oam.size == SPRITE_SIZE(64x64))
            return memcmp((const void *)(OBJ_VRAM0 + sprite->oam.tileNum * TILE_SIZE_4BPP), gDecompressionBuffer, MON_PIC_SIZE) == 0;
    }
    return FALSE;
}

TEST("Full PC box scrolling stays within the frame budget")
{
    MainCallback runner = gMain.callback2;
    MainCallback storage;
    u32 frame, box, slot, cycles, openFrame = 0, scrollFrame = 0, maxOpen = 0, maxScroll = 0, maxCursor = 0;
    u8 boxOption;
    u8 oldMode = gSaveBlock2Ptr->optionsButtonMode;
    u8 oldBox = StorageGetCurrentBox();
    static const u8 boxNames[2][BOX_NAME_LENGTH + 1] = {_("BOX 1"), _("BOX 2")};
    u8 oldWallpapers[2] = {gPokemonStoragePtr->boxWallpapers[0], gPokemonStoragePtr->boxWallpapers[1]};
    u8 oldBoxNames[sizeof(boxNames)];
    size_t backupSize = 2 * sizeof(gPokemonStoragePtr->boxes[0]);
    void *oldBoxes;

    PARAMETRIZE { boxOption = 0; } // Move Pokémon
    PARAMETRIZE { boxOption = 3; } // Move Items

    oldBoxes = Alloc(backupSize);
    EXPECT(oldBoxes != NULL);
    memcpy(oldBoxes, gPokemonStoragePtr->boxes, backupSize);
    gPokemonStoragePtr->currentBox = 0;
    gPokemonStoragePtr->boxWallpapers[0] = 0;
    gPokemonStoragePtr->boxWallpapers[1] = 1;
    memcpy(oldBoxNames, gPokemonStoragePtr->boxNames, sizeof(oldBoxNames));
    memcpy(gPokemonStoragePtr->boxNames, boxNames, sizeof(boxNames));
    for (box = 0; box < 2; box++)
    {
        for (slot = 0; slot < IN_BOX_COUNT; slot++)
        {
            struct Pokemon mon;
            CreateMon(&mon, SPECIES_BULBASAUR + box * IN_BOX_COUNT + slot, 30, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
            SetBoxMonAt(box, slot, &mon.box);
        }
    }
    gSaveBlock2Ptr->optionsButtonMode = OPTIONS_BUTTON_MODE_LR;
    EnterPokeStorage(boxOption);
    storage = gMain.callback2;
    for (frame = 0; frame < 65; frame++)
    {
        cycles = RunMenuFrame(storage, 0);
        if (cycles > maxOpen)
        {
            maxOpen = cycles;
            openFrame = frame;
        }
    }
    EXPECT_EQ(CountStorageIcons(), IN_BOX_COUNT);
    EXPECT_EQ(StorageGetCurrentBox(), 0);
    for (frame = 0; frame < 60; frame++)
    {
        cycles = RunMenuFrame(storage, frame == 0 ? R_BUTTON : 0);
        if (cycles > maxScroll)
        {
            maxScroll = cycles;
            scrollFrame = frame;
        }
    }
    EXPECT(IsBoxPortraitDisplayed(1, 0));
    for (frame = 0; frame < 30; frame++)
    {
        cycles = RunMenuFrame(storage, frame == 0 ? DPAD_RIGHT : 0);
        if (cycles > maxCursor)
            maxCursor = cycles;
    }
    MgbaPrintf_("Full PC box: opening frame %d took %d cycles, scroll frame %d took %d cycles, cursor refresh took at most %d cycles",
                openFrame, maxOpen, scrollFrame, maxScroll, maxCursor);
    EXPECT_EQ(StorageGetCurrentBox(), 1);
    EXPECT_EQ(CountStorageIcons(), IN_BOX_COUNT);
    EXPECT_LT(maxScroll, 280896);
    EXPECT_LT(maxCursor, 280896);
    EXPECT(IsBoxPortraitDisplayed(1, 1));

    for (frame = 0; frame < 70 && gMain.callback2 == storage; frame++)
        RunMenuFrame(storage, (frame == 0 || frame == 6) ? B_BUTTON : 0);
    EXPECT(gMain.callback2 != storage);
    SetVBlankCallback(NULL);
    SetMainCallback2(runner);
    gSaveBlock2Ptr->optionsButtonMode = oldMode;
    memcpy(gPokemonStoragePtr->boxes, oldBoxes, backupSize);
    Free(oldBoxes);
    gPokemonStoragePtr->currentBox = oldBox;
    gPokemonStoragePtr->boxWallpapers[0] = oldWallpapers[0];
    gPokemonStoragePtr->boxWallpapers[1] = oldWallpapers[1];
    memcpy(gPokemonStoragePtr->boxNames, oldBoxNames, sizeof(oldBoxNames));
}

static MainCallback sPartyVBlankCallback;
static u32 sPartyVBlankCycles;

static void MeasurePartyVBlank(void)
{
    u32 elapsed;
    REG_TM3CNT = (TIMER_ENABLE | TIMER_64CLK) << 16;
    sPartyVBlankCallback();
    elapsed = REG_TM3CNT_L * 64;
    REG_TM3CNT_H = 0;
    if (elapsed > sPartyVBlankCycles)
        sPartyVBlankCycles = elapsed;
}

static u8 FindMonIconPalette(struct Pokemon *mon)
{
    u8 i;
    u16 species = GetMonData(mon, MON_DATA_SPECIES);
    u32 personality = GetMonData(mon, MON_DATA_PERSONALITY);
    u8 gender = GetMonGender(mon);
    u16 tintedPalette[16];
    const u16 *expected = GetColoredMonIconPalette(species, personality, gender, IsMonShiny(mon),
                                                   GetMonData(mon, MON_DATA_OT_ID), tintedPalette);

    for (i = 0; i < MAX_SPRITES; i++)
    {
        const struct Sprite *sprite = &gSprites[i];
        if (sprite->inUse && !sprite->invisible
            && sprite->oam.shape == SPRITE_SHAPE(32x32) && sprite->oam.size == SPRITE_SIZE(32x32)
            && memcmp(&gPlttBufferUnfaded[OBJ_PLTT_ID(sprite->oam.paletteNum)], expected, PLTT_SIZE_4BPP) == 0)
            return sprite->oam.paletteNum;
    }
    return 0xFF;
}

TEST("Party icons keep six distinct shiny and Unique palettes")
{
    static const u16 species[PARTY_SIZE] =
    {
        SPECIES_CHARIZARD, SPECIES_GYARADOS, SPECIES_UMBREON,
        SPECIES_VENUSAUR, SPECIES_AMPHAROS, SPECIES_GENGAR
    };
    struct Pokemon oldParty[PARTY_SIZE];
    MainCallback runner = gMain.callback2;
    u8 oldPartyCount = gPlayerPartyCount;
    u8 frame, i;
    u16 usedPalettes = 0;
    u8 paletteBefore[PARTY_SIZE];

    memcpy(oldParty, gPlayerParty, sizeof(oldParty));
    for (i = 0; i < PARTY_SIZE; i++)
    {
        const u32 uniqueId = OTID_FLAG_CUSTOM_MON | OTID_FLAG_DYNAMIC_CUSTOM_MON | (1u << 28) | (TYPE_FIRE + i);
        bool8 shiny = i < 3 || i == PARTY_SIZE - 1;
        u16 tintedPalette[16];
        const u16 *normal, *colored;
        u8 gender;

        CreateMon(&gPlayerParty[i], species[i], 30, 0, TRUE, 0, i >= 3 ? OT_ID_CUSTOM_MON : OT_ID_PRESET, i >= 3 ? uniqueId : 0);
        SetMonData(&gPlayerParty[i], MON_DATA_IS_SHINY, &shiny);
        SetMonData(&gPlayerParty[i], MON_DATA_HELD_ITEM, &(u16){ITEM_LEFTOVERS});
        gender = GetMonGender(&gPlayerParty[i]);
        normal = GetColoredMonIconPalette(species[i], 0, gender, FALSE, 0, tintedPalette);
        colored = GetColoredMonIconPalette(species[i], 0, gender, shiny, GetMonData(&gPlayerParty[i], MON_DATA_OT_ID), tintedPalette);
        EXPECT(memcmp(normal, colored, PLTT_SIZE_4BPP) != 0);
    }
    gPlayerPartyCount = PARTY_SIZE;
    ChooseMonForTradingBoard(PARTY_MENU_TYPE_FIELD, runner);
    for (frame = 0; frame < 75; frame++)
        RunMenuFrame(gMain.callback2, 0);

    for (i = 0; i < PARTY_SIZE; i++)
    {
        u8 palette = FindMonIconPalette(&gPlayerParty[i]);
        EXPECT_LT(palette, 16);
        EXPECT_EQ(usedPalettes & (1u << palette), 0);
        usedPalettes |= 1u << palette;
        paletteBefore[i] = palette;
    }

    // Open the field action menu and use its second action, Switch.
    sPartyVBlankCallback = gMain.vblankCallback;
    EXPECT(sPartyVBlankCallback != NULL);
    sPartyVBlankCycles = 0;
    SetVBlankCallback(MeasurePartyVBlank);
    for (frame = 0; frame < 90; frame++)
    {
        u16 keys = 0;
        if (frame == 0 || frame == 12 || frame == 33)
            keys = A_BUTTON;
        if (frame == 8 || frame == 22)
            keys = DPAD_DOWN;
        VBlankIntrWait();
        gMain.heldKeys = keys;
        gMain.newKeys = keys;
        gMain.newAndRepeatedKeys = keys;
        gMain.callback2();
    }
    EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_SPECIES), species[1]);
    EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_SPECIES), species[0]);
    EXPECT_EQ(FindMonIconPalette(&gPlayerParty[0]), paletteBefore[1]);
    EXPECT_EQ(FindMonIconPalette(&gPlayerParty[1]), paletteBefore[0]);
    SetVBlankCallback(sPartyVBlankCallback);
    MgbaPrintf_("Six party icons: worst menu VBlank callback took %d cycles", sPartyVBlankCycles);
    EXPECT_LT(sPartyVBlankCycles, 68 * 1232);

    for (frame = 0; frame < 65 && gMain.callback2 != runner; frame++)
        RunMenuFrame(gMain.callback2, frame == 0 ? B_BUTTON : 0);
    EXPECT(gMain.callback2 == runner);
    SetVBlankCallback(NULL);
    memcpy(gPlayerParty, oldParty, sizeof(oldParty));
    gPlayerPartyCount = oldPartyCount;
}
