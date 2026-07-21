#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "main.h"
#include "pokemon.h"
#include "rogue_controller.h"
#include "rogue_script.h"
#include "string_util.h"
#include "test/test.h"

TEST("Non-battle animation speed settings map to one through four times speed")
{
    u16 heldKeys = gMain.heldKeys;
    u8 cutsceneSpeed = gSaveBlock2Ptr->optionsCutsceneSpeed;
    u8 evolutionSpeed = gSaveBlock2Ptr->optionsEvolutionSpeed;
    u8 catchingSpeed = gSaveBlock2Ptr->optionsCatchingSpeed;
    u8 i;

    gMain.heldKeys = 0;
    for (i = 0; i < OPTIONS_ANIM_SPEED_COUNT; i++)
    {
        gSaveBlock2Ptr->optionsCutsceneSpeed = i;
        gSaveBlock2Ptr->optionsEvolutionSpeed = i;
        gSaveBlock2Ptr->optionsCatchingSpeed = i;

        EXPECT_EQ(Rogue_GetCutsceneSpeedScale(), i + 1);
        EXPECT_EQ(Rogue_GetEvolutionSpeedScale(), i + 1);
        EXPECT_EQ(Rogue_GetCatchingSpeedScale(), i + 1);
    }

    gMain.heldKeys = L_BUTTON;
    EXPECT_EQ(Rogue_GetCutsceneSpeedScale(), 1);
    EXPECT_EQ(Rogue_GetEvolutionSpeedScale(), 1);
    EXPECT_EQ(Rogue_GetCatchingSpeedScale(), 1);

    gMain.heldKeys = heldKeys;
    gSaveBlock2Ptr->optionsCutsceneSpeed = cutsceneSpeed;
    gSaveBlock2Ptr->optionsEvolutionSpeed = evolutionSpeed;
    gSaveBlock2Ptr->optionsCatchingSpeed = catchingSpeed;
}

TEST("Gift Pokemon nickname handling respects every nickname mode")
{
    struct Pokemon originalParty[PARTY_SIZE];
    u8 originalNicknameMode = gSaveBlock2Ptr->optionsNicknameMode;
    u16 originalPartyIndex = gSpecialVar_0x8004;
    u8 originalPartyCount = gPlayerPartyCount;
    bool8 wasWildSafari = FlagGet(FLAG_ROGUE_WILD_SAFARI);
    u8 nickname[POKEMON_NAME_LENGTH + 1];
    u8 mode;

    memcpy(originalParty, gPlayerParty, sizeof(originalParty));
    FlagClear(FLAG_ROGUE_WILD_SAFARI);
    gPlayerPartyCount = 1;
    gSpecialVar_0x8004 = 0;

    for (mode = 0; mode < OPTIONS_NICKNAME_COUNT; ++mode)
    {
        CreateMon(&gPlayerParty[0], SPECIES_PIKACHU, 5, 0, FALSE, 0, OT_ID_PLAYER_ID, 0);
        gSaveBlock2Ptr->optionsNicknameMode = mode;

        switch (mode)
        {
        case OPTIONS_NICKNAME_MODE_ASK:
            EXPECT_EQ(Rogue_ApplyPartyMonNicknameSettings(), NICKNAME_ACTION_ASK);
            break;
        case OPTIONS_NICKNAME_MODE_ALWAYS:
            EXPECT_EQ(Rogue_ApplyPartyMonNicknameSettings(), NICKNAME_ACTION_FORCE);
            break;
        case OPTIONS_NICKNAME_MODE_NEVER:
            EXPECT_EQ(Rogue_ApplyPartyMonNicknameSettings(), NICKNAME_ACTION_SKIP);
            break;
        case OPTIONS_NICKNAME_RANDOM:
            EXPECT_EQ(Rogue_ApplyPartyMonNicknameSettings(), NICKNAME_ACTION_SKIP);
            GetMonData(&gPlayerParty[0], MON_DATA_NICKNAME, nickname);
            EXPECT_NE(StringCompare(nickname, GetSpeciesName(SPECIES_PIKACHU)), 0);
            break;
        }
    }

    memcpy(gPlayerParty, originalParty, sizeof(originalParty));
    gPlayerPartyCount = originalPartyCount;
    gSpecialVar_0x8004 = originalPartyIndex;
    gSaveBlock2Ptr->optionsNicknameMode = originalNicknameMode;
    if (wasWildSafari)
        FlagSet(FLAG_ROGUE_WILD_SAFARI);
}
