#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WORK_UP].effect == EFFECT_ATTACK_SPATK_UP);
    ASSUME(gBattleMoves[MOVE_AGILITY].type == TYPE_PSYCHIC);
    ASSUME(IS_MOVE_STATUS(MOVE_AGILITY));
    ASSUME(gBattleMoves[MOVE_CELEBRATE].type != TYPE_PSYCHIC);
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(gBattleMoves[MOVE_TAR_SHOT].effect == EFFECT_TAR_SHOT);
    ASSUME(gBattleMoves[MOVE_CONFUSE_RAY].type == TYPE_GHOST);
    ASSUME(IS_MOVE_STATUS(MOVE_CONFUSE_RAY));
    ASSUME(gBattleMoves[MOVE_SHADOW_CLAW].slicingMove);
}

SINGLE_BATTLE_TEST("Tempered Ember uses Work Up after the first Fire-type move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_CHARCADET) { Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_TEMPERED_EMBER); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_TEMPERED_EMBER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORK_UP, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TEMPERED_EMBER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WORK_UP, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Tempered Ember refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_CHARCADET) { Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_TEMPERED_EMBER); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_TEMPERED_EMBER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORK_UP, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_TEMPERED_EMBER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORK_UP, player);
    }
}

SINGLE_BATTLE_TEST("Tar Cannon uses Tar Shot after a Psychic-type status move")
{
    GIVEN {
        PLAYER(SPECIES_ARMAROUGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_TAR_CANNON); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AGILITY, player);
        ABILITY_POPUP(player, ABILITY_TAR_CANNON);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAR_SHOT, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Tar Cannon does not trigger after non-Psychic status moves")
{
    GIVEN {
        PLAYER(SPECIES_ARMAROUGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_TAR_CANNON); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TAR_CANNON);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAR_SHOT, player);
        }
    }
}

SINGLE_BATTLE_TEST("Soul Brand makes the next slicing move a critical hit after a Ghost-type status move")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_CONFUSE_RAY, MOVE_SHADOW_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHADOW_CLAW, criticalHit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
        ABILITY_POPUP(player, ABILITY_SOUL_BRAND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_CLAW, player);
        MESSAGE("A critical hit!");
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Soul Brand does not force a critical hit without a Ghost-type status move")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_SHADOW_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_CLAW, criticalHit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_CLAW, player);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Soul Brand is not consumed when the slicing move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_CONFUSE_RAY, MOVE_SHADOW_CLAW); }
        OPPONENT(SPECIES_SKITTY) { Ability(ABILITY_NORMALIZE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSE_RAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHADOW_CLAW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SOUL_BRAND);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

DOUBLE_BATTLE_TEST("Soul Brand only activates for the battler that used the Ghost-type status move")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_CONFUSE_RAY); }
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CONFUSE_RAY, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SOUL_BRAND);
        NONE_OF {
            ABILITY_POPUP(playerRight, ABILITY_SOUL_BRAND);
        }
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)].uniquePersistentStateActive);
    }
}

DOUBLE_BATTLE_TEST("Soul Brand slicing-move consumption only applies to the slicing move user")
{
    GIVEN {
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_CONFUSE_RAY, MOVE_SHADOW_CLAW); }
        PLAYER(SPECIES_CERULEDGE) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_SOUL_BRAND); Moves(MOVE_CONFUSE_RAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CONFUSE_RAY, target: opponentLeft); MOVE(playerRight, MOVE_CONFUSE_RAY, target: opponentRight); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_SHADOW_CLAW, target: opponentLeft, criticalHit: FALSE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)].uniquePersistentStateActive);
    }
}
