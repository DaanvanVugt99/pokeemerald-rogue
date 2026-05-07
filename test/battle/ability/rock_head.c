#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
}

SINGLE_BATTLE_TEST("Rock Head converts Normal-type moves to Rock-type", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_ROCK_HEAD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); Ability(ability); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CHARIZARD) { HP(1000); MaxHP(1000); Defense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 3);
    }
}

SINGLE_BATTLE_TEST("Rock Head converts dynamically Normal-type moves to Rock-type", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_ROCK_HEAD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ability); Moves(MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_CHARIZARD) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 3);
    }
}
