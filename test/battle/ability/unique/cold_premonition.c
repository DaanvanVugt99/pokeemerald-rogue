#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Cold Premonition sets Future Sight after Freezing Glare")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); UniqueAbility(ABILITY_COLD_PREMONITION); Moves(MOVE_FREEZING_GLARE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FREEZING_GLARE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FREEZING_GLARE, player);
        ABILITY_POPUP(player, ABILITY_COLD_PREMONITION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
    } THEN {
        u32 target = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        EXPECT_EQ(gWishFutureKnock.futureSightMove[target], MOVE_FUTURE_SIGHT);
        EXPECT_EQ(gWishFutureKnock.futureSightAttacker[target], GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Cold Premonition does not replace a pending Future Sight")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); UniqueAbility(ABILITY_COLD_PREMONITION); Moves(MOVE_FUTURE_SIGHT, MOVE_FREEZING_GLARE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FUTURE_SIGHT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_FREEZING_GLARE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FREEZING_GLARE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COLD_PREMONITION);
        }
    }
}

SINGLE_BATTLE_TEST("Cold Premonition sets Future Sight after Freezing Glare knocks out its target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(500); UniqueAbility(ABILITY_COLD_PREMONITION); Moves(MOVE_FREEZING_GLARE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(1); SpDefense(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CHANSEY) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FREEZING_GLARE); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_COLD_PREMONITION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
    } THEN {
        u32 target = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        EXPECT_EQ(gWishFutureKnock.futureSightMove[target], MOVE_FUTURE_SIGHT);
    }
}

SINGLE_BATTLE_TEST("Cold Premonition does not trigger when Freezing Glare misses")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_COLD_PREMONITION); Moves(MOVE_CELEBRATE, MOVE_FREEZING_GLARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SAND_ATTACK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SAND_ATTACK); }
        TURN { MOVE(player, MOVE_FREEZING_GLARE, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COLD_PREMONITION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
        }
    }
}

SINGLE_BATTLE_TEST("Cold Premonition does not trigger after knocking out the final opposing Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(500); UniqueAbility(ABILITY_COLD_PREMONITION); Moves(MOVE_FREEZING_GLARE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(1); SpDefense(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FREEZING_GLARE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_COLD_PREMONITION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FUTURE_SIGHT, player);
        }
    } THEN {
        u32 target = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        EXPECT_EQ(gWishFutureKnock.futureSightMove[target], MOVE_NONE);
        EXPECT_EQ(gWishFutureKnock.futureSightCounter[target], 0);
    }
}
