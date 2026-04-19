#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("King's Domain prevents loafing if the party shares a type")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Moves(MOVE_SONIC_BOOM, MOVE_SONIC_BOOM); }
        PLAYER(SPECIES_SLAKOTH) { Ability(ABILITY_TRUANT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 60);
    }
}

SINGLE_BATTLE_TEST("King's Domain does not prevent loafing if the party does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_SLAKING) { Ability(ABILITY_TRUANT); Moves(MOVE_SONIC_BOOM, MOVE_SONIC_BOOM); }
        PLAYER(SPECIES_ODDISH) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Slaking is loafing around!");
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
    }
}
