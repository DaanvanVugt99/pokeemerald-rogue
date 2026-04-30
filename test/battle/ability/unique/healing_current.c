#include "global.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Healing Current heals and cures the adjacent ally after the user uses a healing move")
{
    GIVEN {
        PLAYER(SPECIES_ALOMOMOLA)      { HP(40); MaxHP(100); Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_HEALING_CURRENT); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET)      { HP(60); MaxHP(160); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)    { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)    { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_RECOVER);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_HEALING_CURRENT);
    } THEN {
        EXPECT_EQ(playerRight->hp, 80);
        EXPECT_EQ(playerRight->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Healing Current does not trigger after a non-healing move")
{
    GIVEN {
        PLAYER(SPECIES_ALOMOMOLA)      { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_HEALING_CURRENT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET)      { HP(60); MaxHP(160); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)    { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)    { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_HEALING_CURRENT);
        }
    } THEN {
        EXPECT_EQ(playerRight->hp, 50);
        EXPECT_EQ(playerRight->status1, STATUS1_BURN);
    }
}
