#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].effect == EFFECT_ALL_STATS_UP_HIT);
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].secondaryEffectChance == 10);
}

SINGLE_BATTLE_TEST("Living Fossil selects by listed power and breaks ties by moveslot order")
{
    u16 first, second, expected;
    PARAMETRIZE { first = MOVE_HEAD_SMASH; second = MOVE_FLAIL; expected = MOVE_FLAIL; }
    PARAMETRIZE { first = MOVE_AQUA_TAIL; second = MOVE_HEAD_SMASH; expected = MOVE_AQUA_TAIL; }
    PARAMETRIZE { first = MOVE_TACKLE; second = MOVE_WATER_GUN; expected = MOVE_TACKLE; }
    PARAMETRIZE { first = MOVE_WATER_GUN; second = MOVE_TACKLE; expected = MOVE_WATER_GUN; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power == gBattleMoves[MOVE_WATER_GUN].power);
        PLAYER(SPECIES_RELICANTH) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_LIVING_FOSSIL); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE, first, second); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVING_FOSSIL);
        ANIMATION(ANIM_TYPE_MOVE, expected, player);
    }
}

SINGLE_BATTLE_TEST("Living Fossil does nothing with only status moves")
{
    GIVEN {
        PLAYER(SPECIES_RELICANTH) { UniqueAbility(ABILITY_LIVING_FOSSIL); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE, MOVE_YAWN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF { ABILITY_POPUP(player, ABILITY_LIVING_FOSSIL); }
    }
}

SINGLE_BATTLE_TEST("Living Fossil triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_RELICANTH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_LIVING_FOSSIL); Item(ITEM_SITRUS_BERRY); HP(100); MaxHP(200); Moves(MOVE_CELEBRATE, MOVE_ANCIENT_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVING_FOSSIL);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LIVING_FOSSIL);
        }
    }
}

SINGLE_BATTLE_TEST("Living Fossil does not trigger if the user was already below 50 percent HP before the hit")
{
    GIVEN {
        PLAYER(SPECIES_RELICANTH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_LIVING_FOSSIL); HP(80); MaxHP(200); Moves(MOVE_CELEBRATE, MOVE_ANCIENT_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LIVING_FOSSIL);
        }
    }
}

SINGLE_BATTLE_TEST("Living Fossil does not trigger from confusion damage or a later hit while below half HP")
{
    s16 confusionDamage;

    PASSES_RANDOMLY(1, 3, RNG_CONFUSION);

    GIVEN {
        PLAYER(SPECIES_RELICANTH) {
            Ability(ABILITY_SWIFT_SWIM);
            UniqueAbility(ABILITY_LIVING_FOSSIL);
            HP(60);
            MaxHP(100);
            Attack(150);
            Defense(255);
            Speed(100);
            Moves(MOVE_CELEBRATE, MOVE_ANCIENT_POWER);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1); Speed(1); Moves(MOVE_CONFUSE_RAY, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CONFUSE_RAY); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("It hurt itself in its confusion!");
        HP_BAR(player, captureDamage: &confusionDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LIVING_FOSSIL);
        }
    } THEN {
        EXPECT_GT(confusionDamage, 10);
    }
}

SINGLE_BATTLE_TEST("Living Fossil's Ancient Power can grant the all-stats boost")
{
    PASSES_RANDOMLY(1, 10, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_RELICANTH) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_LIVING_FOSSIL); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE, MOVE_ANCIENT_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
