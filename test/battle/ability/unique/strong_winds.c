#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WHIRLWIND].effect == EFFECT_ROAR);
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
}

SINGLE_BATTLE_TEST("Strong Winds sets Tailwind when Pidgeot forces out a foe with Whirlwind")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Moves(MOVE_WHIRLWIND); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_BULBASAUR);
    } WHEN {
        TURN { MOVE(player, MOVE_WHIRLWIND); }
    } SCENE {
        MESSAGE("Foe Bulbasaur was dragged out!");
        ABILITY_POPUP(player, ABILITY_STRONG_WINDS);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindBattlerId, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
        EXPECT_EQ(opponent->species, SPECIES_BULBASAUR);
    }
}

DOUBLE_BATTLE_TEST("Strong Winds sets Tailwind when an ally forces out a foe")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_WHIRLWIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_WHIRLWIND, target: opponentLeft); }
    } SCENE {
        MESSAGE("Foe Bulbasaur was dragged out!");
        ABILITY_POPUP(playerLeft, ABILITY_STRONG_WINDS);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindBattlerId, GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
        EXPECT_EQ(opponentLeft->species, SPECIES_BULBASAUR);
    }
}
