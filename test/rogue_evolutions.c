#include "global.h"
#include "constants/items.h"
#include "constants/pokemon.h"
#include "constants/species.h"
#include "pokemon.h"
#include "rogue_baked.h"
#include "test/test.h"

TEST("Both Gimmighoul forms evolve with a Gimmighoul Coin")
{
#ifdef ROGUE_EXPANSION
    static const u16 sGimmighoulForms[] =
    {
        SPECIES_GIMMIGHOUL_CHEST,
        SPECIES_GIMMIGHOUL_ROAMING,
    };
    struct Evolution evolution;
    struct Pokemon mon;
    u8 i;

    for(i = 0; i < ARRAY_COUNT(sGimmighoulForms); ++i)
    {
        Rogue_ModifyEvolution(sGimmighoulForms[i], 0, &evolution);
        EXPECT_EQ(evolution.method, EVO_ITEM);
        EXPECT_EQ(evolution.param, ITEM_GIMMIGHOUL_COIN);
        EXPECT_EQ(evolution.targetSpecies, SPECIES_GHOLDENGO);

        CreateMon(&mon, sGimmighoulForms[i], 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
        EXPECT_EQ(GetEvolutionTargetSpecies(&mon, EVO_MODE_ITEM_CHECK, ITEM_GIMMIGHOUL_COIN, NULL), SPECIES_GHOLDENGO);
    }
#else
    ASSUME(FALSE);
#endif
}

TEST("Gholdengo reverts to Gimmighoul Chest Form as its egg species")
{
#ifdef ROGUE_EXPANSION
    EXPECT_EQ(Rogue_GetEggSpecies(SPECIES_GHOLDENGO), SPECIES_GIMMIGHOUL_CHEST);
#else
    ASSUME(FALSE);
#endif
}
