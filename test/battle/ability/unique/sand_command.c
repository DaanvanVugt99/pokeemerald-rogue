#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sand Command sets Sandstorm after a KO with Flying and Ground allies")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_LANDORUS_INCARNATE; }
    PARAMETRIZE { species = SPECIES_LANDORUS_THERIAN; }

    GIVEN {
        PLAYER(species) { Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_PIDGEY);
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SAND_COMMAND);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
    }
}

SINGLE_BATTLE_TEST("Sand Command does not trigger without both Flying and Ground allies")
{
    bool32 hasFlyingAlly;
    bool32 hasGroundAlly;

    PARAMETRIZE { hasFlyingAlly = TRUE;  hasGroundAlly = TRUE; }
    PARAMETRIZE { hasFlyingAlly = TRUE;  hasGroundAlly = FALSE; }
    PARAMETRIZE { hasFlyingAlly = FALSE; hasGroundAlly = TRUE; }

    GIVEN {
        PLAYER(SPECIES_LANDORUS_INCARNATE) { Moves(MOVE_TACKLE); }
        PLAYER(hasFlyingAlly ? SPECIES_PIDGEY : SPECIES_CLEFAIRY);
        PLAYER(hasGroundAlly ? SPECIES_SANDSHREW : SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        if (hasFlyingAlly && hasGroundAlly)
            ABILITY_POPUP(player, ABILITY_SAND_COMMAND);
        else
            NOT ABILITY_POPUP(player, ABILITY_SAND_COMMAND);
    } THEN {
        if (hasFlyingAlly && hasGroundAlly)
            EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
        else
            EXPECT(!(gBattleWeather & B_WEATHER_SANDSTORM));
    }
}

SINGLE_BATTLE_TEST("Sand Command does not trigger if Landorus does not knock out the target")
{
    GIVEN {
        PLAYER(SPECIES_LANDORUS_INCARNATE) { Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_PIDGEY);
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SAND_COMMAND);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SANDSTORM));
    }
}
