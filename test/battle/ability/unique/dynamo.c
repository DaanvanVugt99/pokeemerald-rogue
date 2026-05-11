#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CHARGE].type == TYPE_ELECTRIC);
    ASSUME(IS_MOVE_STATUS(MOVE_CHARGE));
}

SINGLE_BATTLE_TEST("Dynamo heals half HP when using Charge")
{
    GIVEN {
        PLAYER(SPECIES_VIKAVOLT) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DYNAMO); MaxHP(100); HP(40); Moves(MOVE_CHARGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CHARGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        ABILITY_POPUP(player, ABILITY_DYNAMO);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 90);
    }
}

SINGLE_BATTLE_TEST("Dynamo raises Speed when using Charge in Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_VIKAVOLT) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DYNAMO); Moves(MOVE_CHARGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ELECTRIC_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); MOVE(player, MOVE_CHARGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        ABILITY_POPUP(player, ABILITY_DYNAMO);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Dynamo does not trigger after non-Charge moves")
{
    GIVEN {
        PLAYER(SPECIES_VIKAVOLT) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_DYNAMO); MaxHP(100); HP(40); Moves(MOVE_THUNDER_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ELECTRIC_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); MOVE(player, MOVE_THUNDER_WAVE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DYNAMO);
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 40);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
