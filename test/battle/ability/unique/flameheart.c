#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
    ASSUME(gItems[ITEM_WHITE_HERB].holdEffect == HOLD_EFFECT_RESTORE_STATS);
}

SINGLE_BATTLE_TEST("Flameheart burns the user on switch-in and shows burn status")
{
    GIVEN {
        PLAYER(SPECIES_MAGMAR) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_FLAMEHEART); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLAMEHEART);
        MESSAGE("Magmar was burned!");
        STATUS_ICON(player, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Flameheart continues switch-in effects after burning the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STICKY_WEB, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGMAR) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_FLAMEHEART); Item(ITEM_WHITE_HERB); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { SWITCH(opponent, 1); }
        TURN {}
    } SCENE {
        MESSAGE("Foe Magmar was caught in a Sticky Web!");
        ABILITY_POPUP(opponent, ABILITY_FLAMEHEART);
        MESSAGE("Foe Magmar was burned!");
        MESSAGE("Foe Magmar's White Herb restored its status!");
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Flameheart heals 1/8 max HP instead of taking burn damage")
{
    GIVEN {
        PLAYER(SPECIES_MAGMAR) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_FLAMEHEART); Status1(STATUS1_BURN); HP(1); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLAMEHEART);
        MESSAGE("Magmar's Flameheart restored its HP a little!");
        HP_BAR(player, damage: -50);
        NONE_OF {
            MESSAGE("Magmar is hurt by its burn!");
        }
    }
}

SINGLE_BATTLE_TEST("Flameheart re-applies burn after it is cured")
{
    GIVEN {
        PLAYER(SPECIES_MAGMAR) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_FLAMEHEART); Status1(STATUS1_BURN); Moves(MOVE_REFRESH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_REFRESH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Magmar was burned!");
        STATUS_ICON(player, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Flameheart does not re-apply burn if already burned")
{
    GIVEN {
        PLAYER(SPECIES_MAGMAR) { Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_FLAMEHEART); Status1(STATUS1_BURN); HP(1); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Magmar was burned!");
        }
        ABILITY_POPUP(player, ABILITY_FLAMEHEART);
        MESSAGE("Magmar's Flameheart restored its HP a little!");
        HP_BAR(player, damage: -50);
    }
}
