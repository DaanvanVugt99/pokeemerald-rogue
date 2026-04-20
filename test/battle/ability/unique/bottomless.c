#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
}

SINGLE_BATTLE_TEST("Bottomless uses Stockpile at the end of each turn")
{
    GIVEN {
        PLAYER(SPECIES_GULPIN) { Ability(ABILITY_LIQUID_OOZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Bottomless cannot stockpile beyond 3")
{
    GIVEN {
        PLAYER(SPECIES_GULPIN) { Ability(ABILITY_LIQUID_OOZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 3);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 3);
    }
}

SINGLE_BATTLE_TEST("Bottomless still gains its stat boosts after the previous move had no effect")
{
    GIVEN {
        PLAYER(SPECIES_GULPIN) { Ability(ABILITY_LIQUID_OOZE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SABLEYE) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Bottomless does not suppress borrowed end-of-turn abilities")
{
    GIVEN {
        PLAYER(SPECIES_GULPIN) { HP(150); MaxHP(160); Ability(ABILITY_LIQUID_OOZE); UniqueAbility(ABILITY_BOTTOMLESS); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_LOTAD) { Ability(ABILITY_RAIN_DISH); Moves(MOVE_SKILL_SWAP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_SKILL_SWAP, target: playerLeft);
            MOVE(opponentLeft, MOVE_RAIN_DANCE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerLeft->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(playerLeft->hp, 160);
    }
}
