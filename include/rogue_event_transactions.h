#ifndef GUARD_ROGUE_EVENT_TRANSACTIONS_H
#define GUARD_ROGUE_EVENT_TRANSACTIONS_H

#include "global.h"

#define ROGUE_EVENT_TRANSACTION_ITEM_CAPACITY 2
#define ROGUE_EVENT_TRANSACTION_FLAG_ALLOW_COST_SLOTS_FOR_REWARDS (1 << 0)

struct RogueEventTransactionItem
{
    u16 itemId;
    u16 count;
};

// A stack-local description of an atomic Bag and money exchange. Transactions
// intentionally own no persistent state; callers keep quest and scene changes
// outside the exchange and may roll a successful exchange back if that later
// work fails.
struct RogueEventTransaction
{
    struct RogueEventTransactionItem costs[ROGUE_EVENT_TRANSACTION_ITEM_CAPACITY];
    struct RogueEventTransactionItem rewards[ROGUE_EVENT_TRANSACTION_ITEM_CAPACITY];
    u32 moneyReward;
    u8 costCount;
    u8 rewardCount;
    u8 flags;
};

u8 RogueEventTransaction_Execute(const struct RogueEventTransaction *transaction);
void RogueEventTransaction_Rollback(const struct RogueEventTransaction *transaction);

#endif // GUARD_ROGUE_EVENT_TRANSACTIONS_H
