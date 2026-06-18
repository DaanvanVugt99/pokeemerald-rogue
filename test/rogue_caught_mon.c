#include "global.h"
#include "constants/flags.h"
#include "constants/rogue.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "rogue.h"
#include "rogue_controller.h"
#include "rogue_settings.h"
#include "test/test.h"

static void ResetCaughtMonTestState(void)
{
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, TRUE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_HELD_ITEM_CLAUSE, FALSE);
}

static void ClearCaughtMonTestState(void)
{
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, FALSE);
    Rogue_SetConfigToggle(CONFIG_TOGGLE_HELD_ITEM_CLAUSE, FALSE);
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
}

static void SetPartyMon(u8 slot, u16 species)
{
    CreateMon(&gPlayerParty[slot], species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();
}

static struct Pokemon CreateCaughtMon(u16 species)
{
    struct Pokemon mon;

    CreateMon(&mon, species, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    return mon;
}

TEST("Species Clause duplicate catches with party room require releasing the matching evo chain")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    SetPartyMon(0, SPECIES_METAPOD);
    SetPartyMon(1, SPECIES_POOCHYENA);
    caughtMon = CreateCaughtMon(SPECIES_BUTTERFREE);

    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));
    EXPECT(Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 0));
    EXPECT(!Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 1));

    ClearCaughtMonTestState();
}

TEST("Species Clause off allows duplicate catches when the party has room")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    Rogue_SetConfigToggle(CONFIG_TOGGLE_SPECIES_CLAUSE, FALSE);
    SetPartyMon(0, SPECIES_METAPOD);
    caughtMon = CreateCaughtMon(SPECIES_BUTTERFREE);

    EXPECT(Rogue_CanAddCaughtMonToParty(&caughtMon));

    ClearCaughtMonTestState();
}

TEST("Full party catches without a Species Clause conflict can release any party member")
{
    struct Pokemon caughtMon;

    ResetCaughtMonTestState();
    SetPartyMon(0, SPECIES_TREECKO);
    SetPartyMon(1, SPECIES_TORCHIC);
    SetPartyMon(2, SPECIES_MUDKIP);
    SetPartyMon(3, SPECIES_POOCHYENA);
    SetPartyMon(4, SPECIES_TAILLOW);
    SetPartyMon(5, SPECIES_RALTS);
    caughtMon = CreateCaughtMon(SPECIES_SHROOMISH);

    EXPECT(!Rogue_CanAddCaughtMonToParty(&caughtMon));
    EXPECT(Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 0));
    EXPECT(Rogue_CanReleasePartyMonForCaughtMon(&caughtMon, 3));

    ClearCaughtMonTestState();
}
