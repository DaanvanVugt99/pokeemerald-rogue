#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_POUND].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_TAUNT].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_BELLY_DRUM].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Stone Seal disables the first physical move to hit Regirock each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_REGIROCK) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_STONE_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_STONE_SEAL);
        MESSAGE("Foe Wobbuffet's Tackle was disabled by Regirock's Stone Seal!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Stone Seal does not activate after a special move")
{
    GIVEN {
        PLAYER(SPECIES_REGIROCK) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_STONE_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STONE_SEAL);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Frost Seal disables the first special move to hit Regice each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_REGICE) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FROST_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ABILITY_POPUP(player, ABILITY_FROST_SEAL);
        MESSAGE("Foe Wobbuffet's Water Gun was disabled by Regice's Frost Seal!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_WATER_GUN);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Frost Seal does not activate after a physical move")
{
    GIVEN {
        PLAYER(SPECIES_REGICE) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FROST_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FROST_SEAL);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Regi seals only activate once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_REGIROCK) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_STONE_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Moves(MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_POUND); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STONE_SEAL);
        MESSAGE("Foe Wobbuffet's Tackle was disabled by Regirock's Stone Seal!");
        NONE_OF {
            MESSAGE("Foe Wynaut's Pound was disabled by Regirock's Stone Seal!");
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Iron Seal disables an opposing self-targeted status move")
{
    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_IRON_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ABILITY_POPUP(player, ABILITY_IRON_SEAL);
        MESSAGE("Foe Wobbuffet's Swords Dance was disabled by Registeel's Iron Seal!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_SWORDS_DANCE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Iron Seal disables an opposing targeted status move")
{
    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_IRON_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TAUNT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAUNT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, opponent);
        ABILITY_POPUP(player, ABILITY_IRON_SEAL);
        MESSAGE("Foe Wobbuffet's Taunt was disabled by Registeel's Iron Seal!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TAUNT);
    }
}

SINGLE_BATTLE_TEST("Iron Seal does not consume its activation when an opposing status move fails")
{
    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_IRON_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(50); MaxHP(100); Moves(MOVE_BELLY_DRUM, MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BELLY_DRUM); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Foe Wobbuffet's Belly Drum was disabled by Registeel's Iron Seal!");
        }
        ABILITY_POPUP(player, ABILITY_IRON_SEAL);
        MESSAGE("Foe Wobbuffet's Swords Dance was disabled by Registeel's Iron Seal!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_SWORDS_DANCE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Iron Seal does not suppress the move user's own status-move ability")
{
    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_IRON_SEAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(100); MaxHP(200); UniqueAbility(ABILITY_TIDEBORN); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_TIDEBORN);
        ABILITY_POPUP(player, ABILITY_IRON_SEAL);
        MESSAGE("Foe Wobbuffet's Swords Dance was disabled by Registeel's Iron Seal!");
    } THEN {
        EXPECT_EQ(opponent->hp, 150);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_SWORDS_DANCE);
    }
}
