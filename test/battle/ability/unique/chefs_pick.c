#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].type == TYPE_DRAGON);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_HELPING_HAND].effect == EFFECT_HELPING_HAND);
    ASSUME(gBattleMoves[MOVE_FOCUS_ENERGY].effect == EFFECT_FOCUS_ENERGY);
}

SINGLE_BATTLE_TEST("Chef's Pick uses Focus Energy after the first Dragon-type move in singles")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Speed(100); Ability(ABILITY_COMMANDER); Moves(MOVE_DRAGON_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CLAW); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DRAGON_CLAW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_CLAW, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_CHEFS_PICK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        MESSAGE("Tatsugiri is getting pumped!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_CLAW, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHEFS_PICK);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        }
    } THEN {
        EXPECT(player->status2 & STATUS2_FOCUS_ENERGY);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

DOUBLE_BATTLE_TEST("Chef's Pick uses Helping Hand on its ally after the first Dragon-type move in doubles")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI_DROOPY) { Speed(100); Ability(ABILITY_COMMANDER); Moves(MOVE_DRAGON_CLAW); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(30); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_DRAGON_CLAW, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_CLAW, playerLeft);
        HP_BAR(opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_CHEFS_PICK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HELPING_HAND, playerLeft);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Chef's Pick does not trigger after non-Dragon moves")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI_STRETCHY) { Speed(100); Ability(ABILITY_COMMANDER); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHEFS_PICK);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
        }
    } THEN {
        EXPECT(!(player->status2 & STATUS2_FOCUS_ENERGY));
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}
