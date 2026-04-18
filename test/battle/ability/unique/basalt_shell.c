#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SURF].power == 90);
}

SINGLE_BATTLE_TEST("Basalt Shell halves super-effective damage before the user has moved this turn")
{
    s16 turnOneDamage;
    s16 turnTwoDamage;

    GIVEN {
        PLAYER(SPECIES_MAGCARGO) { Speed(70); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_BASALT_SHELL); Moves(MOVE_ROCK_POLISH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SURF); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_POLISH); MOVE(opponent, MOVE_SURF); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SURF); }
    } SCENE {
        HP_BAR(player, captureDamage: &turnOneDamage);
        HP_BAR(player, captureDamage: &turnTwoDamage);
    } THEN {
        EXPECT_MUL_EQ(turnOneDamage, UQ_4_12(2.0), turnTwoDamage);
    }
}

SINGLE_BATTLE_TEST("Basalt Shell does not reduce neutral hits")
{
    s16 turnOneDamage;
    s16 turnTwoDamage;

    GIVEN {
        PLAYER(SPECIES_MAGCARGO) { Speed(1); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_BASALT_SHELL); Moves(MOVE_ROCK_POLISH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWIFT); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_POLISH); MOVE(opponent, MOVE_SWIFT); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &turnOneDamage);
        HP_BAR(player, captureDamage: &turnTwoDamage);
    } THEN {
        EXPECT_EQ(turnOneDamage, turnTwoDamage);
    }
}
