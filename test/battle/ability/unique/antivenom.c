#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Antivenom always critically hits Poison-type targets")
{
    PASSES_RANDOMLY(100, 100, RNG_CRITICAL_HIT);
    GIVEN {
        PLAYER(SPECIES_ZANGOOSE) { Ability(ABILITY_IMMUNITY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_EKANS) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Antivenom restores 1/8 max HP after KOing a Pokemon")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_ZANGOOSE) { HP(120); MaxHP(160); Ability(ABILITY_IMMUNITY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_ANTIVENOM);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -20);
    }
}

DOUBLE_BATTLE_TEST("Antivenom does not guarantee critical hits against allied Poison-type targets")
{
    PASSES_RANDOMLY(23, 24, RNG_CRITICAL_HIT);
    GIVEN {
        PLAYER(SPECIES_ZANGOOSE) { Ability(ABILITY_IMMUNITY); Moves(MOVE_SURF); }
        PLAYER(SPECIES_EKANS) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_SURF); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("A critical hit!");
        }
    }
}
