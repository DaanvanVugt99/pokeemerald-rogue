#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_REST].effect == EFFECT_REST);
}

SINGLE_BATTLE_TEST("Hypnosis guarantees one sleep turn, can wake on turn two, and wakes by turn three")
{
    bool32 wakesOnTurnTwo;
    ASSUME(B_SLEEP_TURNS >= GEN_9);
    PARAMETRIZE { wakesOnTurnTwo = TRUE; }
    PARAMETRIZE { wakesOnTurnTwo = FALSE; }

    PASSES_RANDOMLY(wakesOnTurnTwo ? 1 : 2, 3, RNG_SLEEP_TURNS);

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
        if (!wakesOnTurnTwo)
            TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
        MESSAGE("Foe Wobbuffet is fast asleep.");
        if (!wakesOnTurnTwo)
            MESSAGE("Foe Wobbuffet is fast asleep.");
        MESSAGE("Foe Wobbuffet woke up!");
        STATUS_ICON(opponent, none: TRUE);
        MESSAGE("Foe Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Rest sleep remains deterministic under Gen 9 sleep rules")
{
    ASSUME(B_SLEEP_TURNS >= GEN_9);

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_REST, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_REST); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_SLEEP_TURNS, 0)); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet went to sleep!");
        MESSAGE("Wobbuffet is fast asleep.");
        MESSAGE("Wobbuffet is fast asleep.");
        MESSAGE("Wobbuffet woke up!");
        MESSAGE("Wobbuffet used Celebrate!");
    }
}
