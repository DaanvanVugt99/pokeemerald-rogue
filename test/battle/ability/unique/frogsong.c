#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SING].effect == EFFECT_SLEEP);
}

SINGLE_BATTLE_TEST("Frogsong has a 30% chance to use Sing after being hit")
{
    GIVEN {
        PLAYER(SPECIES_SEISMITOAD) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_FROGSONG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_FROGSONG, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_FROGSONG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SING, player);
    }
}

SINGLE_BATTLE_TEST("Frogsong does not trigger outside rain when the 30% roll fails")
{
    GIVEN {
        PLAYER(SPECIES_SEISMITOAD) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_FROGSONG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_FROGSONG, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FROGSONG);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SING, player);
        }
    }
}

SINGLE_BATTLE_TEST("Frogsong always uses Sing in rain")
{
    GIVEN {
        PLAYER(SPECIES_SEISMITOAD) { Speed(100); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_FROGSONG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_RAIN_DANCE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_FROGSONG, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_FROGSONG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SING, player);
    }
}
