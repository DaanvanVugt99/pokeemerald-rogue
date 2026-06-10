#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
    ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
    ASSUME(gBattleMoves[MOVE_LUNGE].effect == EFFECT_ATTACK_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_LUNGE].makesContact == TRUE);
    ASSUME(gBattleMoves[MOVE_ELECTROWEB].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_ELECTROWEB].target == MOVE_TARGET_BOTH);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
    ASSUME(gBattleMoves[MOVE_SYRUP_BOMB].effect == EFFECT_SYRUP_BOMB);
}

SINGLE_BATTLE_TEST("Pending unique effects clear state after a residual script drain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SYRUP_BOMB); }
        OPPONENT(SPECIES_OINKOLOGNE) { Speed(10); Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SYRUP_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SYRUP_BOMB_SPEED_DROP, opponent);
        ABILITY_POPUP(opponent, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}

SINGLE_BATTLE_TEST("Pending unique effects drain multiple effects after one residual Speed drop")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_SYRUP_BOMB); }
        OPPONENT(SPECIES_OINKOLOGNE) { Speed(100); Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SYRUP_BOMB, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SYRUP_BOMB_SPEED_DROP, opponent);
        ABILITY_POPUP(opponent, ABILITY_AROMA_TRAIL);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_LT(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}

SINGLE_BATTLE_TEST("Pending unique effects clear state after a switch-in script drain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STICKY_WEB, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Oinkologne was caught in a Sticky Web!");
        ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}

SINGLE_BATTLE_TEST("Pending unique effects clear state after a move-end drain")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MAGICIAN); Item(ITEM_NONE); Moves(MOVE_LUNGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_LUNGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
        ABILITY_POPUP(opponent, ABILITY_MAGICIAN);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_ORAN_BERRY);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}

DOUBLE_BATTLE_TEST("Pending unique effects clear all slots after a spread move queues multiple effects")
{
    GIVEN {
        PLAYER(SPECIES_LECHONK) { Speed(50); Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_OINKOLOGNE) { Speed(40); Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_GROWL);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_AROMA_TRAIL);
        ABILITY_POPUP(playerRight, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}

DOUBLE_BATTLE_TEST("Pending unique effects keep called move-end context while a drain is already active")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Speed(100); MaxHP(100); HP(100); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Item(ITEM_LIFE_ORB); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); MaxHP(100); HP(100); Moves(MOVE_SCARY_FACE); }
        OPPONENT(SPECIES_OINKOLOGNE) { Speed(50); MaxHP(100); HP(100); Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); MaxHP(100); HP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SCARY_FACE, target: opponentLeft, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_ELECTROWEB));
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTROWEB, playerLeft);
        ABILITY_POPUP(opponentLeft, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_LT(playerLeft->hp, 100);
        EXPECT_EQ(playerRight->hp, 100);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}

SINGLE_BATTLE_TEST("Pending unique effects stay clear after Scrap Job uses the shared pending storage")
{
    GIVEN {
        PLAYER(SPECIES_TINKATON) { Speed(1); MaxHP(500); HP(500); Item(ITEM_LEFTOVERS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityEffects, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilityBattlers, 0);
        EXPECT_EQ(gBattleStruct->pendingUniqueAbilitySavedContext, 0);
    }
}
