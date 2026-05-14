#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
}

SINGLE_BATTLE_TEST("Mouthful uses Stockpile on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CRAMORANT) { Ability(ABILITY_GULP_MISSILE); UniqueAbility(ABILITY_MOUTHFUL); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MOUTHFUL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, opponent);
    } THEN {
        EXPECT_EQ(gDisableStructs[B_POSITION_OPPONENT_LEFT].stockpileCounter, 1);
    }
}

SINGLE_BATTLE_TEST("Mouthful uses Stockpile after knocking out a target")
{
    GIVEN {
        PLAYER(SPECIES_CRAMORANT) { Speed(100); Ability(ABILITY_GULP_MISSILE); UniqueAbility(ABILITY_MOUTHFUL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOUTHFUL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
    } THEN {
        EXPECT_EQ(gDisableStructs[B_POSITION_PLAYER_LEFT].stockpileCounter, 2);
    }
}
