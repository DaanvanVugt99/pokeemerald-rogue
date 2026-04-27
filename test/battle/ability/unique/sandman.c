#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Sandman can put the target to sleep with Ground moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SANDMAN); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_ROGUE_SANDMAN, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SANDMAN);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_SLP, opponent);
        STATUS_ICON(opponent, sleep: TRUE);
    }
}

SINGLE_BATTLE_TEST("Sandman can put the target to sleep with Rock moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SANDMAN); Moves(MOVE_ROCK_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW, WITH_RNG(RNG_ROGUE_SANDMAN, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Sandman does not affect non Ground or Rock moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SANDMAN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_SANDMAN, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Sandman only sleeps on its proc chance")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SANDMAN); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_ROGUE_SANDMAN, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
