#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Egg Bomb is a 120 BP special ballistic move with 85 percent accuracy")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMoves[MOVE_EGG_BOMB].power, 120);
        EXPECT_EQ(gBattleMoves[MOVE_EGG_BOMB].accuracy, 85);
        EXPECT_EQ(gBattleMoves[MOVE_EGG_BOMB].split, SPLIT_SPECIAL);
        EXPECT(gBattleMoves[MOVE_EGG_BOMB].ballisticMove);
    }
}

SINGLE_BATTLE_TEST("Bone Club has a 30 percent flinch chance")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMoves[MOVE_BONE_CLUB].power, 65);
        EXPECT_EQ(gBattleMoves[MOVE_BONE_CLUB].secondaryEffectChance, 30);
        EXPECT_EQ(gBattleMoves[MOVE_BONE_CLUB].effect, EFFECT_FLINCH_HIT);
    }
}

SINGLE_BATTLE_TEST("Needle Arm is a 75 BP move with a 30 percent flinch chance")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMoves[MOVE_NEEDLE_ARM].power, 75);
        EXPECT_EQ(gBattleMoves[MOVE_NEEDLE_ARM].secondaryEffectChance, 30);
        EXPECT_EQ(gBattleMoves[MOVE_NEEDLE_ARM].effect, EFFECT_FLINCH_HIT);
    }
}

SINGLE_BATTLE_TEST("Spider Web traps the target and lowers its Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPIDER_WEB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIDER_WEB); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Spider Web still traps a target whose Speed cannot be lowered")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPIDER_WEB); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_AMULET); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIDER_WEB); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Trump Card becomes stronger with each remaining use", s16 damage)
{
    u8 startingPp;

    PARAMETRIZE { startingPp = 5; }
    PARAMETRIZE { startingPp = 4; }
    PARAMETRIZE { startingPp = 3; }
    PARAMETRIZE { startingPp = 2; }
    PARAMETRIZE { startingPp = 1; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); MovesWithPP({MOVE_TRUMP_CARD, startingPp}); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(100); HP(9999); MaxHP(9999); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRUMP_CARD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
        EXPECT_LT(results[1].damage, results[2].damage);
        EXPECT_LT(results[2].damage, results[3].damage);
        EXPECT_LT(results[3].damage, results[4].damage);
    }
}
