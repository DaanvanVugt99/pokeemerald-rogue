#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LUNAR_BLESSING].effect == EFFECT_JUNGLE_HEALING);
}

SINGLE_BATTLE_TEST("Lunar Blessing recovers 1/4 of the user's max HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LUNAR_BLESSING); }
    } SCENE {
        HP_BAR(player, damage: -(400 / 4));
    }
}

SINGLE_BATTLE_TEST("Lunar Blessing recovers 1/2 of the user's max HP in Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); MOVE(player, MOVE_LUNAR_BLESSING); }
    } SCENE {
        HP_BAR(player, damage: -(400 / 2));
    }
}
