#include "global.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "event_data.h"
#include "item.h"
#include "item_menu.h"
#include "party_menu.h"
#include "pokemon.h"
#include "test/test.h"

TEST("Quick Heal offers enough medicine to finish, or the best available partial heal")
{
    u16 doses, hpAfter;
    ClearBag();
    EXPECT(AddBagItem(ITEM_POTION, 1));
    EXPECT_EQ(RogueTest_QuickHealOffer(15, 70, 0, &doses, &hpAfter), ITEM_POTION);
    EXPECT_EQ(doses, 1);
    EXPECT_EQ(hpAfter, 35);
    EXPECT(AddBagItem(ITEM_HYPER_POTION, 1));
    EXPECT_EQ(RogueTest_QuickHealOffer(15, 70, 0, &doses, &hpAfter), ITEM_HYPER_POTION);
    EXPECT_EQ(hpAfter, 70);
    EXPECT_EQ(RogueTest_QuickHealOffer(70, 70, 0, &doses, &hpAfter), ITEM_NONE);
}

TEST("Quick Heal reserves Full Restore for combined healing and Max Revive ahead of bitter herbs")
{
    u16 doses, hpAfter;
    ClearBag();
    EXPECT(AddBagItem(ITEM_FULL_RESTORE, 1));
    EXPECT(AddBagItem(ITEM_POTION, 2));
    EXPECT_EQ(RogueTest_QuickHealOffer(35, 70, 0, &doses, &hpAfter), ITEM_POTION);
    EXPECT_EQ(doses, 2);
    EXPECT_EQ(hpAfter, 70);
    EXPECT_EQ(RogueTest_QuickHealOffer(35, 70, STATUS1_BURN, &doses, &hpAfter), ITEM_FULL_RESTORE);
    EXPECT_EQ(doses, 1);
    EXPECT(AddBagItem(ITEM_REVIVAL_HERB, 1));
    EXPECT(AddBagItem(ITEM_MAX_REVIVE, 1));
    EXPECT_EQ(RogueTest_QuickHealOffer(0, 71, 0, &doses, &hpAfter), ITEM_MAX_REVIVE);
    EXPECT(AddBagItem(ITEM_REVIVE, 1));
    EXPECT_EQ(RogueTest_QuickHealOffer(0, 71, 0, &doses, &hpAfter), ITEM_REVIVE);
    EXPECT_EQ(doses, 1);
    EXPECT_EQ(hpAfter, 35);
}

TEST("Quick Heal deducts only effective doses and respects the remaining Bag stock")
{
    struct Pokemon *mon = &gPlayerParty[0];
    u16 maxHp, hp;
    ClearBag();
    EXPECT(AddBagItem(ITEM_POTION, 3));
    CreateMon(mon, SPECIES_BULBASAUR, 50, 31, FALSE, 0, OT_ID_PLAYER_ID, 0);
    gPartyMenu.slotId = 0;
    gPartyMenu.action = 0;
    gSpecialVar_ItemId = ITEM_POTION;
    maxHp = GetMonData(mon, MON_DATA_MAX_HP);
    hp = maxHp - 1;
    SetMonData(mon, MON_DATA_HP, &hp);
    EXPECT_EQ(RogueTest_ApplyQuickHeal(3), 1);
    EXPECT_EQ(GetItemCountInBag(ITEM_POTION), 2);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), maxHp);
    EXPECT_EQ(RogueTest_ApplyQuickHeal(2), 0);
    EXPECT_EQ(GetItemCountInBag(ITEM_POTION), 2);
    hp = 1;
    SetMonData(mon, MON_DATA_HP, &hp);
    EXPECT_EQ(RogueTest_ApplyQuickHeal(10), 2);
    EXPECT_EQ(GetMonData(mon, MON_DATA_HP), 41);
    EXPECT_EQ(GetItemCountInBag(ITEM_POTION), 0);
}
