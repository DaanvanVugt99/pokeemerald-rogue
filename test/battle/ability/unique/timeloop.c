#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Timeloop seeds the foe at end of turn if the party shares a type")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_TIMELOOP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_SLOWBRO) { Ability(ABILITY_OWN_TEMPO); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TIMELOOP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Timeloop does not seed if the party does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_TIMELOOP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}

DOUBLE_BATTLE_TEST("Timeloop chooses a random eligible opposing target")
{
    GIVEN {
        PLAYER(SPECIES_CELEBI) { Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_TIMELOOP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SLOWBRO) { Ability(ABILITY_OWN_TEMPO); Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE, WITH_RNG(RNG_TIMELOOP, 1));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_TIMELOOP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, playerLeft);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
        EXPECT(gStatuses3[B_POSITION_OPPONENT_RIGHT] & STATUS3_LEECHSEED);
    }
}
