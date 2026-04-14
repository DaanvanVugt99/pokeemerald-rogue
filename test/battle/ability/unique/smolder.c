#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Smolder applies a burn-like physical damage penalty to opposing Pokemon", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_OPEN_FIELD; }
    PARAMETRIZE { uniqueAbility = ABILITY_SMOLDER; }

    GIVEN {
        PLAYER(SPECIES_FLAREON) { HP(400); MaxHP(400); Ability(ABILITY_FLASH_FIRE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); Moves(MOVE_STRENGTH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Smolder does not affect opposing special damage", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_IONIZE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SMOLDER; }

    GIVEN {
        PLAYER(SPECIES_FLAREON) { HP(400); MaxHP(400); Ability(ABILITY_FLASH_FIRE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ALAKAZAM) { Moves(MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}
