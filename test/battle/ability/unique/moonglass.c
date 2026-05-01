#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Moonglass heals 1/8 max HP after using a successful status move")
{
    GIVEN {
        PLAYER(SPECIES_CARBINK) { HP(80); MaxHP(160); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_MOONGLASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Moonglass does not heal after a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_CARBINK) { HP(80); MaxHP(160); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_MOONGLASS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 80);
    }
}
