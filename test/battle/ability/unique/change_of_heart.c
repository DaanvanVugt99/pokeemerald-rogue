#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HEART_SWAP].effect == EFFECT_HEART_SWAP);
}

SINGLE_BATTLE_TEST("Change of Heart uses Heart Swap on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GOREBYSS) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_CHANGE_OF_HEART); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CALM_MIND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CALM_MIND); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Calm Mind!");
        ABILITY_POPUP(player, ABILITY_CHANGE_OF_HEART);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEART_SWAP, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1]);
    }
}

SINGLE_BATTLE_TEST("Change of Heart only works once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GOREBYSS) { Speed(100); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_CHANGE_OF_HEART); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CALM_MIND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CALM_MIND); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CALM_MIND); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Calm Mind!");
        ABILITY_POPUP(player, ABILITY_CHANGE_OF_HEART);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEART_SWAP, player);
        MESSAGE("Foe Wobbuffet used Calm Mind!");
        NOT ABILITY_POPUP(player, ABILITY_CHANGE_OF_HEART);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1]);
    }
}
