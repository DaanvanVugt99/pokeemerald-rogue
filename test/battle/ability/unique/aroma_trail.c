#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
    ASSUME(gBattleMoves[MOVE_CLOSE_COMBAT].effect == EFFECT_CLOSE_COMBAT);
    ASSUME(gBattleMoves[MOVE_LUNGE].effect == EFFECT_ATTACK_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_LUNGE].makesContact == TRUE);
    ASSUME(gBattleMoves[MOVE_MUD_SLAP].effect == EFFECT_ACCURACY_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
    ASSUME(gBattleMoves[MOVE_SYRUP_BOMB].effect == EFFECT_SYRUP_BOMB);
}

SINGLE_BATTLE_TEST("Aroma Trail lowers a foe's Accuracy after the foe lowers this Pokemon's stat")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        MESSAGE("Oinkologne's Attack fell!");
        ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
        MESSAGE("Foe Wobbuffet's accuracy fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

DOUBLE_BATTLE_TEST("Aroma Trail drains multiple queued activations after a spread stat drop")
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
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponentLeft);
        MESSAGE("Lechonk's Attack fell!");
        MESSAGE("Oinkologne's Attack fell!");
        ABILITY_POPUP(playerLeft, ABILITY_AROMA_TRAIL);
        MESSAGE("Foe Wobbuffet's accuracy fell!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerRight);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        }
        ABILITY_POPUP(playerRight, ABILITY_AROMA_TRAIL);
        MESSAGE("Foe Wobbuffet's accuracy fell!");
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentLeft->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Aroma Trail preserves move-end context for later attacker abilities")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MAGICIAN); Item(ITEM_NONE); Moves(MOVE_LUNGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_LUNGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LUNGE, opponent);
        MESSAGE("Oinkologne's Attack fell!");
        ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
        MESSAGE("Foe Wobbuffet's accuracy fell!");
        ABILITY_POPUP(opponent, ABILITY_MAGICIAN);
        MESSAGE("Foe Wobbuffet stole Oinkologne's Oran Berry!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_ORAN_BERRY);
    }
}

SINGLE_BATTLE_TEST("Aroma Trail triggers after a foe's Sticky Web lowers this Pokemon's Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STICKY_WEB, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, opponent);
        MESSAGE("Oinkologne was caught in a Sticky Web!");
        ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
        MESSAGE("Foe Wobbuffet's accuracy fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Aroma Trail attributes Syrup Bomb's delayed Speed drop to the syrup user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SYRUP_BOMB); }
        OPPONENT(SPECIES_OINKOLOGNE) { Speed(10); Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SYRUP_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SYRUP_BOMB, player);
        MESSAGE("Foe Oinkologne got covered in sticky syrup!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SYRUP_BOMB_SPEED_DROP, opponent);
        MESSAGE("Foe Oinkologne's Speed fell!");
        ABILITY_POPUP(opponent, ABILITY_AROMA_TRAIL);
        MESSAGE("Wobbuffet's accuracy fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Aroma Trail does not trigger from self-inflicted stat drops")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CLOSE_COMBAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CLOSE_COMBAT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Aroma Trail does not trigger from allied stat drops")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_GROWL, target: playerLeft);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NOT ABILITY_POPUP(playerLeft, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(playerRight->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Aroma Trail does not trigger when the stat drop is blocked")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Aroma Trail does not trigger when the foe's Accuracy is already minimized")
{
    GIVEN {
        PLAYER(SPECIES_OINKOLOGNE) { Ability(ABILITY_AROMA_VEIL); UniqueAbility(ABILITY_AROMA_TRAIL); Moves(MOVE_MUD_SLAP, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_CELEBRATE, MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_MUD_SLAP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MUD_SLAP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MUD_SLAP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MUD_SLAP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MUD_SLAP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MUD_SLAP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_AROMA_TRAIL);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ACC], MIN_STAT_STAGE);
    }
}
