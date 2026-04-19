#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
}

SINGLE_BATTLE_TEST("Unmovable blocks forced switches")
{
    GIVEN {
        PLAYER(SPECIES_HARIYAMA) { Ability(ABILITY_GUTS); UniqueAbility(ABILITY_UNMOVABLE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROAR); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Roar!");
        ABILITY_POPUP(player, ABILITY_UNMOVABLE);
        MESSAGE("Hariyama anchors itself with Unmovable!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_HARIYAMA);
    }
}

SINGLE_BATTLE_TEST("Unmovable forces out the attacker on the first contact hit each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_HARIYAMA) { Ability(ABILITY_GUTS); UniqueAbility(ABILITY_UNMOVABLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_UNMOVABLE);
        MESSAGE("Foe Wynaut was dragged out!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
    }
}

DOUBLE_BATTLE_TEST("Unmovable only forces out the first contact hit before switching out")
{
    GIVEN {
        PLAYER(SPECIES_HARIYAMA) { Speed(10); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_UNMOVABLE); }
        PLAYER(SPECIES_WYNAUT) { Speed(5); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(90); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(80); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_UNMOVABLE);
        MESSAGE("Foe Bulbasaur was dragged out!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentRight);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_UNMOVABLE);
            MESSAGE("Foe Wynaut was dragged out!");
        }
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_BULBASAUR);
        EXPECT_EQ(opponentRight->species, SPECIES_WYNAUT);
    }
}
