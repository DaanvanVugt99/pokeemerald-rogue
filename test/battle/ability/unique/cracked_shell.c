#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERSHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Cracked Shell uses Shell Smash after being hit by a super-effective move")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Ability(ABILITY_SHELL_ARMOR); UniqueAbility(ABILITY_CRACKED_SHELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Moves(MOVE_THUNDERSHOCK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERSHOCK); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CRACKED_SHELL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELL_SMASH, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Cracked Shell triggers only once against a multi-hit super-effective move")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Ability(ABILITY_SHELL_ARMOR); UniqueAbility(ABILITY_CRACKED_SHELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_HITMONLEE) { Ability(ABILITY_SKILL_LINK); Moves(MOVE_DOUBLE_KICK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DOUBLE_KICK); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Cracked Shell does not trigger on neutral hits")
{
    GIVEN {
        PLAYER(SPECIES_CLOYSTER) { Ability(ABILITY_SHELL_ARMOR); UniqueAbility(ABILITY_CRACKED_SHELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
