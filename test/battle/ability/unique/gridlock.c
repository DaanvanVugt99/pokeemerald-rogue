#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > 0);
    ASSUME(gBattleMoves[MOVE_BITE].priority == 0);
}

SINGLE_BATTLE_TEST("Gridlock blocks priority moves in Electric Terrain")
{
    bool32 terrain;

    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_REGIELEKI) { Speed(100); Ability(ABILITY_TRANSISTOR); Moves(MOVE_CELEBRATE, MOVE_ELECTRIC_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_QUICK_ATTACK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, terrain ? MOVE_ELECTRIC_TERRAIN : MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        if (terrain)
        {
            ABILITY_POPUP(player, ABILITY_GRIDLOCK);
            NOT { HP_BAR(player); }
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
            HP_BAR(player);
        }
    } THEN {
        if (terrain)
            EXPECT_EQ(player->hp, player->maxHP);
        else
            EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Gridlock does not block non-priority moves in Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_REGIELEKI) { Speed(100); Ability(ABILITY_TRANSISTOR); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_BITE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Gridlock is ignored by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_REGIELEKI) { Speed(100); Ability(ABILITY_TRANSISTOR); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PINSIR) { Speed(50); Ability(ABILITY_MOLD_BREAKER); Moves(MOVE_QUICK_ATTACK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MOLD_BREAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        HP_BAR(player);
        NOT { ABILITY_POPUP(player, ABILITY_GRIDLOCK); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
