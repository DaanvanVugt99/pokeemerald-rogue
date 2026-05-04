#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Royal Charm lowers Attack for opposite-gender opponents on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_ROYAL_CHARM); Gender(MON_FEMALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Royal Charm lowers Sp. Atk for same-gender opponents on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_ROYAL_CHARM); Gender(MON_FEMALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Gender(MON_FEMALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Royal Charm lowers Sp. Atk for genderless opponents on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_ROYAL_CHARM); Gender(MON_FEMALE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_STARYU) { Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Regal Pressure lowers the target's highest stat after hitting an opposite-gender target")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ABILITY_POISON_POINT); Gender(MON_MALE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BLISSEY) { HP(1000); MaxHP(1000); Ability(ABILITY_NATURAL_CURE); Gender(MON_FEMALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Regal Pressure does not trigger against same-gender targets")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ABILITY_POISON_POINT); Gender(MON_MALE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_SHADOW_TAG); Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Regal Pressure does not trigger against genderless targets")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ABILITY_POISON_POINT); Gender(MON_MALE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_STARYU) { HP(1000); MaxHP(1000); Ability(ABILITY_NATURAL_CURE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
