#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_SUBSTITUTE].effect == EFFECT_SUBSTITUTE);
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
}

SINGLE_BATTLE_TEST("RKS Relay passes stat changes from a matching ally into Silvally")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_SILVALLY_WATER) { Item(ITEM_WATER_MEMORY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RKS_RELAY);
        MESSAGE("The RKS System transferred battle effects to Silvally!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("RKS Relay passes stat changes from Silvally into a matching ally")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_WATER) { Item(ITEM_WATER_MEMORY); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_VAPOREON) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RKS_RELAY);
        MESSAGE("The RKS System transferred battle effects to Vaporeon!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("RKS Relay works with switching moves")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(100); Moves(MOVE_SWORDS_DANCE, MOVE_U_TURN); }
        PLAYER(SPECIES_SILVALLY_WATER) { Speed(50); Item(ITEM_WATER_MEMORY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RKS_RELAY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("RKS Relay announces after an outgoing switch-transfer ability")
{
    GIVEN {
        PLAYER(SPECIES_TANGELA) { UniqueAbility(ABILITY_LIVING_ROOTS); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_SILVALLY_GRASS) { Item(ITEM_GRASS_MEMORY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVING_ROOTS);
        ABILITY_POPUP(player, ABILITY_RKS_RELAY);
        MESSAGE("The RKS System transferred battle effects to Silvally!");
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_ROOTED);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("RKS Relay passes Baton Pass volatile effects")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { MaxHP(100); Moves(MOVE_SUBSTITUTE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_SILVALLY_WATER) { Item(ITEM_WATER_MEMORY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RKS_RELAY);
    } THEN {
        EXPECT(player->status2 & STATUS2_SUBSTITUTE);
    }
}

SINGLE_BATTLE_TEST("RKS Relay does not pass effects to an ally that does not share the Memory type")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_WATER) { Item(ITEM_WATER_MEMORY); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_FLAREON) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RKS_RELAY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("RKS Relay requires Silvally to hold a Memory")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_NORMAL) { Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_EEVEE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RKS_RELAY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("RKS Relay does not pass effects while the ability is suppressed")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_SILVALLY_WATER) { Item(ITEM_WATER_MEMORY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KOFFING) { Ability(ABILITY_NEUTRALIZING_GAS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RKS_RELAY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("RKS Relay does not pass effects after Silvally faints")
{
    GIVEN {
        PLAYER(SPECIES_SILVALLY_WATER) { HP(1); Item(ITEM_WATER_MEMORY); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_VAPOREON) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE, MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RKS_RELAY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
