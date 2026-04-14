#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Body of Water halves incoming damage on the first turn out", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_OPEN_FIELD; }
    PARAMETRIZE { uniqueAbility = ABILITY_BODY_OF_WATER; }

    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Body of Water only applies on the first turn out")
{
    s16 turnOneDamage;
    s16 turnTwoDamage;

    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_BODY_OF_WATER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(player, captureDamage: &turnOneDamage);
        HP_BAR(player, captureDamage: &turnTwoDamage);
    } THEN {
        EXPECT_MUL_EQ(turnOneDamage, UQ_4_12(2.0), turnTwoDamage);
    }
}

SINGLE_BATTLE_TEST("Body of Water refreshes after switching out and back in")
{
    s16 firstSwitchInDamage;
    s16 secondSwitchInDamage;

    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_BODY_OF_WATER); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_THUNDERBOLT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(player, captureDamage: &firstSwitchInDamage);
        HP_BAR(player, captureDamage: &secondSwitchInDamage);
    } THEN {
        EXPECT_EQ(firstSwitchInDamage, secondSwitchInDamage);
    }
}
