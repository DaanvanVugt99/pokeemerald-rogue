#include "global.h"

#include "constants/items.h"
#include "constants/rogue_route_events.h"

#include "item.h"
#include "money.h"

#include "rogue_event_transactions.h"

static bool8 IsItemEntryValid(const struct RogueEventTransactionItem *entry)
{
    return entry->itemId > ITEM_NONE && entry->itemId < ITEMS_COUNT && entry->count != 0;
}

static void RestoreCosts(const struct RogueEventTransaction *transaction, u8 removedCount)
{
    while(removedCount != 0)
    {
        bool8 restored;

        --removedCount;
        restored = AddBagItem(
            transaction->costs[removedCount].itemId,
            transaction->costs[removedCount].count);
        AGB_ASSERT(restored);
        (void)restored;
    }
}

static void RemoveRewards(const struct RogueEventTransaction *transaction, u8 addedCount)
{
    while(addedCount != 0)
    {
        bool8 removed;

        --addedCount;
        removed = RemoveBagItem(
            transaction->rewards[addedCount].itemId,
            transaction->rewards[addedCount].count);
        AGB_ASSERT(removed);
        (void)removed;
    }
}

u8 RogueEventTransaction_Execute(const struct RogueEventTransaction *transaction)
{
    u32 money;
    u8 i;

    if(transaction == NULL
        || transaction->costCount > ROGUE_EVENT_TRANSACTION_ITEM_CAPACITY
        || transaction->rewardCount > ROGUE_EVENT_TRANSACTION_ITEM_CAPACITY
        || (transaction->flags & ~ROGUE_EVENT_TRANSACTION_FLAG_ALLOW_COST_SLOTS_FOR_REWARDS) != 0
        || (transaction->costCount == 0 && transaction->rewardCount == 0 && transaction->moneyReward == 0))
        return ROGUE_ROUTE_EVENT_RESULT_FAILED;

    for(i = 0; i < transaction->costCount; ++i)
    {
        if(!IsItemEntryValid(&transaction->costs[i]))
            return ROGUE_ROUTE_EVENT_RESULT_FAILED;
        if(!CheckBagHasItem(transaction->costs[i].itemId, transaction->costs[i].count))
            return ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
    }

    for(i = 0; i < transaction->rewardCount; ++i)
    {
        if(!IsItemEntryValid(&transaction->rewards[i]))
            return ROGUE_ROUTE_EVENT_RESULT_FAILED;
        if((transaction->flags & ROGUE_EVENT_TRANSACTION_FLAG_ALLOW_COST_SLOTS_FOR_REWARDS) == 0
            && !CheckBagHasSpace(transaction->rewards[i].itemId, transaction->rewards[i].count))
            return ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
    }

    money = GetMoney(&gSaveBlock1Ptr->money);
    if(transaction->moneyReward > MAX_MONEY || money > MAX_MONEY - transaction->moneyReward)
        return ROGUE_ROUTE_EVENT_RESULT_MONEY_FULL;

    // Remove costs before adding rewards so an exchange may reuse the Bag slot
    // it just freed. Any later failure restores the exact prior inventory.
    for(i = 0; i < transaction->costCount; ++i)
    {
        if(!RemoveBagItem(transaction->costs[i].itemId, transaction->costs[i].count))
        {
            RestoreCosts(transaction, i);
            return ROGUE_ROUTE_EVENT_RESULT_MISSING_ITEM;
        }
    }

    for(i = 0; i < transaction->rewardCount; ++i)
    {
        u16 countBefore = GetItemCountInBag(transaction->rewards[i].itemId);

        if(!AddBagItem(transaction->rewards[i].itemId, transaction->rewards[i].count))
        {
            u16 countAfter = GetItemCountInBag(transaction->rewards[i].itemId);

            // AddBagItem may fill an existing stack before discovering that
            // the remainder cannot fit. Remove that partial grant too.
            if(countAfter > countBefore)
            {
                bool8 removed = RemoveBagItem(
                    transaction->rewards[i].itemId,
                    countAfter - countBefore);

                AGB_ASSERT(removed);
                (void)removed;
            }
            RemoveRewards(transaction, i);
            RestoreCosts(transaction, transaction->costCount);
            return ROGUE_ROUTE_EVENT_RESULT_NO_SPACE;
        }
    }

    AddMoney(&gSaveBlock1Ptr->money, transaction->moneyReward);
    return ROGUE_ROUTE_EVENT_RESULT_SUCCESS;
}

void RogueEventTransaction_Rollback(const struct RogueEventTransaction *transaction)
{
    if(transaction == NULL)
        return;

    RemoveMoney(&gSaveBlock1Ptr->money, transaction->moneyReward);
    RemoveRewards(transaction, transaction->rewardCount);
    RestoreCosts(transaction, transaction->costCount);
}
