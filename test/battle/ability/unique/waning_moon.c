#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Waning Moon switches out after setting Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_UMBREON) { UniqueAbility(ABILITY_WANING_MOON); }
        PLAYER(SPECIES_EEVEE);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WANING_MOON);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_EEVEE);
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
    }
}

SINGLE_BATTLE_TEST("Waning Moon stays in when a foe sets Eclipse and when Eclipse is already active")
{
    GIVEN {
        PLAYER(SPECIES_UMBREON) { UniqueAbility(ABILITY_WANING_MOON); }
        PLAYER(SPECIES_EEVEE);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_ECLIPSE); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_UMBREON);
    }
}

SINGLE_BATTLE_TEST("Waning Moon reduces incoming move accuracy by 20 percent in Eclipse")
{
    PASSES_RANDOMLY(1, 5, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_UMBREON) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_WANING_MOON); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(opponent, MOVE_POUND); }
    } SCENE {
        NONE_OF {
            HP_BAR(player);
        }
    }
}
