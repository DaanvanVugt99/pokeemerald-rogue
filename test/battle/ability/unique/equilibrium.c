#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_IRON_DEFENSE].effect == EFFECT_DEFENSE_UP_2);
    ASSUME(gBattleMoves[MOVE_AGILITY].effect == EFFECT_SPEED_UP_2);
}

SINGLE_BATTLE_TEST("Equilibrium ignores offensive stat changes while active", s16 damageBefore, s16 damageAfter)
{
    u32 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_EQUILIBRIUM; }
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE, MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damageBefore);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        HP_BAR(opponent, captureDamage: &results[i].damageAfter);
    } FINALLY {
        EXPECT_EQ(results[0].damageBefore, results[0].damageAfter);
        EXPECT_GT(results[1].damageAfter, results[1].damageBefore);
    }
}

SINGLE_BATTLE_TEST("Equilibrium ignores defensive stat changes while active", s16 damageBefore, s16 damageAfter)
{
    u32 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_EQUILIBRIUM; }
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(uniqueAbility); Moves(MOVE_IRON_DEFENSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_IRON_DEFENSE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_IRON_DEFENSE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damageBefore);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, opponent);
        HP_BAR(opponent, captureDamage: &results[i].damageAfter);
    } FINALLY {
        EXPECT_EQ(results[0].damageBefore, results[0].damageAfter);
        EXPECT_LT(results[1].damageAfter, results[1].damageBefore);
    }
}

SINGLE_BATTLE_TEST("Equilibrium ignores Speed stat changes for turn order")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); UniqueAbility(ABILITY_EQUILIBRIUM); Moves(MOVE_AGILITY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AGILITY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}
