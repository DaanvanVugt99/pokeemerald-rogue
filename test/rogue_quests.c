#include "global.h"
#include "constants/flags.h"
#include "constants/items.h"
#include "constants/rogue.h"
#include "constants/rogue_quests.h"
#include "constants/species.h"
#include "event_data.h"
#include "pokemon.h"
#include "rogue.h"
#include "rogue_adventurepaths.h"
#include "rogue_controller.h"
#include "rogue_gifts.h"
#include "rogue_quest.h"
#include "test/test.h"

static void ResetQuestTestState(void)
{
    memset(&gRogueRun, 0, sizeof(gRogueRun));
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
    RogueQuest_OnNewGame();
    RogueQuest_ActivateQuestsFor(QUEST_CONST_ACTIVE_IN_RUN);
}

static void ClearQuestTestState(void)
{
    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    memset(gPlayerParty, 0, sizeof(gPlayerParty));
    gPlayerPartyCount = 0;
}

static bool8 QuestHasMoneyReward(u16 questId, u32 amount)
{
    u16 i;

    for(i = 0; i < RogueQuest_GetRewardCount(questId); ++i)
    {
        struct RogueQuestReward const* reward = RogueQuest_GetReward(questId, i);

        if(reward->type == QUEST_REWARD_MONEY && reward->perType.money.amount == amount)
            return TRUE;
    }

    return FALSE;
}

static bool8 QuestHasItemReward(u16 questId, u16 item, u16 count)
{
    u16 i;

    for(i = 0; i < RogueQuest_GetRewardCount(questId); ++i)
    {
        struct RogueQuestReward const* reward = RogueQuest_GetReward(questId, i);

        if(reward->type == QUEST_REWARD_ITEM
         && reward->perType.item.item == item
         && reward->perType.item.count == count)
            return TRUE;
    }

    return FALSE;
}

TEST("Full House completes only when an Adventure starts with six Pokemon")
{
    u8 i;

    ResetQuestTestState();

    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_FULL_HOUSE));
    EXPECT(RogueQuest_IsQuestActive(QUEST_ID_FULL_HOUSE));

    for(i = 0; i < PARTY_SIZE - 1; ++i)
        CreateMon(&gPlayerParty[i], SPECIES_EEVEE, 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();

    RogueQuest_OnTrigger(QUEST_TRIGGER_RUN_START);
    EXPECT(!RogueQuest_GetStateFlag(QUEST_ID_FULL_HOUSE, QUEST_STATE_HAS_COMPLETE));

    CreateMon(&gPlayerParty[PARTY_SIZE - 1], SPECIES_EEVEE, 5, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    CalculatePlayerPartyCount();

    RogueQuest_OnTrigger(QUEST_TRIGGER_RUN_START);
    EXPECT(RogueQuest_GetStateFlag(QUEST_ID_FULL_HOUSE, QUEST_STATE_HAS_COMPLETE));
    EXPECT(RogueQuest_HasPendingRewards(QUEST_ID_FULL_HOUSE));

    ClearQuestTestState();
}

TEST("A Unique Legend requires the caught Pokemon to be both Unique and Legendary")
{
    struct Pokemon mon;
    u32 customMonId;

    ResetQuestTestState();

    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_A_UNIQUE_LEGEND));
    EXPECT(RogueQuest_IsQuestActive(QUEST_ID_A_UNIQUE_LEGEND));

    CreateMon(&mon, SPECIES_MEWTWO, 50, USE_RANDOM_IVS, FALSE, 0, OT_ID_PLAYER_ID, 0);
    Rogue_OnAcceptCaughtMon(&mon);
    EXPECT(!RogueQuest_GetStateFlag(QUEST_ID_A_UNIQUE_LEGEND, QUEST_STATE_HAS_COMPLETE));

    customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_COMMON, SPECIES_BULBASAUR);
    RogueGift_CreateMon(customMonId, &mon, SPECIES_BULBASAUR, 50, USE_RANDOM_IVS);
    Rogue_OnAcceptCaughtMon(&mon);
    EXPECT(!RogueQuest_GetStateFlag(QUEST_ID_A_UNIQUE_LEGEND, QUEST_STATE_HAS_COMPLETE));

    customMonId = RogueGift_CreateDynamicMonIdRaw(UNIQUE_RARITY_LEGENDARY, SPECIES_MEWTWO);
    RogueGift_CreateMon(customMonId, &mon, SPECIES_MEWTWO, 50, USE_RANDOM_IVS);
    Rogue_OnAcceptCaughtMon(&mon);
    EXPECT(RogueQuest_GetStateFlag(QUEST_ID_A_UNIQUE_LEGEND, QUEST_STATE_HAS_COMPLETE));
    EXPECT(RogueQuest_HasPendingRewards(QUEST_ID_A_UNIQUE_LEGEND));

    ClearQuestTestState();
}

TEST("New Main Quest rewards follow their progression tiers and Pokeblock distribution")
{
    EXPECT(RogueQuest_GetConstFlag(QUEST_ID_FRONTIER_BRAIN, QUEST_CONST_IS_MAIN_QUEST));
    EXPECT_EQ(RogueQuest_GetRewardCount(QUEST_ID_FRONTIER_BRAIN), 3);
    EXPECT(QuestHasMoneyReward(QUEST_ID_FRONTIER_BRAIN, QUEST_REWARD_MEDIUM_MONEY));
    EXPECT(QuestHasItemReward(QUEST_ID_FRONTIER_BRAIN, ITEM_BUILDING_SUPPLIES, QUEST_REWARD_SMALL_BUILD_AMOUNT));
    EXPECT(QuestHasItemReward(QUEST_ID_FRONTIER_BRAIN, ITEM_POKEBLOCK_FIGHTING, 5));

    EXPECT(RogueQuest_GetConstFlag(QUEST_ID_FULL_HOUSE, QUEST_CONST_IS_MAIN_QUEST));
    EXPECT_EQ(RogueQuest_GetRewardCount(QUEST_ID_FULL_HOUSE), 3);
    EXPECT(QuestHasMoneyReward(QUEST_ID_FULL_HOUSE, QUEST_REWARD_MEDIUM_MONEY));
    EXPECT(QuestHasItemReward(QUEST_ID_FULL_HOUSE, ITEM_BUILDING_SUPPLIES, QUEST_REWARD_SMALL_BUILD_AMOUNT));
    EXPECT(QuestHasItemReward(QUEST_ID_FULL_HOUSE, ITEM_POKEBLOCK_WATER, 5));

    EXPECT(RogueQuest_GetConstFlag(QUEST_ID_A_UNIQUE_LEGEND, QUEST_CONST_IS_MAIN_QUEST));
    EXPECT_EQ(RogueQuest_GetRewardCount(QUEST_ID_A_UNIQUE_LEGEND), 3);
    EXPECT(QuestHasMoneyReward(QUEST_ID_A_UNIQUE_LEGEND, QUEST_REWARD_HUGE_MONEY));
    EXPECT(QuestHasItemReward(QUEST_ID_A_UNIQUE_LEGEND, ITEM_BUILDING_SUPPLIES, QUEST_REWARD_MEDIUM_BUILD_AMOUNT));
    EXPECT(QuestHasItemReward(QUEST_ID_A_UNIQUE_LEGEND, ITEM_POKEBLOCK_DRAGON, 5));

    EXPECT(RogueQuest_GetConstFlag(QUEST_ID_TREASURE_HUNTER, QUEST_CONST_IS_MAIN_QUEST));
    EXPECT_EQ(RogueQuest_GetRewardCount(QUEST_ID_TREASURE_HUNTER), 3);
    EXPECT(QuestHasMoneyReward(QUEST_ID_TREASURE_HUNTER, QUEST_REWARD_HUGE_MONEY));
    EXPECT(QuestHasItemReward(QUEST_ID_TREASURE_HUNTER, ITEM_BUILDING_SUPPLIES, QUEST_REWARD_SMALL_BUILD_AMOUNT));
    EXPECT(QuestHasItemReward(QUEST_ID_TREASURE_HUNTER, ITEM_POKEBLOCK_DARK, 5));

    EXPECT(RogueQuest_GetConstFlag(QUEST_ID_NEW_FRONTIER, QUEST_CONST_IS_MAIN_QUEST));
    EXPECT_EQ(RogueQuest_GetRewardCount(QUEST_ID_NEW_FRONTIER), 3);
    EXPECT(QuestHasMoneyReward(QUEST_ID_NEW_FRONTIER, QUEST_REWARD_SMALL_MONEY));
    EXPECT(QuestHasItemReward(QUEST_ID_NEW_FRONTIER, ITEM_BUILDING_SUPPLIES, QUEST_REWARD_SMALL_BUILD_AMOUNT));
    EXPECT(QuestHasItemReward(QUEST_ID_NEW_FRONTIER, ITEM_BATTLE_CHECKER, 1));
}

TEST("Treasure Hunter completes upon entering a Treasure room for the first time")
{
    ResetQuestTestState();

    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_TREASURE_HUNTER));
    EXPECT(RogueQuest_IsQuestActive(QUEST_ID_TREASURE_HUNTER));

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_ROUTE;
    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_ENCOUNTER);
    EXPECT(!RogueQuest_GetStateFlag(QUEST_ID_TREASURE_HUNTER, QUEST_STATE_HAS_COMPLETE));

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_ITEM;
    RogueQuest_OnTrigger(QUEST_TRIGGER_ENTER_ENCOUNTER);
    EXPECT(RogueQuest_GetStateFlag(QUEST_ID_TREASURE_HUNTER, QUEST_STATE_HAS_COMPLETE));
    EXPECT(RogueQuest_HasPendingRewards(QUEST_ID_TREASURE_HUNTER));

    ClearQuestTestState();
}

TEST("Frontier Brain completes only from a defeated Frontier Brain room")
{
    ResetQuestTestState();

    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_FRONTIER_BRAIN));
    EXPECT(RogueQuest_IsQuestActive(QUEST_ID_FRONTIER_BRAIN));

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_ROUTE;
    RogueQuest_OnTrigger(QUEST_TRIGGER_MAP_SPECIFIC_EVENT);
    EXPECT(!RogueQuest_GetStateFlag(QUEST_ID_FRONTIER_BRAIN, QUEST_STATE_HAS_COMPLETE));

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_MINIBOSS;
    RogueQuest_OnTrigger(QUEST_TRIGGER_MAP_SPECIFIC_EVENT);
    EXPECT(RogueQuest_GetStateFlag(QUEST_ID_FRONTIER_BRAIN, QUEST_STATE_HAS_COMPLETE));
    EXPECT(RogueQuest_HasPendingRewards(QUEST_ID_FRONTIER_BRAIN));

    ClearQuestTestState();
}

TEST("New Frontier completes after clearing a Battle Tower")
{
    ResetQuestTestState();

    EXPECT(RogueQuest_IsQuestUnlocked(QUEST_ID_NEW_FRONTIER));
    EXPECT(RogueQuest_IsQuestActive(QUEST_ID_NEW_FRONTIER));

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_ROUTE;
    RogueQuest_OnTrigger(QUEST_TRIGGER_MAP_SPECIFIC_EVENT);
    EXPECT(!RogueQuest_GetStateFlag(QUEST_ID_NEW_FRONTIER, QUEST_STATE_HAS_COMPLETE));

    gRogueAdvPath.currentRoomType = ADVPATH_ROOM_BATTLE_TOWER;
    RogueQuest_OnTrigger(QUEST_TRIGGER_MAP_SPECIFIC_EVENT);
    EXPECT(RogueQuest_GetStateFlag(QUEST_ID_NEW_FRONTIER, QUEST_STATE_HAS_COMPLETE));
    EXPECT(RogueQuest_HasPendingRewards(QUEST_ID_NEW_FRONTIER));

    ClearQuestTestState();
}
