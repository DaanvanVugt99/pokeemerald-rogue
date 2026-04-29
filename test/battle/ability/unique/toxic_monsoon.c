#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SLUDGE_BOMB].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gSpeciesInfo[SPECIES_MAGNEMITE].types[0] == TYPE_ELECTRIC || gSpeciesInfo[SPECIES_MAGNEMITE].types[1] == TYPE_ELECTRIC);
    ASSUME(gSpeciesInfo[SPECIES_MAGNEMITE].types[0] == TYPE_STEEL || gSpeciesInfo[SPECIES_MAGNEMITE].types[1] == TYPE_STEEL);
}

SINGLE_BATTLE_TEST("Toxic Monsoon sets Acid Rain after using a Poison-type move")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_TOXIC_MONSOON); Moves(MOVE_SLUDGE_BOMB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLUDGE_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Toxic Monsoon does not set Acid Rain if the Poison move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_TOXIC_MONSOON); Moves(MOVE_SLUDGE_BOMB); }
        OPPONENT(SPECIES_MAGNEMITE) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLUDGE_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TOXIC_MONSOON);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ACID_RAIN));
    }
}

SINGLE_BATTLE_TEST("Toxic Monsoon poisons contact attackers during Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_TOXIC_MONSOON); Moves(MOVE_SLUDGE_BOMB, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLUDGE_BOMB); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_MONSOON);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Toxic Monsoon does not poison contact attackers outside Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_QWILFISH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_TOXIC_MONSOON); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT(!(opponent->status1 & STATUS1_POISON));
    }
}
