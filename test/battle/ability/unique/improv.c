#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_RECOVER));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(IS_MOVE_STATUS(MOVE_METRONOME));
}

SINGLE_BATTLE_TEST("Improv uses Metronome after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_MR_MIME) { HP(50); MaxHP(100); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_IMPROV); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ABILITY_POPUP(player, ABILITY_IMPROV);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
    }
}

SINGLE_BATTLE_TEST("Improv does not trigger after non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_MR_MIME) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_IMPROV); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_IMPROV);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        }
    }
}
