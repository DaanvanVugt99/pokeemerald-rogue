#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHELL_SIDE_ARM].effect == EFFECT_SHELL_SIDE_ARM);
    ASSUME(gBattleMoves[MOVE_SHELL_SIDE_ARM].priority == 0);
    ASSUME(gBattleMoves[MOVE_MUD_SHOT].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_ACID_SPRAY].power == 40);
}

SINGLE_BATTLE_TEST("Galarica Rounds loads on a super-effective hit and gives Shell Side Arm priority")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALARIAN) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_GALARICA_ROUNDS); Moves(MOVE_CELEBRATE, MOVE_SHELL_SIDE_ARM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_SHADOW_TAG); SpAttack(100); Moves(MOVE_MUD_SHOT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MUD_SHOT); }
        TURN { MOVE(player, MOVE_SHELL_SIDE_ARM, WITH_RNG(RNG_ROGUE_GALARICA_ROUNDS, MOVE_ACID_SPRAY)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MUD_SHOT, opponent);
        ABILITY_POPUP(player, ABILITY_GALARICA_ROUNDS);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_GALARICA_ROUNDS_LOAD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SIDE_ARM, player);
        ABILITY_POPUP(player, ABILITY_GALARICA_ROUNDS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Galarica Rounds does not load on neutral hits")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALARIAN) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_GALARICA_ROUNDS); Moves(MOVE_CELEBRATE, MOVE_SHELL_SIDE_ARM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_SHELL_SIDE_ARM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_GALARICA_ROUNDS);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SIDE_ARM, player);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Galarica Rounds is not consumed if Shell Side Arm fails")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALARIAN) { HP(400); MaxHP(400); Speed(100); Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_GALARICA_ROUNDS); Moves(MOVE_CELEBRATE, MOVE_SHELL_SIDE_ARM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_SHADOW_TAG); SpAttack(100); Moves(MOVE_MUD_SHOT, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MUD_SHOT); }
        TURN { MOVE(player, MOVE_SHELL_SIDE_ARM); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GALARICA_ROUNDS);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Galarica Rounds is consumed if Shell Side Arm faints the only active target")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALARIAN) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_GALARICA_ROUNDS); Moves(MOVE_CELEBRATE, MOVE_SHELL_SIDE_ARM); }
        OPPONENT(SPECIES_CLEFAIRY) { HP(1); MaxHP(1); Speed(100); Ability(ABILITY_BATTLE_ARMOR); SpAttack(100); Moves(MOVE_MUD_SHOT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MUD_SHOT); }
        TURN { MOVE(player, MOVE_SHELL_SIDE_ARM); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_SHELL_SIDE_ARM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GALARICA_ROUNDS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SIDE_ARM, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SIDE_ARM, player);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

DOUBLE_BATTLE_TEST("Galarica Rounds retargets the round if Shell Side Arm faints its target")
{
    GIVEN {
        PLAYER(SPECIES_SLOWBRO_GALARIAN) { HP(400); MaxHP(400); Speed(100); Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_GALARICA_ROUNDS); Moves(MOVE_CELEBRATE, MOVE_SHELL_SIDE_ARM); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CLEFAIRY) { HP(1); MaxHP(1); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_MUD_SHOT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_MUD_SHOT, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SHELL_SIDE_ARM, target: opponentLeft, WITH_RNG(RNG_ROGUE_GALARICA_ROUNDS, MOVE_ACID_SPRAY)); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_GALARICA_ROUNDS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SIDE_ARM, playerLeft);
        HP_BAR(opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_GALARICA_ROUNDS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, playerLeft);
        HP_BAR(opponentRight);
    }
}
