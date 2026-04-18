#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SLUDGE_BOMB].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
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
