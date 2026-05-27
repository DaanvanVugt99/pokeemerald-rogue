#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACHYON_CUTTER].target == MOVE_TARGET_SELECTED);
    ASSUME(gBattleMoves[MOVE_TACHYON_CUTTER].strikeCount == 2);
    ASSUME(!IS_MOVE_STATUS(MOVE_TACHYON_CUTTER));
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
}

DOUBLE_BATTLE_TEST("Singularity Array makes Iron Crown's first damaging move hit both opposing Pokemon")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_CROWN) { Level(100); SpAttack(300); Speed(100); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_TACHYON_CUTTER); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_BUNDLE) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(2000); MaxHP(2000); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACHYON_CUTTER, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACHYON_CUTTER, playerLeft);
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);

        if (hasOtherParadox)
        {
            EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}

DOUBLE_BATTLE_TEST("Singularity Array ignores status moves before Iron Crown's first damaging move")
{
    GIVEN {
        PLAYER(SPECIES_IRON_CROWN) { Level(100); SpAttack(300); Speed(100); Ability(ABILITY_QUARK_DRIVE); Moves(MOVE_CELEBRATE, MOVE_TACHYON_CUTTER); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(2000); MaxHP(2000); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_TACHYON_CUTTER, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
