#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Territorial intercepts a switch with a Flying attack at normal damage", s16 damage)
{
    bool32 switches;
    PARAMETRIZE { switches = FALSE; }
    PARAMETRIZE { switches = TRUE; }

    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(80); Attack(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(400); MaxHP(400); Defense(100); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); HP(400); MaxHP(400); Defense(200); }
    } WHEN {
        if (switches)
            TURN { SWITCH(opponent, 1); MOVE(player, MOVE_AERIAL_ACE); }
        else
            TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (switches)
            ABILITY_POPUP(player, ABILITY_TERRITORIAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (switches)
            MESSAGE("2 sent out Wynaut!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        }
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Territorial sets Tailwind after Flying KOs including intercepted switches")
{
    bool32 switches;
    PARAMETRIZE { switches = FALSE; }
    PARAMETRIZE { switches = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SPEAROW) { Speed(80); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(1); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); }
    } WHEN {
        if (switches)
            TURN { SWITCH(opponent, 1); MOVE(player, MOVE_AERIAL_ACE); SEND_OUT(opponent, 1); }
        else
            TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        ABILITY_POPUP(player, ABILITY_TERRITORIAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        MESSAGE("2 sent out Wynaut!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}

SINGLE_BATTLE_TEST("Territorial neither intercepts nor sets Tailwind with non-Flying attacks")
{
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(80); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Ability(ABILITY_TELEPATHY); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); HP(1); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 0); }
    } SCENE {
        MESSAGE("2 sent out Wynaut!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NOT ABILITY_POPUP(player, ABILITY_TERRITORIAL);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}

SINGLE_BATTLE_TEST("Territorial intercepts U-turn, Volt Switch, Flip Turn, and Parting Shot")
{
    u16 move;
    PARAMETRIZE { move = MOVE_U_TURN; }
    PARAMETRIZE { move = MOVE_VOLT_SWITCH; }
    PARAMETRIZE { move = MOVE_FLIP_TURN; }
    PARAMETRIZE { move = MOVE_PARTING_SHOT; }

    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(50); HP(500); MaxHP(500); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(500); MaxHP(500); Moves(move); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); }
    } WHEN {
        TURN { MOVE(opponent, move); MOVE(player, MOVE_AERIAL_ACE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, opponent);
        ABILITY_POPUP(player, ABILITY_TERRITORIAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
        MESSAGE("2 sent out Wynaut!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
    }
}

SINGLE_BATTLE_TEST("Territorial sets Tailwind when it knocks out a foe trying to pivot")
{
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(50); HP(500); MaxHP(500); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1); Moves(MOVE_U_TURN); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_U_TURN); MOVE(player, MOVE_AERIAL_ACE); SEND_OUT(opponent, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        MESSAGE("Foe Wobbuffet fainted!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        MESSAGE("2 sent out Wynaut!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}

DOUBLE_BATTLE_TEST("Territorial and Pursuit can intercept the same switch")
{
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(90); Attack(60); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_AERIAL_ACE); }
        PLAYER(SPECIES_PERSIAN) { Speed(70); Attack(60); Moves(MOVE_PURSUIT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); HP(500); MaxHP(500); Defense(150); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(30); }
    } WHEN {
        TURN {
            SWITCH(opponentLeft, 2);
            MOVE(playerLeft, MOVE_AERIAL_ACE, target: opponentLeft);
            MOVE(playerRight, MOVE_PURSUIT, target: opponentLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_TERRITORIAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, playerLeft);
        HP_BAR(opponentLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, playerRight);
        HP_BAR(opponentLeft);
        MESSAGE("2 sent out Bulbasaur!");
    } THEN {
        EXPECT_EQ(opponentLeft->species, SPECIES_BULBASAUR);
    }
}

DOUBLE_BATTLE_TEST("Territorial and Pursuit can intercept the pivot after Eject Button resolves")
{
    u16 intercept;
    PARAMETRIZE { intercept = MOVE_AERIAL_ACE; }
    PARAMETRIZE { intercept = MOVE_PURSUIT; }
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(50); UniqueAbility(ABILITY_TERRITORIAL); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Item(ITEM_EJECT_BUTTON); }
        PLAYER(SPECIES_WYNAUT) { Speed(30); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(500); MaxHP(500); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(10); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_U_TURN, target: playerRight);
            MOVE(playerLeft, intercept, target: opponentLeft);
            SEND_OUT(playerRight, 2);
            SEND_OUT(opponentLeft, 2);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponentLeft);
        MESSAGE("Go! Wynaut!");
        ANIMATION(ANIM_TYPE_MOVE, intercept, playerLeft);
        HP_BAR(opponentLeft);
        MESSAGE("2 sent out Bulbasaur!");
        NOT ANIMATION(ANIM_TYPE_MOVE, intercept, playerLeft);
    }
}
