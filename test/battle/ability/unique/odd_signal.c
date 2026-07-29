#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
    ASSUME(gBattleMoves[MOVE_WONDER_ROOM].effect == EFFECT_WONDER_ROOM);
    ASSUME(gBattleMoves[MOVE_MAGIC_ROOM].effect == EFFECT_MAGIC_ROOM);
}

SINGLE_BATTLE_TEST("Odd Signal uses Trick Room after a status move")
{
    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ABILITY_ODD_SIGNAL); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_ODD_SIGNAL, MOVE_TRICK_ROOM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_ODD_SIGNAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}

SINGLE_BATTLE_TEST("Odd Signal can choose Wonder Room after a status move")
{
    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ABILITY_ODD_SIGNAL); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_ODD_SIGNAL, MOVE_WONDER_ROOM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_ODD_SIGNAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WONDER_ROOM, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_WONDER_ROOM);
    }
}

SINGLE_BATTLE_TEST("Odd Signal can choose Magic Room after a status move")
{
    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ABILITY_ODD_SIGNAL); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_ODD_SIGNAL, MOVE_MAGIC_ROOM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_ODD_SIGNAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_ROOM, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MAGIC_ROOM);
    }
}

SINGLE_BATTLE_TEST("Odd Signal does not repeat the Room move that triggered it")
{
    static const u16 expectedMoves[] =
    {
        MOVE_WONDER_ROOM,
        MOVE_MAGIC_ROOM,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_ODD_SIGNAL);

    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Speed(100); Ability(ABILITY_TELEPATHY); UniqueAbility(ABILITY_ODD_SIGNAL); Moves(MOVE_TRICK_ROOM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK_ROOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ODD_SIGNAL);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
