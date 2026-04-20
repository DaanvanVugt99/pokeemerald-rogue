#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FIRE_PUNCH].makesContact);
    ASSUME(gBattleMoves[MOVE_FIRE_PUNCH].type == TYPE_FIRE);
}

SINGLE_BATTLE_TEST("Super Soft reduces contact damage to one quarter", s16 damage)
{
    bool32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = FALSE; }
    PARAMETRIZE { uniqueAbility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility ? ABILITY_COTTON_GUARDING : ABILITY_NONE); HP(300); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Wobbuffet used Tackle!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.25), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Super Soft quadruples damage from non-contact Fire-type moves", s16 damage)
{
    bool32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = FALSE; }
    PARAMETRIZE { uniqueAbility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility ? ABILITY_COTTON_GUARDING : ABILITY_NONE); HP(300); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        MESSAGE("Wobbuffet used Ember!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(4.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Super Soft does not alter damage of Fire-type contact moves", s16 damage)
{
    bool32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = FALSE; }
    PARAMETRIZE { uniqueAbility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility ? ABILITY_COTTON_GUARDING : ABILITY_NONE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_PUNCH); }
    } SCENE {
        MESSAGE("Wobbuffet used Fire Punch!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
