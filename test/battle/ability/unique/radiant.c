#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_HEAT_WAVE].type == TYPE_FIRE);
}

SINGLE_BATTLE_TEST("Radiant sets sun after a successful Fire-type move when the party shares a type")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_LARVESTA) { Speed(90); Ability(ABILITY_FLAME_BODY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RADIANT);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(!(gBattleWeather & B_WEATHER_SUN_PERMANENT));
    }
}

SINGLE_BATTLE_TEST("Radiant does not set sun if any party member does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_SQUIRTLE) { Speed(90); Ability(ABILITY_TORRENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RADIANT);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SUN));
    }
}

DOUBLE_BATTLE_TEST("Radiant still sets sun if a spread Fire move hits at least one target")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_HEAT_WAVE); }
        PLAYER(SPECIES_LARVESTA) { Speed(90); Ability(ABILITY_FLAME_BODY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(110); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HEAT_WAVE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_SPLASH); MOVE(opponentRight, MOVE_PROTECT); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_RADIANT);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}
