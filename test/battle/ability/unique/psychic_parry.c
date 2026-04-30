#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PSYCHO_CUT].slicingMove);
    ASSUME(!gBattleMoves[MOVE_CELEBRATE].slicingMove);
    ASSUME(gBattleMoves[MOVE_SWIFT].split != SPLIT_STATUS);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
}

SINGLE_BATTLE_TEST("Psychic Parry halves the next damaging move after using a slicing move", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_PSYCHIC_PARRY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_PSYCHO_CUT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SWIFT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHO_CUT); MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Psychic Parry is consumed by the next damaging move")
{
    s16 firstDamage;
    s16 secondDamage;

    GIVEN {
        PLAYER(SPECIES_GALLADE) { Speed(100); Ability(ABILITY_STEADFAST); UniqueAbility(ABILITY_PSYCHIC_PARRY); Moves(MOVE_PSYCHO_CUT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SWIFT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHO_CUT); MOVE(opponent, MOVE_SWIFT); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &firstDamage);
        HP_BAR(player, captureDamage: &secondDamage);
    } THEN {
        EXPECT_MUL_EQ(firstDamage, UQ_4_12(2.0), secondDamage);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Psychic Parry expires when the user moves again before being hit", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_PSYCHIC_PARRY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_PSYCHO_CUT, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SWIFT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHO_CUT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Psychic Parry lowers Attack when the reduced move makes contact")
{
    GIVEN {
        PLAYER(SPECIES_GALLADE) { Speed(100); Ability(ABILITY_STEADFAST); UniqueAbility(ABILITY_PSYCHIC_PARRY); Moves(MOVE_PSYCHO_CUT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHO_CUT); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PSYCHIC_PARRY);
        MESSAGE("Gallade's Psychic Parry readied a parry!");
        ABILITY_POPUP(player, ABILITY_PSYCHIC_PARRY);
        MESSAGE("Gallade's Psychic Parry parried the attack!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

