#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Pressure Shell reduces damage taken after using a Water-type move that turn", s16 damage)
{
    u16 move;
    PARAMETRIZE { move = MOVE_SPLASH; }
    PARAMETRIZE { move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_PRESSURE_SHELL); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Pressure Shell activation resets between turns", s16 damage)
{
    u16 turn2Move;
    PARAMETRIZE { turn2Move = MOVE_SPLASH; }
    PARAMETRIZE { turn2Move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_BLASTOISE) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_PRESSURE_SHELL); Moves(MOVE_WATER_GUN, turn2Move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, turn2Move); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
    }
}
