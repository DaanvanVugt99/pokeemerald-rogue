#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ENCORE].effect == EFFECT_ENCORE);
    ASSUME(gBattleMoves[MOVE_ENCORE].priority == 0);
    ASSUME(gBattleMoves[MOVE_DISABLE].effect == EFFECT_DISABLE);
    ASSUME(gBattleMoves[MOVE_DISABLE].priority == 0);
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
}

SINGLE_BATTLE_TEST("Lanakila Law gives Encore priority in Snow")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLAN) { HP(100); MaxHP(100); Speed(1); Ability(ABILITY_SNOW_WARNING); UniqueAbility(ABILITY_LANAKILA_LAW); Moves(MOVE_CELEBRATE, MOVE_ENCORE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ENCORE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(gDisableStructs[B_POSITION_OPPONENT_LEFT].encoredMove, MOVE_CELEBRATE);
    }
}

SINGLE_BATTLE_TEST("Lanakila Law gives Disable priority in Snow")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLAN) { HP(100); MaxHP(100); Speed(1); Ability(ABILITY_SNOW_WARNING); UniqueAbility(ABILITY_LANAKILA_LAW); Moves(MOVE_CELEBRATE, MOVE_DISABLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DISABLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Ninetales used Disable!");
        MESSAGE("Foe Wobbuffet's Celebrate was disabled!");
        MESSAGE("Foe Wobbuffet used Tackle!");
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ(gDisableStructs[B_POSITION_OPPONENT_LEFT].disabledMove, MOVE_CELEBRATE);
    }
}
