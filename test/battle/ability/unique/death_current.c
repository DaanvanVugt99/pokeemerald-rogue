#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_SONIC_BOOM].power > 0);
}

SINGLE_BATTLE_TEST("Death Current traps with Water and Ghost moves if any Pokemon fainted last turn")
{
    u16 move;

    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_SHADOW_BALL; }

    GIVEN {
        PLAYER(SPECIES_BASCULEGION) { Speed(100); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_SONIC_BOOM, move); }
        OPPONENT(SPECIES_WYNAUT) { HP(20); MaxHP(20); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DEATH_CURRENT);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].battlerPreventingEscape,
                  GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Death Current does not trap without a faint last turn")
{
    GIVEN {
        PLAYER(SPECIES_BASCULEGION) { Speed(100); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_DEATH_CURRENT);
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
    }
}
