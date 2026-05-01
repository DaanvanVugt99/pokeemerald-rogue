#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Barrage is a Grass-type special 20 BP multi-hit move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMoves[MOVE_BARRAGE].effect, EFFECT_MULTI_HIT);
        EXPECT_EQ(gBattleMoves[MOVE_BARRAGE].type, TYPE_GRASS);
        EXPECT_EQ(gBattleMoves[MOVE_BARRAGE].split, SPLIT_SPECIAL);
        EXPECT_EQ(gBattleMoves[MOVE_BARRAGE].power, 20);
        EXPECT_EQ(gBattleMoves[MOVE_BARRAGE].accuracy, 100);
        EXPECT_EQ(gBattleMoves[MOVE_BARRAGE].pp, 10);
    }
}

SINGLE_BATTLE_TEST("Barrage hits five times in sun")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_BARRAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BARRAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BARRAGE, player);
        MESSAGE("Hit 5 time(s)!");
    }
}
