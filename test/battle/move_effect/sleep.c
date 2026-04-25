#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
}

SINGLE_BATTLE_TEST("Hypnosis inflicts 2-3 turns of sleep")
{
    u32 turns, count, passes;
    ASSUME(B_SLEEP_TURNS >= GEN_9);
    PARAMETRIZE { turns = 2; passes = 1; }
    PARAMETRIZE { turns = 3; passes = 2; }
    PASSES_RANDOMLY(passes, 3, RNG_SLEEP_TURNS);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS); MOVE(opponent, MOVE_CELEBRATE); }
        for (count = 0; count < turns; ++count)
            TURN {}
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        MESSAGE("Foe Wobbuffet fell asleep!");
        STATUS_ICON(opponent, sleep: TRUE);
        for (count = 0; count < turns; ++count)
        {
            if (count < turns - 1)
                MESSAGE("Foe Wobbuffet is fast asleep.");
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        }
        MESSAGE("Foe Wobbuffet woke up!");
        STATUS_ICON(opponent, none: TRUE);
    }
}
