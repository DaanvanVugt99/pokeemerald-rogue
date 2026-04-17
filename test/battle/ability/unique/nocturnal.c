#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy < 100);
}

DOUBLE_BATTLE_TEST("Nocturnal gives +1 priority when targeting a sleeping foe")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)   { Speed(50); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }

        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Status1(STATUS1_SLEEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Nocturnal does not give +1 priority when targeting an awake foe")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)   { Speed(50); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }

        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft);
            MOVE(playerRight, MOVE_TACKLE, target: opponentRight);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
    }
}

SINGLE_BATTLE_TEST("Nocturnal makes Hypnosis perfectly accurate during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)     { Speed(50); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_HYPNOSIS, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, 99)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}

DOUBLE_BATTLE_TEST("Nocturnal only gives priority against the sleeping target")
{
    GIVEN {
        PLAYER(SPECIES_NOCTOWL)   { Speed(50); Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_NOCTURNAL); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }

        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Status1(STATUS1_SLEEP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_TACKLE, target: opponentRight);
            MOVE(playerRight, MOVE_TACKLE, target: opponentLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
    }
}
