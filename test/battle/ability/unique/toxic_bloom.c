#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Toxic Bloom poisons the foe at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_VILEPLUME) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_TOXIC_BLOOM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

DOUBLE_BATTLE_TEST("Toxic Bloom poisons both foes at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_VILEPLUME) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_TOXIC_BLOOM); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT(opponentLeft->status1 & STATUS1_POISON);
        EXPECT(opponentRight->status1 & STATUS1_POISON);
    }
}
