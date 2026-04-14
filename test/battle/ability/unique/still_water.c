#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Still Water heals 1/8 max HP at end of turn if user took no damage")
{
    GIVEN {
        PLAYER(SPECIES_LAPRAS) { Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_STILL_WATER); HP(300); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STILL_WATER);
        MESSAGE("Lapras's Still Water restored its HP a little!");
        HP_BAR(player, damage: -50);
    }
}

SINGLE_BATTLE_TEST("Still Water does not heal if the user took damage that turn")
{
    GIVEN {
        PLAYER(SPECIES_LAPRAS) { Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_STILL_WATER); HP(300); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STILL_WATER);
            MESSAGE("Lapras's Still Water restored its HP a little!");
        }
    } THEN {
        EXPECT_LT(player->hp, 300);
    }
}
