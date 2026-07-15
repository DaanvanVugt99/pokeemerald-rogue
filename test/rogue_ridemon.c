#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "rogue_ridemon.h"
#include "test/test.h"

static void SetRideStealthTestState(u16 rideSpecies, bool8 isRiding, bool8 isUnlocked)
{
    struct RogueRideMonState *rideState;

    Rogue_RideMonInit();
    rideState = Rogue_GetPlayerRideMonStatePtr();
    rideState->monGfx = rideSpecies;

    if(isRiding)
        gPlayerAvatar.flags |= PLAYER_AVATAR_FLAG_RIDING;
    else
        gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_RIDING;

    if(isUnlocked)
        FlagSet(FLAG_SYS_RIDING_STEALTH);
    else
        FlagClear(FLAG_SYS_RIDING_STEALTH);
}

TEST("Ride Stealth requires its upgrade and an active compatible mount")
{
    SetRideStealthTestState(SPECIES_ZOROARK, TRUE, FALSE);
    EXPECT(!Rogue_IsRideMonStealthActive());

    SetRideStealthTestState(SPECIES_ZOROARK, FALSE, TRUE);
    EXPECT(!Rogue_IsRideMonStealthActive());

    SetRideStealthTestState(SPECIES_STANTLER, TRUE, TRUE);
    EXPECT(!Rogue_IsRideMonStealthActive());

    SetRideStealthTestState(SPECIES_ZOROARK, TRUE, TRUE);
    EXPECT(Rogue_IsRideMonStealthActive());

    FlagClear(FLAG_SYS_RIDING_STEALTH);
    gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_RIDING;
    Rogue_RideMonInit();
}

TEST("Ride Stealth handles shiny graphics and inherited ride forms")
{
    SetRideStealthTestState(SPECIES_ZOROARK + FOLLOWMON_SHINY_OFFSET, TRUE, TRUE);
    EXPECT(Rogue_IsRideMonStealthActive());

    EXPECT(Rogue_IsValidRideStealthSpecies(SPECIES_DECIDUEYE_HISUIAN));

    FlagClear(FLAG_SYS_RIDING_STEALTH);
    gPlayerAvatar.flags &= ~PLAYER_AVATAR_FLAG_RIDING;
    Rogue_RideMonInit();
}
