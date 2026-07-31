#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SKY_UPPERCUT].effect == EFFECT_SKY_UPPERCUT);
    ASSUME(gBattleMoves[MOVE_SKY_UPPERCUT].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_SKY_UPPERCUT].accuracy == 90);
    ASSUME(gBattleMoves[MOVE_SKY_UPPERCUT].damagesAirborne);
    ASSUME(gSpeciesInfo[SPECIES_PIDGEY].types[0] == TYPE_NORMAL || gSpeciesInfo[SPECIES_PIDGEY].types[1] == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_PIDGEY].types[0] == TYPE_FLYING || gSpeciesInfo[SPECIES_PIDGEY].types[1] == TYPE_FLYING);
    ASSUME(gSpeciesInfo[SPECIES_CHARMANDER].types[0] == TYPE_FIRE && gSpeciesInfo[SPECIES_CHARMANDER].types[1] == TYPE_FIRE);
    ASSUME(gSpeciesInfo[SPECIES_MACHOP].types[0] == TYPE_FIGHTING && gSpeciesInfo[SPECIES_MACHOP].types[1] == TYPE_FIGHTING);
}

SINGLE_BATTLE_TEST("Sky Uppercut is super effective against Flying-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_MACHOP) { Moves(MOVE_SKY_UPPERCUT); }
        OPPONENT(SPECIES_PIDGEY);
    } WHEN {
        TURN { MOVE(player, MOVE_SKY_UPPERCUT, hit: TRUE); }
    } SCENE {
        MESSAGE("Machop used Sky Uppercut!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_UPPERCUT, player);
        MESSAGE("It's extremely effective!");
    }
}

SINGLE_BATTLE_TEST("Sky Uppercut behaves normally against non-Flying Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_MACHOP) { Moves(MOVE_SKY_UPPERCUT); }
        OPPONENT(SPECIES_CHARMANDER);
    } WHEN {
        TURN { MOVE(player, MOVE_SKY_UPPERCUT, hit: TRUE); }
    } SCENE {
        MESSAGE("Machop used Sky Uppercut!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_UPPERCUT, player);
        NOT MESSAGE("It's super effective!");
    }
}
