#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
}

SINGLE_BATTLE_TEST("Swallowed uses Stockpile after the first biting move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO) { Speed(100); Ability(ABILITY_UNAWARE); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SWALLOWED);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SWALLOWED);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].stockpileCounter, 1);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Swallowed does not trigger after non-biting moves")
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
