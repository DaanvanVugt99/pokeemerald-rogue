#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_METRONOME].effect == EFFECT_METRONOME);
}

SINGLE_BATTLE_TEST("Chaos Theory uses Metronome after a move when all party types are unique")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { UniqueAbility(ABILITY_CHAOS_THEORY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD);
        PLAYER(SPECIES_PIKACHU);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_METRONOME, MOVE_SCRATCH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_CHAOS_THEORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Chaos Theory does not activate when party types overlap")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { UniqueAbility(ABILITY_CHAOS_THEORY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SLOWPOKE);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHAOS_THEORY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        }
    }
}

SINGLE_BATTLE_TEST("Chaos Theory does not recursively trigger from its Metronome move")
{
    GIVEN {
        PLAYER(SPECIES_MEW) { UniqueAbility(ABILITY_CHAOS_THEORY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD);
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_METRONOME, MOVE_SCRATCH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHAOS_THEORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHAOS_THEORY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        }
    }
}
