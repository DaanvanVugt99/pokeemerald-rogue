#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(!IS_MOVE_STATUS(MOVE_WATER_GUN));
    ASSUME(!IS_MOVE_STATUS(MOVE_FLAMETHROWER));
    ASSUME(!IS_MOVE_STATUS(MOVE_ICE_BEAM));
    ASSUME(!IS_MOVE_STATUS(MOVE_PSYBEAM));
    ASSUME(!IS_MOVE_STATUS(MOVE_SURF));
    ASSUME(!IS_MOVE_STATUS(MOVE_WEATHER_BALL));
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
}

SINGLE_BATTLE_TEST("Adaptive Armory changes the user's type when it has four attacking moves of different types")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Ability(ABILITY_SNIPER); Moves(MOVE_WATER_GUN, MOVE_FLAMETHROWER, MOVE_ICE_BEAM, MOVE_PSYBEAM); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_ARMORY);
        MESSAGE("Octillery transformed into the Fire type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAMETHROWER, player);
    }
}

SINGLE_BATTLE_TEST("Adaptive Armory does not change the user's type when attacking move types are not all different")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Ability(ABILITY_SNIPER); Moves(MOVE_WATER_GUN, MOVE_SURF, MOVE_ICE_BEAM, MOVE_FLAMETHROWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ADAPTIVE_ARMORY);
            MESSAGE("Octillery transformed into the Fire type!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAMETHROWER, player);
    }
}

SINGLE_BATTLE_TEST("Adaptive Armory does not change type when a dynamic move duplicates another move's battle type")
{
    GIVEN {
        PLAYER(SPECIES_OCTILLERY) { Ability(ABILITY_SNIPER); Moves(MOVE_WATER_GUN, MOVE_ICE_BEAM, MOVE_FLAMETHROWER, MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_TORKOAL) { Ability(ABILITY_DROUGHT); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_BEAM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ADAPTIVE_ARMORY);
            MESSAGE("Octillery transformed into the Ice type!");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_BEAM, player);
    }
}
