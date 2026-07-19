#include "global.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "event_data.h"
#include "item.h"
#include "rogue.h"
#include "rogue_save.h"
#include "test/test.h"

static void ResetDynamicItemTestState(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    ClearBag();
    RogueSave_SaveHubStates();
    gRogueSaveBlock->dynamicItemLayoutVersion = ROGUE_DYNAMIC_ITEM_LAYOUT_VERSION;
}

static u16 GetHubBagItemQuantity(u16 itemId)
{
    u16 i;
    u16 quantity = 0;

    for(i = 0; i < BAG_ITEM_CAPACITY; ++i)
    {
        if(RogueSave_GetHubBagItemIdAt(i) == itemId)
            quantity += RogueSave_GetHubBagItemQuantityAt(i);
    }

    return quantity;
}

TEST("Dynamic Rogue item groups are packed into the versioned range")
{
    EXPECT_EQ(FIRST_ITEM_CHARM, FIRST_ITEM_DYNAMIC);
    EXPECT_EQ(FIRST_ITEM_CURSE, LAST_ITEM_CHARM + 1);
    EXPECT_EQ(FIRST_ITEM_TR, LAST_ITEM_CURSE + 1);
    EXPECT_LE(LAST_ITEM_TR, LAST_ITEM_DYNAMIC);
}

TEST("Changing the dynamic item layout clears dynamic items from saved bags")
{
    ResetDynamicItemTestState();

    EXPECT(AddBagItem(ITEM_POTION, 2));
    EXPECT(AddBagItem(ITEM_CONVERSION_CHARM, 1));
    RogueSave_SaveHubStates();

    gRogueSaveBlock->dynamicItemLayoutVersion = 0;
    RogueSave_OnSaveLoaded();

    EXPECT_EQ(GetItemCountInBag(ITEM_POTION), 2);
    EXPECT_EQ(GetItemCountInBag(ITEM_CONVERSION_CHARM), 0);
    EXPECT_EQ(GetHubBagItemQuantity(ITEM_POTION), 2);
    EXPECT_EQ(GetHubBagItemQuantity(ITEM_CONVERSION_CHARM), 0);
    EXPECT_EQ(gRogueSaveBlock->dynamicItemLayoutVersion, ROGUE_DYNAMIC_ITEM_LAYOUT_VERSION);

    ResetDynamicItemTestState();
}

TEST("Loading the current dynamic item layout preserves dynamic items")
{
    ResetDynamicItemTestState();

    EXPECT(AddBagItem(ITEM_CONVERSION_CHARM, 1));
    RogueSave_SaveHubStates();
    RogueSave_OnSaveLoaded();

    EXPECT_EQ(GetItemCountInBag(ITEM_CONVERSION_CHARM), 1);
    EXPECT_EQ(GetHubBagItemQuantity(ITEM_CONVERSION_CHARM), 1);

    ResetDynamicItemTestState();
}
