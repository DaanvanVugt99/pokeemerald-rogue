#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].strikeCount == 2);
    ASSUME(gBattleMoves[MOVE_FAKE_OUT].effect == EFFECT_FAKE_OUT);
    ASSUME(gBattleMoves[MOVE_FIRST_IMPRESSION].effect == EFFECT_FAKE_OUT);
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

SINGLE_BATTLE_TEST("Body of Water shows a popup when it reduces damage")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_BODY_OF_WATER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_THUNDERBOLT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDERBOLT); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BODY_OF_WATER);
        MESSAGE("Vaporeon's Body of Water softened the attack!");
    }
}

SINGLE_BATTLE_TEST("Body of Water only halves the first hit it takes")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(100); Ability(ABILITY_WATER_ABSORB); UniqueAbility(ABILITY_BODY_OF_WATER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DOUBLE_KICK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DOUBLE_KICK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, opponent);
        HP_BAR(player, captureDamage: &firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, opponent);
        HP_BAR(player, captureDamage: &secondHit);
    } THEN {
        EXPECT_MUL_EQ(firstHit, UQ_4_12(2.0), secondHit);
    }
}

SINGLE_BATTLE_TEST("Body of Water does not consume first-turn move eligibility")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Speed(1); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_BODY_OF_WATER); Moves(MOVE_FIRST_IMPRESSION); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FAKE_OUT); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRST_IMPRESSION); MOVE(opponent, MOVE_FAKE_OUT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        ABILITY_POPUP(player, ABILITY_BODY_OF_WATER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRST_IMPRESSION, player);
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
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(player, captureDamage: &firstSwitchInDamage);
        HP_BAR(player, captureDamage: &secondSwitchInDamage);
    } THEN {
        EXPECT_EQ(firstSwitchInDamage, secondSwitchInDamage);
    }
}
