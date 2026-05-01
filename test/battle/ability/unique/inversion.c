#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Inversion uses Topsy-Turvy after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_MALAMAR) { Speed(50); Ability(ABILITY_CONTRARY); UniqueAbility(ABILITY_INVERSION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_HOWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_HOWL); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Inversion does not trigger after non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_MALAMAR) { Ability(ABILITY_CONTRARY); UniqueAbility(ABILITY_INVERSION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HOWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_HOWL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_INVERSION);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
