#include "global.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "event_data.h"
#include "item.h"
#include "rogue_baked.h"
#include "rogue_controller.h"
#include "test/test.h"

#ifdef ROGUE_EXPANSION

TEST("Tera Shards use the reusable single-copy item rules")
{
    ClearBag();

    EXPECT(Rogue_IsReusableItem(ITEM_FIRE_TERA_SHARD));
    EXPECT(Rogue_IsReusableItem(ITEM_STELLAR_TERA_SHARD));
    EXPECT_EQ(Rogue_GetPrice(ITEM_FIRE_TERA_SHARD), 8000);
    EXPECT(AddBagItem(ITEM_FIRE_TERA_SHARD, 3));
    EXPECT_EQ(GetItemCountInBag(ITEM_FIRE_TERA_SHARD), 1);
    EXPECT(!CheckBagHasSpace(ITEM_FIRE_TERA_SHARD, 1));
    EXPECT(!AddBagItem(ITEM_FIRE_TERA_SHARD, 1));

    ClearBag();
}

TEST("TMs and TRs retain the reusable single-copy item rules")
{
    ClearBag();

    EXPECT(Rogue_IsReusableItem(ITEM_TM_PROTECT));
    EXPECT(Rogue_IsReusableItem(ITEM_TR01));
    EXPECT(AddBagItem(ITEM_TM_PROTECT, 1));
    EXPECT(!AddBagItem(ITEM_TM_PROTECT, 1));

    ClearBag();
}

#endif
