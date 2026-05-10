#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Showdown sets Eclipse on switch-in if the team has no other Dark-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_INCINEROAR) { Speed(50); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SHOWDOWN); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Speed(40); Ability(ABILITY_SWIFT_SWIM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHOWDOWN);
        MESSAGE("An eclipse blotted out the sun!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ECLIPSE_CONTINUES);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
    }
}

SINGLE_BATTLE_TEST("Showdown does not set Eclipse on switch-in if the team has another Dark-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_INCINEROAR) { Speed(50); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SHOWDOWN); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_POOCHYENA) { Speed(40); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SHOWDOWN);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ECLIPSE));
    }
}
