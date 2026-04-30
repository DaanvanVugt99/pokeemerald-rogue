#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_GEODUDE].types[0] == TYPE_ROCK || gSpeciesInfo[SPECIES_GEODUDE].types[1] == TYPE_ROCK);
}

SINGLE_BATTLE_TEST("Blood Scent removes resistance while any active Pokemon is below half HP", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_BLOOD_SCENT; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GEODUDE) { HP(200); MaxHP(500); Defense(100); Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Blood Scent does nothing when no active Pokemon is below half HP", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_BLOOD_SCENT; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); HP(100); MaxHP(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GEODUDE) { HP(500); MaxHP(500); Defense(100); Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
