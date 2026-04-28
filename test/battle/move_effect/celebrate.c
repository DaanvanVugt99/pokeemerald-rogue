#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CELEBRATE].effect == EFFECT_CELEBRATE);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].pp == 10);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].healBlockBanned);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].snatchAffected);
}

SINGLE_BATTLE_TEST("Celebrate restores a quarter of the user's max HP and raises Speed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Celebrate fails while the user is under Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_HEAL_BLOCK); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        MESSAGE("Wobbuffet was prevented from healing!");
        MESSAGE("Wobbuffet was prevented from healing!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Snatch steals Celebrate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(200); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(200); MaxHP(400); Moves(MOVE_SNATCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SNATCH); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNATCH, opponent);
        MESSAGE("Foe Wobbuffet SNATCHED Wobbuffet's move!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->hp, 300);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Celebrate raises Speed even when the user is at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->hp, 400);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
