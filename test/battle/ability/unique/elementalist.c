#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLAME_CHARGE].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_ICE_SHARD].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_SHOCK_WAVE].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Elementalist can burn with Fire-type moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAMPA) { Ability(ABILITY_BERSERK); UniqueAbility(ABILITY_ELEMENTALIST); Moves(MOVE_FLAME_CHARGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_CHARGE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Elementalist can inflict frostbite with Ice-type moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAMPA) { Ability(ABILITY_BERSERK); UniqueAbility(ABILITY_ELEMENTALIST); Moves(MOVE_ICE_SHARD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_SHARD, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(opponent->status1 & (STATUS1_FROSTBITE | STATUS1_FREEZE), 0);
    }
}

SINGLE_BATTLE_TEST("Elementalist can paralyze with Electric-type moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAMPA) { Ability(ABILITY_BERSERK); UniqueAbility(ABILITY_ELEMENTALIST); Moves(MOVE_SHOCK_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Elementalist does not inflict status when the 20% roll fails")
{
    GIVEN {
        PLAYER(SPECIES_DRAMPA) { Ability(ABILITY_BERSERK); UniqueAbility(ABILITY_ELEMENTALIST); Moves(MOVE_SHOCK_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHOCK_WAVE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
