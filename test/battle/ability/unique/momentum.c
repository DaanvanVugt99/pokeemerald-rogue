#include "global.h"
#include "test/battle.h"

static u32 CountBoostedStats(struct BattlePokemon *mon)
{
    u32 count = 0;
    u32 statId;

    for (statId = STAT_ATK; statId < NUM_BATTLE_STATS; statId++)
    {
        if (mon->statStages[statId] == DEFAULT_STAT_STAGE + 1)
            count++;
        else
            EXPECT_EQ(mon->statStages[statId], DEFAULT_STAT_STAGE);
    }

    return count;
}

SINGLE_BATTLE_TEST("Momentum raises one random stat after landing a critical hit")
{
    GIVEN {
        PLAYER(SPECIES_SAMUROTT_HISUIAN) { Ability(ABILITY_SHARPNESS); UniqueAbility(ABILITY_MOMENTUM); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK, criticalHit: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOMENTUM);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(CountBoostedStats(player), 1);
    }
}

SINGLE_BATTLE_TEST("Momentum does not trigger without a critical hit")
{
    GIVEN {
        PLAYER(SPECIES_SAMUROTT_HISUIAN) { Ability(ABILITY_SHARPNESS); UniqueAbility(ABILITY_MOMENTUM); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK, criticalHit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MOMENTUM);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        }
    } THEN {
        EXPECT_EQ(CountBoostedStats(player), 0);
    }
}
