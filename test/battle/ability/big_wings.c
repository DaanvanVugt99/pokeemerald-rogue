#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Big wings increases the power of wind moves", s16 damage)
{
    u32 move;
    u16 ability;
    ASSUME(gBattleMoves[MOVE_AIR_CUTTER].flags & FLAG_WIND_BASED);
    ASSUME(!(gBattleMoves[MOVE_SCRATCH].flags & FLAG_WIND_BASED));
    PARAMETRIZE
    {
        move = MOVE_AIR_CUTTER;
        ability = ABILITY_BIG_WINGS;
    }
    PARAMETRIZE
    {
        move = MOVE_AIR_CUTTER;
        ability = ABILITY_KEEN_EYE;
    }
    PARAMETRIZE
    {
        move = MOVE_SCRATCH;
        ability = ABILITY_BIG_WINGS;
    }
    PARAMETRIZE
    {
        move = MOVE_SCRATCH;
        ability = ABILITY_KEEN_EYE;
    }
    GIVEN
    {
        PLAYER(SPECIES_PIDGEOT) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage); // big wings affects wind moves
        EXPECT_EQ(results[2].damage, results[3].damage);                  // big wings does not affect non-wind moves
    }
}