#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type != TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_EMBER].type != TYPE_GHOST);
}

SINGLE_BATTLE_TEST("Hex Instinct disables the target's last used move after Flutter Mane's first Ghost move if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_FLUTTER_MANE) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SHADOW_BALL); }
        if (hasOtherParadox)
            PLAYER(SPECIES_SCREAM_TAIL) { Speed(25); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_SHADOW_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BALL, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_HEX_INSTINCT);
            MESSAGE("Foe Wobbuffet's Water Gun was disabled!");
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_HEX_INSTINCT);
                MESSAGE("Foe Wobbuffet's Water Gun was disabled!");
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_WATER_GUN);
            EXPECT_GT((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 0);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}

SINGLE_BATTLE_TEST("Hex Instinct only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_FLUTTER_MANE) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SHADOW_BALL); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_WATER_GUN, MOVE_EMBER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_SHADOW_BALL); }
        TURN { MOVE(opponent, MOVE_EMBER); MOVE(player, MOVE_SHADOW_BALL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HEX_INSTINCT);
        MESSAGE("Foe Wobbuffet's Water Gun was disabled!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEX_INSTINCT);
            MESSAGE("Foe Wobbuffet's Ember was disabled!");
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_WATER_GUN);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Hex Instinct does not trigger after non-Ghost moves")
{
    GIVEN {
        PLAYER(SPECIES_FLUTTER_MANE) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEX_INSTINCT);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
