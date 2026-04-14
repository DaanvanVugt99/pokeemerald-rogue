#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
}

SINGLE_BATTLE_TEST("Open Field gives +1 priority against full-HP targets")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Speed(50); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_OPEN_FIELD); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Open Field does not give priority when target is not at full HP")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Speed(50); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_OPEN_FIELD); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(99); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Open Field boosts damage against full-HP targets in Plain Terrain", s16 tackleDamage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_PLAIN_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_OPEN_FIELD); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].tackleDamage);
    } FINALLY {
        EXPECT_GT(results[1].tackleDamage, results[0].tackleDamage * 11 / 10);
    }
}
