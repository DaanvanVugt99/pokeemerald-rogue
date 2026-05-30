#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_REST].effect == EFFECT_REST);
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
}

SINGLE_BATTLE_TEST("Swallowed uses Stockpile after the first successful Rest each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO) { HP(50); MaxHP(100); Speed(100); Ability(ABILITY_UNAWARE); Moves(MOVE_REST); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_REST); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REST, player);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_SWALLOWED);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].stockpileCounter, 1);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Swallowed does not trigger after non-Rest moves")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO) { Speed(100); Ability(ABILITY_UNAWARE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SWALLOWED);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].stockpileCounter, 0);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Swallowed does not trigger if Rest is blocked")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO) { HP(50); MaxHP(100); Speed(100); Ability(ABILITY_INSOMNIA); Moves(MOVE_REST); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_REST); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SWALLOWED);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].stockpileCounter, 0);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
