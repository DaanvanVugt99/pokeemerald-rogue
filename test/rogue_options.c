#include "global.h"
#include "main.h"
#include "rogue_controller.h"
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
