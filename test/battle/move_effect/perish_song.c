#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PERISH_SONG].effect == EFFECT_PERISH_SONG);
}

SINGLE_BATTLE_TEST("Perish Song is blocked by Soundproof")
{
    GIVEN {
        PLAYER(SPECIES_JIGGLYPUFF) { Moves(MOVE_PERISH_SONG, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WIGGLYTUFF);
        OPPONENT(SPECIES_VOLTORB) { Ability(ABILITY_SOUNDPROOF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ELECTRODE);
    } WHEN {
        TURN { MOVE(player, MOVE_PERISH_SONG); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PERISH_SONG, player);
        MESSAGE("Foe Voltorb's Soundproof blocks Perish Song!");
        MESSAGE("Jigglypuff fainted!");
        NONE_OF {
            MESSAGE("Foe Voltorb fainted!");
        }
    }
}

SINGLE_BATTLE_TEST("Unique Soundproof blocks Perish Song")
{
    GIVEN {
        PLAYER(SPECIES_JIGGLYPUFF) { Moves(MOVE_PERISH_SONG, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WIGGLYTUFF);
        OPPONENT(SPECIES_VOLTORB) { UniqueAbility(ABILITY_SOUNDPROOF); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ELECTRODE);
    } WHEN {
        TURN { MOVE(player, MOVE_PERISH_SONG); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PERISH_SONG, player);
        MESSAGE("Foe Voltorb's Soundproof blocks Perish Song!");
        MESSAGE("Jigglypuff fainted!");
        NONE_OF {
            MESSAGE("Foe Voltorb fainted!");
        }
    }
}
