#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cotton Guarding raises Defense by 1 stage on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ALTARIA) { Ability(ABILITY_NATURAL_CURE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_COTTON_GUARDING);
        MESSAGE("Foe Altaria's Cotton Guarding raised its Defense!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}
