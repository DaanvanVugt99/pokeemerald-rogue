#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Freeze has a 25% chance of being thawed")
{
    PASSES_RANDOMLY(1, 4, RNG_FROZEN);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_FREEZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(player, none: TRUE);
    }
}

SINGLE_BATTLE_TEST("Freeze always thaws on the third attempted turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_FREEZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_FROZEN, FALSE)); }
    } SCENE {
        MESSAGE("Wobbuffet is frozen solid!");
        MESSAGE("Wobbuffet is frozen solid!");
        MESSAGE("Wobbuffet was defrosted!");
        STATUS_ICON(player, none: TRUE);
        MESSAGE("Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Freeze is thawed by opponent's Fire-type attacks")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_FREEZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EMBER); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Ember!");
        MESSAGE("Wobbuffet was defrosted!");
        STATUS_ICON(player, none: TRUE);
    }
}

SINGLE_BATTLE_TEST("Freeze is thawed by user's Flame Wheel")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_FLAME_WHEEL].thawsUser);
        PLAYER(SPECIES_WOBBUFFET) { Status1(STATUS1_FREEZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FLAME_WHEEL); }
    } SCENE {
        MESSAGE("Wobbuffet was defrosted by Flame Wheel!");
        STATUS_ICON(player, none: TRUE);
        MESSAGE("Wobbuffet used Flame Wheel!");
    }
}
