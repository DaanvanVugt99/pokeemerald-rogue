#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
    ASSUME(gSpeciesInfo[SPECIES_CHANSEY].types[0] == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Sidewinder gives +1 priority to the first biting move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SEVIPER) { Speed(50); Ability(ABILITY_SHED_SKIN); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
    }
}

SINGLE_BATTLE_TEST("Sidewinder does not give priority to non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_SEVIPER) { Speed(50); Ability(ABILITY_SHED_SKIN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

DOUBLE_BATTLE_TEST("Sidewinder boosts damage against Normal-type targets")
{
    s16 sidewinderDamage;
    s16 controlDamage;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SIDEWINDER); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CHANSEY) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHANSEY) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); MOVE(playerRight, MOVE_TACKLE, target: opponentRight); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &sidewinderDamage);
        HP_BAR(opponentRight, captureDamage: &controlDamage);
    } THEN {
        EXPECT_GT(sidewinderDamage * 2, controlDamage * 3 - 2);
        EXPECT_LT(sidewinderDamage * 2, controlDamage * 3 + 2);
    }
}
