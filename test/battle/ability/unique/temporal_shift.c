#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
}

SINGLE_BATTLE_TEST("Temporal Shift randomly sets Trick Room on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BRAVIARY_HISUIAN);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_TEMPORAL_SHIFT, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TEMPORAL_SHIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}

SINGLE_BATTLE_TEST("Temporal Shift randomly sets Tailwind on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BRAVIARY_HISUIAN);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_TEMPORAL_SHIFT, 1)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TEMPORAL_SHIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Temporal Shift preserves an active Trick Room")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TRICK_ROOM, MOVE_CELEBRATE); }
        PLAYER(SPECIES_BRAVIARY_HISUIAN);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK_ROOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_TEMPORAL_SHIFT, 1)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TEMPORAL_SHIFT);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}

SINGLE_BATTLE_TEST("Temporal Shift preserves an active Tailwind")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TAILWIND, MOVE_CELEBRATE); }
        PLAYER(SPECIES_BRAVIARY_HISUIAN);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH_WITH_RNG(player, 1, WITH_RNG(RNG_ROGUE_TEMPORAL_SHIFT, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TEMPORAL_SHIFT);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}
