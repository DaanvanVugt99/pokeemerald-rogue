#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
}

SINGLE_BATTLE_TEST("Solar Core sets sun on switch-in if no weather or terrain is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SOLGALEO) { Ability(ABILITY_FULL_METAL_BODY); UniqueAbility(ABILITY_SOLAR_CORE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SOLAR_CORE);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1]);
    }
}

SINGLE_BATTLE_TEST("Solar Core does not trigger or consume while any terrain is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SOLGALEO) { Ability(ABILITY_FULL_METAL_BODY); UniqueAbility(ABILITY_SOLAR_CORE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        NOT ABILITY_POPUP(player, ABILITY_SOLAR_CORE);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SUN));
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[1], 0);
    }
}
