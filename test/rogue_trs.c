#include "global.h"
#include "characters.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "event_data.h"
#include "item.h"
#include "party_menu.h"
#include "pokemon.h"
#include "rogue.h"
#include "rogue_baked.h"
#include "rogue_controller.h"
#include "test/test.h"

TEST("Custom moves are valid dynamic TR items")
{
    u16 move = MOVE_NONE;
    u16 species = SPECIES_NONE;
    u8 itemName[ITEM_NAME_LENGTH + 1];

    PARAMETRIZE { move = MOVE_SHIMMER; species = SPECIES_VENOMOTH; }
    PARAMETRIZE { move = MOVE_COLD_SNAP; species = SPECIES_BEARTIC; }
    PARAMETRIZE { move = MOVE_NIGHTFALL; species = SPECIES_HOUNDOOM; }
    PARAMETRIZE { move = MOVE_JETSTREAM; species = SPECIES_CROBAT; }
    PARAMETRIZE { move = MOVE_THUNDERCRUSH; species = SPECIES_LUXRAY; }

    memset(&gRogueRun, 0, sizeof(gRogueRun));
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    gRogueRun.dynamicTRMoves[0] = move;

    EXPECT_EQ(Rogue_GetTRMove(0), move);
    EXPECT_EQ(ItemIdToBattleMoveId(ITEM_TR01), move);
    EXPECT_EQ(BattleMoveIdToItemId(move), ITEM_TR01);

    CopyItemName(ITEM_TR01, itemName);
    EXPECT_NE(itemName[0], EOS);
    EXPECT(ItemId_GetDescription(ITEM_TR01) != NULL);
    EXPECT(Rogue_GetPrice(ITEM_TR01) > 0);
    EXPECT(Rogue_GetItemIconPicOrPalette(ITEM_TR01, 0) != NULL);
    EXPECT(Rogue_GetItemIconPicOrPalette(ITEM_TR01, 1) != NULL);
    EXPECT(CanSpeciesLearnTM(species, ITEM_TR01));

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
}
