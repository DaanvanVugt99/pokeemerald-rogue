#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
}

SINGLE_BATTLE_TEST("Insectivore grants immunity to Bug-type moves")
{
    GIVEN {
        PLAYER(SPECIES_HEATMOR) { HP(200); MaxHP(200); Ability(ABILITY_GLUTTONY); UniqueAbility(ABILITY_INSECTIVORE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WURMPLE) { Moves(MOVE_BUG_BITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BUG_BITE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INSECTIVORE);
    } THEN {
        EXPECT_EQ(player->hp, 200);
    }
}

SINGLE_BATTLE_TEST("Insectivore heals half HP after knocking out a Bug-type target")
{
    GIVEN {
        PLAYER(SPECIES_HEATMOR) { HP(100); MaxHP(300); Ability(ABILITY_GLUTTONY); UniqueAbility(ABILITY_INSECTIVORE); Moves(MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); MaxHP(300); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INSECTIVORE);
    } THEN {
        EXPECT_EQ(player->hp, 250);
    }
}
