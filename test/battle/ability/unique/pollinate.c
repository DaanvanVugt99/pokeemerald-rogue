#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(IS_MOVE_STATUS(MOVE_SWEET_SCENT));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Pollinate uses Sweet Scent after status moves")
{
    GIVEN {
        PLAYER(SPECIES_RIBOMBEE) { Ability(ABILITY_SHIELD_DUST); UniqueAbility(ABILITY_POLLINATE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_POLLINATE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWEET_SCENT, player);
        MESSAGE("Foe Wobbuffet's evasiveness harshly fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Pollinate does not trigger after damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_RIBOMBEE) { Ability(ABILITY_SHIELD_DUST); UniqueAbility(ABILITY_POLLINATE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_POLLINATE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWEET_SCENT, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Pollinate does not trigger after Sweet Scent")
{
    GIVEN {
        PLAYER(SPECIES_RIBOMBEE) { Ability(ABILITY_SHIELD_DUST); UniqueAbility(ABILITY_POLLINATE); Moves(MOVE_SWEET_SCENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWEET_SCENT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWEET_SCENT, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_POLLINATE);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_EVASION], DEFAULT_STAT_STAGE - 2);
    }
}
