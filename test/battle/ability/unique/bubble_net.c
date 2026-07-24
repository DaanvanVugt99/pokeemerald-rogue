#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Bubble Net traps contact attackers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_ARAQUANID) { HP(400); MaxHP(400); Ability(ABILITY_WATER_BUBBLE); UniqueAbility(ABILITY_BUBBLE_NET); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_BUBBLE_NET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].battlerPreventingEscape,
                  GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Bubble Net does not trap non-contact attackers")
{
    GIVEN {
        PLAYER(SPECIES_ARAQUANID) { HP(400); MaxHP(400); Ability(ABILITY_WATER_BUBBLE); UniqueAbility(ABILITY_BUBBLE_NET); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BUBBLE_NET);
        }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_ESCAPE_PREVENTION));
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}

DOUBLE_BATTLE_TEST("Bubble Net only traps the first contact attacker per battle")
{
    GIVEN {
        PLAYER(SPECIES_ARAQUANID) { HP(400); MaxHP(400); Speed(10); Ability(ABILITY_WATER_BUBBLE); UniqueAbility(ABILITY_BUBBLE_NET); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(90); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerLeft);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_BUBBLE_NET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentRight);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_BUBBLE_NET);
        }
    } THEN {
        EXPECT(opponentLeft->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT(!(opponentRight->status2 & STATUS2_ESCAPE_PREVENTION));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
