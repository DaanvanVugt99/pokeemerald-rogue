#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mortar Shell grants immunity to Rock-type moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
        PLAYER(SPECIES_TURTONATOR) { Ability(ABILITY_SHELL_ARMOR); UniqueAbility(ABILITY_MORTAR_SHELL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROCK_THROW); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROCK_THROW); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MORTAR_SHELL);
        NONE_OF { HP_BAR(player); }
    }
}

SINGLE_BATTLE_TEST("Mortar Shell raises Defense by one stage when hit by a Rock-type move")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
        PLAYER(SPECIES_TURTONATOR) { Ability(ABILITY_SHELL_ARMOR); UniqueAbility(ABILITY_MORTAR_SHELL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROCK_THROW); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROCK_THROW); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MORTAR_SHELL);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Turtonator's Defense rose!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Mortar Shell does not raise Defense when already at max")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
        PLAYER(SPECIES_TURTONATOR) { Ability(ABILITY_SHELL_ARMOR); UniqueAbility(ABILITY_MORTAR_SHELL); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_ROCK_THROW); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_ROCK_THROW); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_ROCK_THROW); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_ROCK_THROW); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], MAX_STAT_STAGE);
    }
}
