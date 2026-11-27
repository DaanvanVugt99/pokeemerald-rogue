#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LAST_RESORT].effect == EFFECT_LAST_RESORT);
}

SINGLE_BATTLE_TEST("Last Resort forces Struggle if it is the only known move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_LAST_RESORT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LAST_RESORT, allowed: FALSE); }
    } SCENE {
        MESSAGE("Wobbuffet used Struggle!");
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Last Resort becomes selectable after using every other move")
{
    u32 count;
    PARAMETRIZE { count = 2; }
    PARAMETRIZE { count = 3; }
    PARAMETRIZE { count = 4; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_LAST_RESORT,
            count >= 3 ? MOVE_SCRATCH : MOVE_NONE, count == 4 ? MOVE_GUST : MOVE_NONE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LAST_RESORT, allowed: FALSE); MOVE(player, MOVE_TACKLE); }
        if (count >= 3)
            TURN { MOVE(player, MOVE_LAST_RESORT, allowed: FALSE); MOVE(player, MOVE_SCRATCH); }
        if (count == 4)
            TURN { MOVE(player, MOVE_LAST_RESORT, allowed: FALSE); MOVE(player, MOVE_GUST); }
        TURN { MOVE(player, MOVE_LAST_RESORT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        if (count >= 3)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        if (count == 4)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LAST_RESORT, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Last Resort works with Sleep Talk")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SLEEP_TALK].effect == EFFECT_SLEEP_TALK);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_LAST_RESORT, MOVE_SLEEP_TALK); Status1(STATUS1_SLEEP_TURN(3)); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SLEEP_TALK); }
        TURN { MOVE(player, MOVE_SLEEP_TALK); }
    } SCENE {
        // Turn 1
        MESSAGE("Wobbuffet used Sleep Talk!");
        MESSAGE("Wobbuffet used Last Resort!");
        NOT MESSAGE("But it failed!");
        HP_BAR(opponent);
        // Turn 2
        MESSAGE("Wobbuffet used Sleep Talk!");
        MESSAGE("Wobbuffet used Last Resort!");
        NOT MESSAGE("But it failed!");
        HP_BAR(opponent);
    }
}
