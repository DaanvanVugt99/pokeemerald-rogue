#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].power > 0);
}

SINGLE_BATTLE_TEST("Sacred Ash revives to half HP and cures status on first faint if all party Pokemon share a type")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { HP(10); MaxHP(10); Status1(STATUS1_POISON); Ability(ABILITY_PRESSURE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_FEAROW) { Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_ZAPDOS) { Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SACRED_ASH);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 5);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Sacred Ash only works once per battle")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { HP(10); MaxHP(10); Speed(100); Ability(ABILITY_PRESSURE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD) { Speed(90); Ability(ABILITY_BLAZE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_FEAROW) { Speed(80); Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_ZAPDOS) { Speed(50); Moves(MOVE_THUNDERBOLT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 0);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Sacred Ash does not trigger if another party Pokemon does not share Ho-Oh's type")
{
    GIVEN {
        PLAYER(SPECIES_HO_OH) { HP(10); MaxHP(10); Speed(100); Status1(STATUS1_POISON); Ability(ABILITY_PRESSURE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHARIZARD) { Speed(90); Ability(ABILITY_BLAZE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PIKACHU) { Speed(80); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_ZAPDOS) { Speed(50); Moves(MOVE_THUNDERBOLT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[0], MON_DATA_HP), 0);
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}
