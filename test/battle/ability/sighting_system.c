#include "global.h"
#include "test/battle.h"

// Test: Sighting System makes moves always hit
SINGLE_BATTLE_TEST("Sighting System makes moves always hit")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY); // Always hits
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_ZAP_CANNON].accuracy == 50);
        PLAYER(SPECIES_PORYGON_Z) { Ability(ABILITY_SIGHTING_SYSTEM); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_ZAP_CANNON); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, player);
        HP_BAR(opponent);
    }
}

// Test: Sighting System does not affect moves that already always hit
SINGLE_BATTLE_TEST("Sighting System does not affect always-hit moves")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY); // Always hits (no change)
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].accuracy == 0); // Move already never misses
        PLAYER(SPECIES_PORYGON_Z) { Ability(ABILITY_SIGHTING_SYSTEM); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_AERIAL_ACE); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
    }
}

// Test: Sighting System does not affect OHKO moves
SINGLE_BATTLE_TEST("Sighting System does not affect OHKO moves")
{
    PASSES_RANDOMLY(30, 100, RNG_ACCURACY); // Still 30% accuracy for OHKO moves
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_FISSURE].accuracy == 30);
        ASSUME(gBattleMoves[MOVE_FISSURE].effect == EFFECT_OHKO);
        PLAYER(SPECIES_PORYGON_Z) { Ability(ABILITY_SIGHTING_SYSTEM); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_FISSURE); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FISSURE, player);
        HP_BAR(opponent, hp : 0);
    }
}