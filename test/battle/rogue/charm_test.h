#ifndef TEST_BATTLE_ROGUE_CHARM_TEST_H
#define TEST_BATTLE_ROGUE_CHARM_TEST_H

#include "constants/flags.h"
#include "event_data.h"
#include "item.h"
#include "rogue_charms.h"
#include "rogue_controller.h"
#include "rogue_pokedex.h"

static inline void ClearCharmTestState(void)
{
    u32 item;

    for (item = FIRST_ITEM_CHARM; item <= LAST_ITEM_CHARM; item++)
    {
        u16 count = GetItemCountInBag(item);

        if (count != 0)
            RemoveBagItem(item, count);
    }

    for (item = FIRST_ITEM_CURSE; item <= LAST_ITEM_CURSE; item++)
    {
        u16 count = GetItemCountInBag(item);

        if (count != 0)
            RemoveBagItem(item, count);
    }

    FlagClear(FLAG_ROGUE_RUN_ACTIVE);
    RecalcCharmCurseValues();
}

static inline void BeginCharmTestRun(void)
{
    ClearCharmTestState();
    gRogueRun.trialState.trialId = ROGUE_TRIAL_NONE;
    RoguePokedex_SetDexVariant(POKEDEX_VARIANT_NATIONAL_MAX);
    FlagSet(FLAG_ROGUE_RUN_ACTIVE);
}

static inline void AddCharmForTest(u16 item, u16 count)
{
    if (count != 0)
        AddBagItem(item, count);
}

static inline void FinishCharmTestSetup(void)
{
    RecalcCharmCurseValues();
}

static inline void SetSingleCharmForTest(u16 item, u16 count)
{
    BeginCharmTestRun();
    AddCharmForTest(item, count);
    FinishCharmTestSetup();
}

#endif
