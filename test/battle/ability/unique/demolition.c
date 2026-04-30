#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
}

SINGLE_BATTLE_TEST("Demolition removes the target side's barriers with punching moves and lowers Defense by 2")
{
    GIVEN {
        PLAYER(SPECIES_CONKELDURR) { Speed(50); UniqueAbility(ABILITY_DEMOLITION); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); Defense(200); Moves(MOVE_REFLECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, MOVE_MEGA_PUNCH); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT));
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Demolition does not trigger on non-punching moves")
{
    GIVEN {
        PLAYER(SPECIES_CONKELDURR) { Speed(50); UniqueAbility(ABILITY_DEMOLITION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_REFLECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEMOLITION);
            MESSAGE("The wall shattered!");
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Demolition does not trigger when no barrier is present")
{
    GIVEN {
        PLAYER(SPECIES_CONKELDURR) { UniqueAbility(ABILITY_DEMOLITION); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEMOLITION);
            MESSAGE("The wall shattered!");
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Demolition does not remove barriers when the punching move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_CONKELDURR) { Speed(50); UniqueAbility(ABILITY_DEMOLITION); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_GASTLY) { Speed(100); Moves(MOVE_REFLECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_REFLECT); MOVE(player, MOVE_MEGA_PUNCH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEMOLITION);
            MESSAGE("The wall shattered!");
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_REFLECT);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

