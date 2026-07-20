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

SINGLE_BATTLE_TEST("charms: defense - Eviolite Charm boosts physical and special defense", s16 damage)
{
    bool32 hasCharm;
    u32 move;

    PARAMETRIZE { hasCharm = FALSE; move = MOVE_TACKLE; }
    PARAMETRIZE { hasCharm = TRUE;  move = MOVE_TACKLE; }
    PARAMETRIZE { hasCharm = FALSE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { hasCharm = TRUE;  move = MOVE_WATER_GUN; }

    GIVEN {
        SetSingleCharmForTest(ITEM_EVIOLITE_CHARM, hasCharm);
        PLAYER(SPECIES_MAREEP) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.5), results[0].damage);
        EXPECT_MUL_EQ(results[3].damage, UQ_4_12(1.5), results[2].damage);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Eviolite Charm applies to middle evolutionary stages", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        SetSingleCharmForTest(ITEM_EVIOLITE_CHARM, hasCharm);
        PLAYER(SPECIES_FLAAFFY) { Defense(120); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.5), results[0].damage);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Eviolite Charm does not apply to final evolutions", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        SetSingleCharmForTest(ITEM_EVIOLITE_CHARM, hasCharm);
        PLAYER(SPECIES_AMPHAROS) { Defense(120); HP(1000); MaxHP(1000); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Eviolite Charm stacks with a held Eviolite", s16 damage)
{
    bool32 hasCharm;
    u32 item;

    PARAMETRIZE { hasCharm = FALSE; item = ITEM_NONE; }
    PARAMETRIZE { hasCharm = TRUE;  item = ITEM_NONE; }
    PARAMETRIZE { hasCharm = FALSE; item = ITEM_EVIOLITE; }
    PARAMETRIZE { hasCharm = TRUE;  item = ITEM_EVIOLITE; }

    GIVEN {
        SetSingleCharmForTest(ITEM_EVIOLITE_CHARM, hasCharm);
        PLAYER(SPECIES_MAREEP) { Defense(120); HP(1000); MaxHP(1000); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.5), results[0].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.5), results[0].damage);
        EXPECT_MUL_EQ(results[3].damage, UQ_4_12(2.25), results[0].damage);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - Eviolite Charm does not protect opponents", s16 damage)
{
    bool32 hasCharm;

    PARAMETRIZE { hasCharm = FALSE; }
    PARAMETRIZE { hasCharm = TRUE; }

    GIVEN {
        SetSingleCharmForTest(ITEM_EVIOLITE_CHARM, hasCharm);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_MAREEP) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearCharmTestState();
    }
}

SINGLE_BATTLE_TEST("charms: defense - item suppression leaves Eviolite Charm active", s16 damage)
{
    u32 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_EVIOLITE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_MAGIC_ROOM].effect == EFFECT_MAGIC_ROOM);
        SetSingleCharmForTest(ITEM_EVIOLITE_CHARM, 1);
        PLAYER(SPECIES_MAREEP) { Speed(100); Defense(120); HP(1000); MaxHP(1000); Item(item); Moves(MOVE_MAGIC_ROOM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGIC_ROOM); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        ClearCharmTestState();
    }
}
