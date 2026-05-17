#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].type == TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Avalanche Hide halves only the first Fire hit each battle", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_OPEN_FIELD; }
    PARAMETRIZE { uniqueAbility = ABILITY_AVALANCHE_HIDE; }

    GIVEN {
        PLAYER(SPECIES_BAXCALIBUR) { HP(500); MaxHP(500); Speed(50); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FLAMETHROWER, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Avalanche Hide sets Snow after the first Fire hit")
{
    GIVEN {
        PLAYER(SPECIES_BAXCALIBUR) { HP(500); MaxHP(500); Speed(50); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FLAMETHROWER, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_AVALANCHE_HIDE);
        MESSAGE("It started to snow!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Avalanche Hide does not trigger on non-Fire damage")
{
    GIVEN {
        PLAYER(SPECIES_BAXCALIBUR) { HP(500); MaxHP(500); Speed(50); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_PULSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_PULSE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_AVALANCHE_HIDE);
            MESSAGE("It started to snow!");
        }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

SINGLE_BATTLE_TEST("Avalanche Hide does not set Snow if the first Fire hit KOs the user")
{
    GIVEN {
        PLAYER(SPECIES_BAXCALIBUR) { HP(1); MaxHP(500); Speed(50); SpDefense(1); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(255); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FLAMETHROWER, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player);
        MESSAGE("Baxcalibur fainted!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_AVALANCHE_HIDE);
            MESSAGE("It started to snow!");
        }
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
