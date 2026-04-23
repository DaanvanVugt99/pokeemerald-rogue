#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_NIGHTFALL].effect == EFFECT_OVERHEAT);
    ASSUME(gBattleMoves[MOVE_NIGHTFALL].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_NIGHTFALL].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_NIGHTFALL].power == 130);
    ASSUME(gBattleMoves[MOVE_NIGHTFALL].accuracy == 90);
    ASSUME(gBattleMoves[MOVE_NIGHTFALL].pp == 5);
}

SINGLE_BATTLE_TEST("Nightfall deals damage normally")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_NIGHTFALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_NIGHTFALL); }
    } SCENE {
        MESSAGE("Wobbuffet used Nightfall!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NIGHTFALL, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Sp. Atk harshly fell!");
    }
}

SINGLE_BATTLE_TEST("Nightfall harshly lowers the user's Sp. Atk")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_NIGHTFALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); }
    } WHEN {
        TURN { MOVE(player, MOVE_NIGHTFALL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
    }
}
