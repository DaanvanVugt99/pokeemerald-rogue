#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
    ASSUME(gBattleMoves[MOVE_SILVER_WIND].windMove);
    ASSUME(gBattleMoves[MOVE_WING_ATTACK].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Soaring Gale boosts wind move damage by 1.3x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SOARING_GALE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_SILVER_WIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_SILVER_WIND); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Soaring Gale boosts wing move damage by 1.3x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SOARING_GALE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Soaring Gale does not boost other moves", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SOARING_GALE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Soaring Gale makes Tailwind last 6 turns")
{
    GIVEN {
        ASSUME(B_TAILWIND_TURNS >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_SOARING_GALE); Moves(MOVE_TAILWIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(15); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_TAILWIND); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 5);
    }
}

SINGLE_BATTLE_TEST("Soaring Gale does not change Tailwind duration without the ability")
{
    GIVEN {
        ASSUME(B_TAILWIND_TURNS >= GEN_5);
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TAILWIND); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(15); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_TAILWIND); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Soaring Gale requested moves have the wind move flag")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleMoves[MOVE_RAZOR_WIND].windMove);
        EXPECT(gBattleMoves[MOVE_SILVER_WIND].windMove);
        EXPECT(gBattleMoves[MOVE_OMINOUS_WIND].windMove);
        EXPECT(gBattleMoves[MOVE_DEFOG].windMove);
        EXPECT(gBattleMoves[MOVE_AIR_SLASH].windMove);
        EXPECT(gBattleMoves[MOVE_LEAF_TORNADO].windMove);
    }
}

SINGLE_BATTLE_TEST("Soaring Gale is assigned to the Unfezant line")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_PIDOVE), ABILITY_SOARING_GALE);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_TRANQUILL), ABILITY_SOARING_GALE);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_UNFEZANT), ABILITY_SOARING_GALE);
    }
}
