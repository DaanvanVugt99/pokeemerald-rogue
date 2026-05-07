#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Snow Cloak prevents damage from hail")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_GLACEON) { Ability(ABILITY_SNOW_CLOAK); }
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); MOVE(opponent, MOVE_SKILL_SWAP); }
    } SCENE {
        NONE_OF { HP_BAR(player); }
    }
}

SINGLE_BATTLE_TEST("Snow Cloak takes 30 percent less damage during snow or hail", s16 damage)
{
    u32 weatherMove;
    u32 ability;

    PARAMETRIZE { weatherMove = MOVE_CELEBRATE; ability = ABILITY_NONE; }
    PARAMETRIZE { weatherMove = MOVE_CELEBRATE; ability = ABILITY_SNOW_CLOAK; }
    PARAMETRIZE { weatherMove = MOVE_HAIL; ability = ABILITY_NONE; }
    PARAMETRIZE { weatherMove = MOVE_HAIL; ability = ABILITY_SNOW_CLOAK; }
    PARAMETRIZE { weatherMove = MOVE_SNOWSCAPE; ability = ABILITY_NONE; }
    PARAMETRIZE { weatherMove = MOVE_SNOWSCAPE; ability = ABILITY_SNOW_CLOAK; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power != 0);
        ASSUME(gBattleMoves[MOVE_HAIL].effect == EFFECT_HAIL);
        ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(weatherMove, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        if (weatherMove != MOVE_CELEBRATE)
            TURN { MOVE(player, weatherMove); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.7), results[3].damage);
        EXPECT_EQ(results[0].damage, results[4].damage);
        EXPECT_MUL_EQ(results[4].damage, Q_4_12(0.7), results[5].damage);
    }
}
