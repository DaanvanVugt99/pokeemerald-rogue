#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GUST].type == TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Aerodynamic negates damage from Flying-type moves")
{
    GIVEN {
        PLAYER(SPECIES_YANMA) { Ability(ABILITY_SPEED_BOOST); UniqueAbility(ABILITY_AERODYNAMIC); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GUST); }
    } SCENE {
        NONE_OF { HP_BAR(player); }
    }
}

SINGLE_BATTLE_TEST("Aerodynamic increases Speed by one stage when hit by a Flying-type move")
{
    GIVEN {
        PLAYER(SPECIES_YANMA) { Ability(ABILITY_SPEED_BOOST); UniqueAbility(ABILITY_AERODYNAMIC); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_GUST); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AERODYNAMIC);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Yanma's Speed rose!");
    }
}

SINGLE_BATTLE_TEST("Aerodynamic does not increase Speed if already maxed")
{
    GIVEN {
        PLAYER(SPECIES_YANMA) { Ability(ABILITY_SPEED_BOOST); UniqueAbility(ABILITY_AERODYNAMIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_GUST); }
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_GUST); }
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_GUST); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_AERODYNAMIC);
        ABILITY_POPUP(player, ABILITY_AERODYNAMIC);
        ABILITY_POPUP(player, ABILITY_AERODYNAMIC);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
            MESSAGE("Yanma's Speed rose!");
        }
    }
}
