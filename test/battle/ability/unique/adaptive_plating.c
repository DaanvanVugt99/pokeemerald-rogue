#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Adaptive Plating raises Defense when the foe's Attack is higher")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GOLISOPOD) { UniqueAbility(ABILITY_ADAPTIVE_PLATING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(130); SpAttack(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_PLATING);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Adaptive Plating raises Sp. Def when the foe's Sp. Atk is higher")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GOLISOPOD) { UniqueAbility(ABILITY_ADAPTIVE_PLATING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(60); SpAttack(130); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_PLATING);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Adaptive Plating and Tough Claws coexist after Golisopite Mega Evolution")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_GOLISOPOD) { Item(ITEM_GOLISOPITE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(130); SpAttack(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, megaEvolve: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ADAPTIVE_PLATING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_GOLISOPOD_MEGA);
        EXPECT_EQ(player->ability, ABILITY_TOUGH_CLAWS);
        EXPECT_GT(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
