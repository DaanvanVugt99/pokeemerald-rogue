#include "global.h"
#include "test/battle.h"

#include "charm_test.h"

SINGLE_BATTLE_TEST("charms: defense - Recoil Charm prevents damage-based recoil")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DOUBLE_EDGE].effect == EFFECT_RECOIL_33);
        SetSingleCharmForTest(ITEM_RECOIL_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_EDGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE); }
    } SCENE {
        HP_BAR(opponent);
        NONE_OF {
            HP_BAR(player);
            MESSAGE("Wobbuffet is hit with recoil!");
        }
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Recoil Charm prevents fixed max-HP recoil")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_STEEL_BEAM].effect == EFFECT_MAX_HP_50_RECOIL);
        SetSingleCharmForTest(ITEM_RECOIL_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_STEEL_BEAM); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_STEEL_BEAM); }
    } SCENE {
        HP_BAR(opponent);
        NOT HP_BAR(player);
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Recoil Charm prevents Struggle recoil")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_GIGATON_HAMMER].cantUseTwice);
        SetSingleCharmForTest(ITEM_RECOIL_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_GIGATON_HAMMER, MOVE_NONE, MOVE_NONE, MOVE_NONE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GIGATON_HAMMER); }
        TURN { FORCED_MOVE(player); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GIGATON_HAMMER, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, player);
        NOT HP_BAR(player);
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Recoil Charm prevents crash damage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_JUMP_KICK].effect == EFFECT_RECOIL_IF_MISS);
        SetSingleCharmForTest(ITEM_RECOIL_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_JUMP_KICK); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_JUMP_KICK, hit: FALSE); }
    } SCENE {
        MESSAGE("Wobbuffet's attack missed!");
        NONE_OF {
            MESSAGE("Wobbuffet kept going and crashed!");
            HP_BAR(player);
        }
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Recoil Charm does not protect opponents")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DOUBLE_EDGE].effect == EFFECT_RECOIL_33);
        SetSingleCharmForTest(ITEM_RECOIL_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_EDGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_EDGE); }
    } SCENE {
        HP_BAR(player);
        HP_BAR(opponent);
        MESSAGE("Foe Wobbuffet is hit with recoil!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Guard Charm blocks forced critical hits")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_GUARD_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, criticalHit: TRUE); }
    } SCENE {
        HP_BAR(player);
        NOT MESSAGE("A critical hit!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Guard Charm does not protect opponents")
{
    GIVEN {
        SetSingleCharmForTest(ITEM_GUARD_CHARM, 1);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, criticalHit: TRUE); }
    } SCENE {
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
    } THEN {
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - duplicate Recoil and Guard Charms clamp to one")
{
    GIVEN {
        BeginCharmTestRun();
        AddCharmForTest(ITEM_RECOIL_CHARM, 2);
        AddCharmForTest(ITEM_GUARD_CHARM, 2);
        FinishCharmTestSetup();
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetCharmValue(EFFECT_RECOIL_PROTECTION), 1);
        EXPECT_EQ(GetCharmValue(EFFECT_GUARD), 1);
        ClearCharmTestState();
    }
}
