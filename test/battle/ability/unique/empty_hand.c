#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
    ASSUME(gBattleMoves[MOVE_MEDITATE].effect == EFFECT_ATTACK_UP);
}

SINGLE_BATTLE_TEST("Empty Hand triggers only on the first punching move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MEDICHAM) { Ability(ABILITY_PURE_POWER); UniqueAbility(ABILITY_EMPTY_HAND); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMPTY_HAND);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_EMPTY_HAND);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Empty Hand does not consume on non-punching moves")
{
    GIVEN {
        PLAYER(SPECIES_MEDICHAM) { Ability(ABILITY_PURE_POWER); UniqueAbility(ABILITY_EMPTY_HAND); Moves(MOVE_TACKLE, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMPTY_HAND);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Empty Hand refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_MEDICHAM) { Ability(ABILITY_PURE_POWER); UniqueAbility(ABILITY_EMPTY_HAND); Moves(MOVE_MEGA_PUNCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_EMPTY_HAND);
        ABILITY_POPUP(player, ABILITY_EMPTY_HAND);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
