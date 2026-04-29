#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Dark Dimension sets Eclipse on switch-in if the party has 2 other Ghost-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MISDREAVUS) { Speed(100); Ability(ABILITY_LEVITATE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GIRATINA) { Speed(50); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); }
        OPPONENT(SPECIES_GASTLY) { Speed(40); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_DUSKULL) { Speed(30); Ability(ABILITY_LEVITATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_DARK_DIMENSION);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
    }
}

SINGLE_BATTLE_TEST("Dark Dimension does not set Eclipse on switch-in without 2 other Ghost-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Speed(100); Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GIRATINA) { Speed(50); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); }
        OPPONENT(SPECIES_GASTLY) { Speed(40); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(30); Ability(ABILITY_SWIFT_SWIM); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_DARK_DIMENSION);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ECLIPSE));
    }
}

SINGLE_BATTLE_TEST("Dark Dimension only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MISDREAVUS) { Speed(100); Ability(ABILITY_LEVITATE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GIRATINA) { Speed(50); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GASTLY) { Speed(40); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_DUSKULL) { Speed(30); Ability(ABILITY_LEVITATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 0); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_DARK_DIMENSION);
        NOT ABILITY_POPUP(opponent, ABILITY_DARK_DIMENSION);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_OPPONENT] & gBitTable[1]);
    }
}
