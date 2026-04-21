#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
    ASSUME(gSpeciesInfo[SPECIES_EKANS].types[0] == TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_POISON);
}

SINGLE_BATTLE_TEST("Vendetta gives +1 priority to the first slicing move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ZANGOOSE) { Speed(50); Ability(ABILITY_IMMUNITY); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}

SINGLE_BATTLE_TEST("Vendetta is not consumed by non-slicing moves")
{
    GIVEN {
        PLAYER(SPECIES_ZANGOOSE) { Speed(50); Ability(ABILITY_IMMUNITY); Moves(MOVE_TACKLE, MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

DOUBLE_BATTLE_TEST("Vendetta boosts damage against Poison-type targets")
{
    s16 vendettaDamage;
    s16 controlDamage;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_VENDETTA); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_EKANS) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_EKANS) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); MOVE(playerRight, MOVE_TACKLE, target: opponentRight); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &vendettaDamage);
        HP_BAR(opponentRight, captureDamage: &controlDamage);
    } THEN {
        EXPECT_GT(vendettaDamage * 2, controlDamage * 3 - 2);
        EXPECT_LT(vendettaDamage * 2, controlDamage * 3 + 2);
    }
}
