#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("X-Ray Jaws boosts biting move damage by 1.2x", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_X_RAY_JAWS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("X-Ray Jaws bypasses Protect with biting moves")
{
    GIVEN {
        PLAYER(SPECIES_LUXRAY) { Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_X_RAY_JAWS); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}

SINGLE_BATTLE_TEST("X-Ray Jaws does not bypass Protect with non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_LUXRAY) { Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_X_RAY_JAWS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("X-Ray Jaws bypasses Substitute with biting moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_X_RAY_JAWS); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(400); HP(400); Defense(500); Moves(MOVE_SUBSTITUTE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_GT(opponent->hp, 0);
        EXPECT_LT(opponent->hp, opponent->maxHP - (opponent->maxHP / 4));
        EXPECT(opponent->status2 & STATUS2_SUBSTITUTE);
    }
}

SINGLE_BATTLE_TEST("X-Ray Jaws does not bypass Substitute with non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_X_RAY_JAWS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(400); HP(400); Defense(500); Moves(MOVE_SUBSTITUTE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUBSTITUTE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP - (opponent->maxHP / 4));
        EXPECT(opponent->status2 & STATUS2_SUBSTITUTE);
    }
}

SINGLE_BATTLE_TEST("X-Ray Jaws ignores positive Defense stages with biting moves", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_X_RAY_JAWS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_BITE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Defense(100); Moves(MOVE_IRON_DEFENSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 2);
    }
}
