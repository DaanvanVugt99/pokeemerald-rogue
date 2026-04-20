#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_POWER_GEM].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].effect == EFFECT_ALL_STATS_UP_HIT);
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].secondaryEffectChance == 10);
}

SINGLE_BATTLE_TEST("Primordial Wake makes the next Rock-type move also use Ancient Power", s16 damage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_TACKLE; }
    PARAMETRIZE { setupMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_OMASTAR) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_PRIMORDIAL_WAKE); Moves(MOVE_WATER_GUN, MOVE_TACKLE, MOVE_POWER_GEM); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POWER_GEM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 13 / 10);
    }
}

SINGLE_BATTLE_TEST("Primordial Wake is consumed after one Rock-type move")
{
    GIVEN {
        PLAYER(SPECIES_OMASTAR) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_PRIMORDIAL_WAKE); Moves(MOVE_WATER_GUN, MOVE_POWER_GEM); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POWER_GEM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POWER_GEM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_GEM, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ANCIENT_POWER, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POWER_GEM, player);
    }
}

SINGLE_BATTLE_TEST("Primordial Wake's Ancient Power can grant the all-stats boost")
{
    PASSES_RANDOMLY(1, 10, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_OMASTAR) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_PRIMORDIAL_WAKE); Moves(MOVE_WATER_GUN, MOVE_POWER_GEM); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POWER_GEM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
