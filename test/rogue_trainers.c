#include "global.h"
#include "test/test.h"
#include "rogue_trainers.h"

TEST("Rogue trainer items: Black Sludge converts to Leftovers with tera")
{
#if defined(ROGUE_EXPANSION)
    EXPECT_EQ(RogueDebug_AdjustHeldItemForTera(ITEM_BLACK_SLUDGE, TRUE), ITEM_LEFTOVERS);
    EXPECT_EQ(RogueDebug_AdjustHeldItemForTera(ITEM_BLACK_SLUDGE, FALSE), ITEM_BLACK_SLUDGE);
    EXPECT_EQ(RogueDebug_AdjustHeldItemForTera(ITEM_LEFTOVERS, TRUE), ITEM_LEFTOVERS);
#else
    ASSUME(FALSE);
#endif
}
