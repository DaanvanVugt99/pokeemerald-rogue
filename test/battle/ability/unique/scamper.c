#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Scamper uses Substitute on switch-in at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FURRET) { Ability(ABILITY_RUN_AWAY); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->hp < opponent->maxHP);
        EXPECT(opponent->status2 & STATUS2_SUBSTITUTE);
    }
}

SINGLE_BATTLE_TEST("Scamper does not use Substitute on switch-in if not at full HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FURRET) { Ability(ABILITY_RUN_AWAY); HP(50); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_SUBSTITUTE));
    }
}

SINGLE_BATTLE_TEST("Scamper does not use Substitute if switch-in damage breaks full HP before it can activate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_FURRET) { Ability(ABILITY_RUN_AWAY); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->hp < opponent->maxHP);
        EXPECT(!(opponent->status2 & STATUS2_SUBSTITUTE));
    }
}
