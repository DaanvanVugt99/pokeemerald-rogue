#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
}

SINGLE_BATTLE_TEST("Ringleader sets Tailwind on switch-in with at least 2 other Dark-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_HONCHKROW) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_RINGLEADER); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_UMBREON) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_MIGHTYENA) { Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Ringleader does not set Tailwind on switch-in with fewer than 2 other Dark-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_HONCHKROW) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_RINGLEADER); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_UMBREON) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}

SINGLE_BATTLE_TEST("Ringleader torments the target on a critical hit")
{
    GIVEN {
        PLAYER(SPECIES_HONCHKROW) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_RINGLEADER); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK, criticalHit: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status2 & STATUS2_TORMENT);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tormentTimer, PERMANENT_TORMENT);
    }
}
