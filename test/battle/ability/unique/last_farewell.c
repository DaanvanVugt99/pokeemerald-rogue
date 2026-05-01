#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Last Farewell sets Eclipse on switch-in after an ally fainted last turn")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_YVELTAL) { Ability(ABILITY_DARK_AURA); UniqueAbility(ABILITY_LAST_FAREWELL); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LAST_FAREWELL);
        MESSAGE("An eclipse blotted out the sun!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ECLIPSE_CONTINUES);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
    }
}

SINGLE_BATTLE_TEST("Last Farewell does not set Eclipse without an ally fainting last turn")
{
    GIVEN {
        PLAYER(SPECIES_WYNAUT) { HP(100); MaxHP(100); Speed(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_YVELTAL) { Ability(ABILITY_DARK_AURA); UniqueAbility(ABILITY_LAST_FAREWELL); Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_LAST_FAREWELL);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ECLIPSE));
    }
}
