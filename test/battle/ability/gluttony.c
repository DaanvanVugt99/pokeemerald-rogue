#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_ROUND].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Gluttony raises the highest offensive stat after eating a Berry")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { HP(100); MaxHP(100); Speed(100); Ability(ABILITY_GLUTTONY); Item(ITEM_SITRUS_BERRY); Moves(MOVE_ROUND, MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        MESSAGE("Abra's Sitrus Berry restored health!");
        ABILITY_POPUP(player, ABILITY_GLUTTONY);
        MESSAGE("Abra's Sp. Atk rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Gluttony raises the highest defensive stat after eating a Berry")
{
    GIVEN {
        PLAYER(SPECIES_ONIX) { HP(100); MaxHP(100); Speed(100); Ability(ABILITY_GLUTTONY); Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE, MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        MESSAGE("Onix's Sitrus Berry restored health!");
        ABILITY_POPUP(player, ABILITY_GLUTTONY);
        MESSAGE("Onix's Defense rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Gluttony can raise Speed if it is the highest stat after eating a Berry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Speed(200); Ability(ABILITY_GLUTTONY); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        MESSAGE("Wobbuffet's Sitrus Berry restored health!");
        ABILITY_POPUP(player, ABILITY_GLUTTONY);
        MESSAGE("Wobbuffet's Speed rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
