#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGNET_RISE].effect == EFFECT_MAGNET_RISE);
}

SINGLE_BATTLE_TEST("Hive Mind uses Magnet Rise on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KADABRA) { Ability(ABILITY_SYNCHRONIZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_HIVE_MIND); }
        OPPONENT(SPECIES_MAGNETON) { Ability(ABILITY_STURDY); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_HIVE_MIND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, opponent);
    } THEN {
        EXPECT_NE(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_MAGNET_RISE, 0);
    }
}

SINGLE_BATTLE_TEST("Hive Mind does not use Magnet Rise on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KADABRA) { Ability(ABILITY_SYNCHRONIZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_HIVE_MIND); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_MAGNET_RISE, 0);
    }
}
