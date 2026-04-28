#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_IMPRISON].effect == EFFECT_IMPRISON);
}

SINGLE_BATTLE_TEST("Lunar Edict uses Imprison on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CRESSELIA) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_LUNAR_EDICT); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_LUNAR_EDICT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IMPRISON, opponent);
    } THEN {
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_IMPRISONED_OTHERS);
    }
}

SINGLE_BATTLE_TEST("Lunar Edict is assigned to Cresselia")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_CRESSELIA), ABILITY_LUNAR_EDICT);
    }
}
