#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPORE].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_METRONOME].effect == EFFECT_METRONOME);
}

SINGLE_BATTLE_TEST("Fever Dream uses Metronome twice after putting a target to sleep")
{
    bool32 targetAlreadyAsleep;

    PARAMETRIZE { targetAlreadyAsleep = FALSE; }
    PARAMETRIZE { targetAlreadyAsleep = TRUE; }

    GIVEN {
        PLAYER(SPECIES_TOEDSCRUEL) { Ability(ABILITY_NO_GUARD); Moves(MOVE_SPORE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); if (targetAlreadyAsleep) Status1(STATUS1_SLEEP_TURN(2)); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE, WITH_RNG(RNG_METRONOME, MOVE_SCRATCH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (!targetAlreadyAsleep)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
            ABILITY_POPUP(player, ABILITY_FEVER_DREAM);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
            HP_BAR(opponent);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_FEVER_DREAM);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
            }
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}
