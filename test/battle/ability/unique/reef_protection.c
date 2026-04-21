#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Reef Protection sets a layer of Spikes the first time the user restores HP in a turn")
{
    GIVEN {
        PLAYER(SPECIES_CORSOLA) { Speed(1); HP(60); MaxHP(100); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_REEF_PROTECTION); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKES, player);
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

SINGLE_BATTLE_TEST("Reef Protection only sets Spikes once even if the user heals twice in the same turn")
{
    GIVEN {
        PLAYER(SPECIES_CORSOLA) { Speed(1); HP(100); MaxHP(100); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_REEF_PROTECTION); Moves(MOVE_ABSORB, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GRASSY_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(player, MOVE_ABSORB); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
    }
}

DOUBLE_BATTLE_TEST("Reef Protection does not stop the healer's move-end ability from triggering")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM)    { Status1(STATUS1_BURN); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_VERDANT_HAVEN); Moves(MOVE_CELEBRATE, MOVE_HEAL_PULSE); }
        PLAYER(SPECIES_CORSOLA)     { HP(50); MaxHP(100); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_REEF_PROTECTION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_HEAL_PULSE, target: playerRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(gSideTimers[B_SIDE_OPPONENT].spikesAmount, 1);
        EXPECT_EQ(playerLeft->status1, STATUS1_NONE);
    }
}
