#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_KOFFING].types[0] == TYPE_POISON || gSpeciesInfo[SPECIES_KOFFING].types[1] == TYPE_POISON);
    ASSUME(gSpeciesInfo[SPECIES_SCOLIPEDE].types[0] == TYPE_POISON || gSpeciesInfo[SPECIES_SCOLIPEDE].types[1] == TYPE_POISON);
}

SINGLE_BATTLE_TEST("Acid Conversion gives all damaging moves a 20% poison chance")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ACID_CONVERSION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Acid Conversion does not poison when the 20% roll fails")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_ACID_CONVERSION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Acid Conversion reduces incoming damage in Acid Rain", s16 damage)
{
    u16 species;
    PARAMETRIZE { species = SPECIES_KOFFING; }
    PARAMETRIZE { species = SPECIES_SCOLIPEDE; }

    GIVEN {
        PLAYER(species) { Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Moves(MOVE_ACID_RAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ACID_RAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.75), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Acid Conversion is the Scolipede line's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_VENIPEDE), ABILITY_ACID_CONVERSION);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_WHIRLIPEDE), ABILITY_ACID_CONVERSION);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SCOLIPEDE), ABILITY_ACID_CONVERSION);
    }
}
