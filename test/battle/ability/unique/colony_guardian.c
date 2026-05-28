#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HONE_CLAWS].effect == EFFECT_ATTACK_ACCURACY_UP);
}

SINGLE_BATTLE_TEST("Colony Guardian uses Hone Claws when a foe lowers this Pokemon's stats")
{
    GIVEN {
        PLAYER(SPECIES_DURANT) { Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_COLONY_GUARDIAN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Colony Guardian uses Hone Claws after switch-in Intimidate lowers this Pokemon's Attack")
{
    GIVEN {
        PLAYER(SPECIES_DURANT) { Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_COLONY_GUARDIAN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KROOKODILE) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Colony Guardian does not trigger when stats are not lowered")
{
    GIVEN {
        PLAYER(SPECIES_DURANT) { Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_COLONY_GUARDIAN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_COLONY_GUARDIAN);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Colony Guardian does not trigger from self-inflicted stat drops")
{
    GIVEN {
        PLAYER(SPECIES_DURANT) { Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_COLONY_GUARDIAN); Moves(MOVE_OVERHEAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_OVERHEAT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_COLONY_GUARDIAN);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}
