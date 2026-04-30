#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Pretty Petals halves damage taken after using a status move", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_PRETTY_PETALS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Pretty Petals does not halve damage after using a damaging move", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_PRETTY_PETALS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Pretty Petals is the unique ability for the Lilligant line")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_PETILIL), ABILITY_PRETTY_PETALS);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_LILLIGANT), ABILITY_PRETTY_PETALS);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_LILLIGANT_HISUIAN), ABILITY_PRETTY_PETALS);
    }
}
