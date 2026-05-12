#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Eclipse Core sets Eclipse after surviving a hit from full HP")
{
    GIVEN {
        PLAYER(SPECIES_LUNALA) { Ability(ABILITY_SHADOW_SHIELD); UniqueAbility(ABILITY_ECLIPSE_CORE); HP(300); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Eclipse Core does not trigger if the hit was not taken from full HP")
{
    GIVEN {
        PLAYER(SPECIES_LUNALA) { Ability(ABILITY_SHADOW_SHIELD); UniqueAbility(ABILITY_ECLIPSE_CORE); HP(250); MaxHP(300); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ECLIPSE));
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

DOUBLE_BATTLE_TEST("Eclipse Core triggers if healed to full before taking a hit")
{
    GIVEN {
        PLAYER(SPECIES_LUNALA) { Ability(ABILITY_SHADOW_SHIELD); UniqueAbility(ABILITY_ECLIPSE_CORE); HP(250); MaxHP(300); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HEAL_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_HEAL_PULSE, target: playerLeft);
            MOVE(opponentLeft, MOVE_BITE, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

DOUBLE_BATTLE_TEST("Eclipse Core does not trigger if the full-HP hit knocks this Pokemon out")
{
    GIVEN {
        PLAYER(SPECIES_LUNALA) { Ability(ABILITY_SHADOW_SHIELD); UniqueAbility(ABILITY_ECLIPSE_CORE); HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_BITE, target: playerLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ECLIPSE));
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

SINGLE_BATTLE_TEST("Eclipse Core only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_LUNALA) { Ability(ABILITY_SHADOW_SHIELD); UniqueAbility(ABILITY_ECLIPSE_CORE); HP(200); MaxHP(300); Moves(MOVE_CELEBRATE, MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_BITE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_BITE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
