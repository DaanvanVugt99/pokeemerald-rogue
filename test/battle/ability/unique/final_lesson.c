#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SACRED_SWORD].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
    ASSUME(gBattleMoves[MOVE_LIGHT_SCREEN].effect == EFFECT_LIGHT_SCREEN);
    ASSUME(gBattleMoves[MOVE_SOAK].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SOAK].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Final Lesson lets the next Water move bypass Protect after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SURF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}

SINGLE_BATTLE_TEST("Final Lesson is consumed after one Water/Fighting move")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SURF); }
        TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SURF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Final Lesson is not re-primed by the Water status move it consumes")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_SOAK, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SOAK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SURF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOAK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Final Lesson uses resolved move type when bypassing Protect")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_WEATHER_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_RAIN_DANCE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WEATHER_BALL, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}

SINGLE_BATTLE_TEST("Final Lesson lets Fighting moves ignore Reflect", s16 damage)
{
    bool32 finalLesson;

    PARAMETRIZE { finalLesson = FALSE; }
    PARAMETRIZE { finalLesson = TRUE; }

    GIVEN {
        if (finalLesson)
            PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_SACRED_SWORD); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_SACRED_SWORD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); Speed(50); Moves(MOVE_REFLECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_REFLECT); }
        TURN { MOVE(player, MOVE_SACRED_SWORD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Final Lesson lets Water moves ignore Light Screen", s16 damage)
{
    bool32 finalLesson;

    PARAMETRIZE { finalLesson = FALSE; }
    PARAMETRIZE { finalLesson = TRUE; }

    GIVEN {
        if (finalLesson)
            PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_SURF); }
        else
            PLAYER(SPECIES_WOBBUFFET) { SpAttack(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Speed(50); Moves(MOVE_LIGHT_SCREEN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_LIGHT_SCREEN); }
        TURN { MOVE(player, MOVE_SURF); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Final Lesson does not activate without using a status move first")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_SURF); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SURF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, player);
            HP_BAR(opponent);
        }
    }
}

SINGLE_BATTLE_TEST("Final Lesson keeps the prime when using a non-Water/Fighting move")
{
    GIVEN {
        PLAYER(SPECIES_KELDEO) { Speed(100); Ability(ABILITY_JUSTIFIED); UniqueAbility(ABILITY_FINAL_LESSON); Moves(MOVE_CELEBRATE, MOVE_TACKLE, MOVE_SACRED_SWORD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SACRED_SWORD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SACRED_SWORD, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}
