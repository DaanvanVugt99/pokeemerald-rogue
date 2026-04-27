#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ATTACK_ORDER].highCritRatio == TRUE);
    ASSUME(gBattleMoves[MOVE_DEFEND_ORDER].effect == EFFECT_COSMIC_POWER);
    ASSUME(gBattleMoves[MOVE_HEAL_ORDER].effect == EFFECT_RESTORE_HP);
}

SINGLE_BATTLE_TEST("Hive Command raises Attack Order's critical-hit ratio by one extra stage")
{
    PASSES_RANDOMLY(1, 2, RNG_CRITICAL_HIT);
    GIVEN {
        PLAYER(SPECIES_VESPIQUEN) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_HIVE_COMMAND); Moves(MOVE_ATTACK_ORDER); }
        PLAYER(SPECIES_COMBEE);
        PLAYER(SPECIES_WURMPLE);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_ATTACK_ORDER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTACK_ORDER, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Hive Command makes Defend Order raise Sp. Def by two stages")
{
    GIVEN {
        PLAYER(SPECIES_VESPIQUEN) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_HIVE_COMMAND); Moves(MOVE_DEFEND_ORDER); }
        PLAYER(SPECIES_COMBEE);
        PLAYER(SPECIES_WURMPLE);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEFEND_ORDER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Hive Command does not improve Defend Order without two other Bug-type Pokémon")
{
    GIVEN {
        PLAYER(SPECIES_VESPIQUEN) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_HIVE_COMMAND); Moves(MOVE_DEFEND_ORDER); }
        PLAYER(SPECIES_COMBEE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DEFEND_ORDER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Hive Command makes Heal Order restore two thirds max HP")
{
    GIVEN {
        PLAYER(SPECIES_VESPIQUEN) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_HIVE_COMMAND); HP(100); MaxHP(300); Moves(MOVE_HEAL_ORDER); }
        PLAYER(SPECIES_COMBEE);
        PLAYER(SPECIES_WURMPLE);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HEAL_ORDER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, damage: -200);
    }
}

SINGLE_BATTLE_TEST("Hive Command does not improve Heal Order without two other Bug-type Pokémon")
{
    GIVEN {
        PLAYER(SPECIES_VESPIQUEN) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_HIVE_COMMAND); HP(100); MaxHP(300); Moves(MOVE_HEAL_ORDER); }
        PLAYER(SPECIES_COMBEE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HEAL_ORDER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, damage: -150);
    }
}
