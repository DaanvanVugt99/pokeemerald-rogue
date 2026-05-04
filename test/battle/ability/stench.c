#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Stench has a 20% chance to force out attackers when hit by contact")
{
    PASSES_RANDOMLY(1, 5, RNG_STENCH);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_STENCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_STENCH);
        MESSAGE("Foe Wynaut was dragged out!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Stench does not force out attackers when hit by non-contact moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power > 0);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_STENCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWIFT, WITH_RNG(RNG_STENCH, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STENCH);
            MESSAGE("Foe Wynaut was dragged out!");
        }
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Stench does not trigger if the attacker cannot switch out")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_STENCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_STENCH, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT ABILITY_POPUP(player, ABILITY_STENCH);
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WOBBUFFET);
    }
}
