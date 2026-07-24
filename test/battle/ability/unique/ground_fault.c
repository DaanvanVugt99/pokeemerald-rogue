#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Ground Fault turns Electric moves into Water against Ground Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_BELLIBOLT) { Speed(100); SpAttack(100); UniqueAbility(ABILITY_GROUND_FAULT); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_GOLEM) { HP(500); MaxHP(500); Speed(1); SpDefense(100); Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GROUND_FAULT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOAK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Electric moves remain ineffective against Ground Pokemon without Ground Fault")
{
    GIVEN {
        PLAYER(SPECIES_BELLIBOLT) { Speed(100); SpAttack(100); UniqueAbility(ABILITY_FREESTYLE); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_GOLEM) { HP(500); MaxHP(500); Speed(1); SpDefense(100); Ability(ABILITY_STURDY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("It doesn't affect Foe Golem…");
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Ground Fault does not change Electric moves against non-Ground Pokemon", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_FREESTYLE; }
    PARAMETRIZE { uniqueAbility = ABILITY_GROUND_FAULT; }

    GIVEN {
        PLAYER(SPECIES_BELLIBOLT) { Speed(100); SpAttack(100); UniqueAbility(uniqueAbility); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(1); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_GROUND_FAULT);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
