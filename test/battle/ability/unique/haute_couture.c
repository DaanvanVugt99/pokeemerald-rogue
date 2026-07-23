#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_BODY_SLAM].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Haute Couture changes Normal moves to the user's second type")
{
    GIVEN {
        PLAYER(SPECIES_FURFROU_STAR_TRIM) { Ability(ABILITY_FUR_COAT); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_GOLEM) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("It doesn't affect Foe Golem…");
        NONE_OF {
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Haute Couture boosts converted Normal move damage by 1.2x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_RAPID_REPLICA; }
    PARAMETRIZE { uniqueAbility = ABILITY_HAUTE_COUTURE; }

    GIVEN {
        PLAYER(SPECIES_FURFROU_NATURAL) { Attack(100); Ability(ABILITY_FUR_COAT); UniqueAbility(uniqueAbility); Moves(MOVE_BODY_SLAM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BODY_SLAM, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Haute Couture gives converted Normal moves +1 critical-hit ratio")
{
    PASSES_RANDOMLY(1, 8, RNG_CRITICAL_HIT);

    GIVEN {
        PLAYER(SPECIES_FURFROU_STAR_TRIM) { Ability(ABILITY_FUR_COAT); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Haute Couture takes precedence over standard Normal-type conversion abilities")
{
    GIVEN {
        PLAYER(SPECIES_FURFROU_KABUKI_TRIM) { Ability(ABILITY_PIXILATE); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_HOUNDOOM) { Ability(ABILITY_FLASH_FIRE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_FLASH_FIRE);
        NONE_OF {
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Haute Couture does not stack with matching standard Normal-type conversion abilities", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_FUR_COAT; }
    PARAMETRIZE { ability = ABILITY_PIXILATE; }

    GIVEN {
        PLAYER(SPECIES_FURFROU_HEART_TRIM) { Attack(100); Ability(ability); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Haute Couture does not boost a move overridden by Ion Deluge", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_RAPID_REPLICA; }
    PARAMETRIZE { uniqueAbility = ABILITY_HAUTE_COUTURE; }

    GIVEN {
        PLAYER(SPECIES_FURFROU_NATURAL) { Speed(1); Attack(100); Ability(ABILITY_FUR_COAT); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(400); MaxHP(400); Defense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_ION_DELUGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ION_DELUGE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Haute Couture does not raise the critical-hit ratio after Ion Deluge overrides it")
{
    PASSES_RANDOMLY(1, 24, RNG_CRITICAL_HIT);

    GIVEN {
        PLAYER(SPECIES_FURFROU_NATURAL) { Speed(1); Ability(ABILITY_FUR_COAT); UniqueAbility(ABILITY_HAUTE_COUTURE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ION_DELUGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ION_DELUGE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("A critical hit!");
    }
}
