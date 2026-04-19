#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Switchstep boosts damage by 1.5x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SWITCHSTEP; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_POUND); UniqueAbility(uniqueAbility); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_POUND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POUND, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage * 2, results[0].damage * 3 - 2);
        EXPECT_LT(results[1].damage * 2, results[0].damage * 3 + 2);
    }
}

SINGLE_BATTLE_TEST("Switchstep prevents consecutive move uses")
{
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Moves(MOVE_SPLASH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH, allowed: FALSE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPLASH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Switchstep forces Struggle if the only move is prevented")
{
    GIVEN {
        PLAYER(SPECIES_ZIGZAGOON) { Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SPLASH, allowed: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPLASH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, player);
    }
}
