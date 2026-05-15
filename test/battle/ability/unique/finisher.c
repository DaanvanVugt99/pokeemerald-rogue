#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CRUNCH].power > 40);
}

SINGLE_BATTLE_TEST("Finisher uses 40 BP Crunch when a hit drops the target below quarter HP")
{
    s16 crunchHit;

    GIVEN {
        PLAYER(SPECIES_MABOSSTIFF) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SONIC_BOOM, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_FINISHER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
        HP_BAR(opponent, captureDamage: &crunchHit);
    } THEN {
        EXPECT_GT(crunchHit, 0);
        EXPECT_LT(crunchHit, gBattleMoves[MOVE_CRUNCH].power);
    }
}
