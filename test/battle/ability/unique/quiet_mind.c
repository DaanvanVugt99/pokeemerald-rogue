#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CALM_MIND].effect == EFFECT_CALM_MIND);
}

SINGLE_BATTLE_TEST("Quiet Mind uses Calm Mind when first dropping below 50 percent HP")
{
    GIVEN {
        PLAYER(SPECIES_DARMANITAN) { Ability(ABILITY_SHEER_FORCE); UniqueAbility(ABILITY_QUIET_MIND); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Quiet Mind triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DARMANITAN) { Ability(ABILITY_SHEER_FORCE); UniqueAbility(ABILITY_QUIET_MIND); Item(ITEM_SITRUS_BERRY); HP(100); MaxHP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Quiet Mind is the Darmanitan line's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DARUMAKA), ABILITY_QUIET_MIND);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DARMANITAN), ABILITY_QUIET_MIND);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DARMANITAN_ZEN_MODE), ABILITY_QUIET_MIND);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DARUMAKA_GALARIAN), ABILITY_QUIET_MIND);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DARMANITAN_GALARIAN), ABILITY_QUIET_MIND);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DARMANITAN_GALARIAN_ZEN_MODE), ABILITY_QUIET_MIND);
    }
}
