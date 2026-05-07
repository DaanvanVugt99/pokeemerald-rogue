#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_ROUND].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Anger Point raises Attack by 1 when hit by an opposing physical move")
{
    u16 move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_ROUND; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_ANGER_POINT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        HP_BAR(player);
        if (move == MOVE_TACKLE) {
            ABILITY_POPUP(player, ABILITY_ANGER_POINT);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Wobbuffet's Attack rose!");
        } else {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_ANGER_POINT);
                MESSAGE("Wobbuffet's Attack rose!");
            }
        }
    } THEN {
        if (move == MOVE_TACKLE)
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        else
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Anger Point maxes Attack when hit by an opposing physical critical hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_ANGER_POINT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, criticalHit: TRUE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        MESSAGE("A critical hit!");
        ABILITY_POPUP(player, ABILITY_ANGER_POINT);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Anger Point maxed its Attack!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], MAX_STAT_STAGE);
    }
}
