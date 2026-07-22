#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_BONSLY].types[0] == TYPE_GRASS);
    ASSUME(gSpeciesInfo[SPECIES_BONSLY].types[1] == TYPE_GRASS);
    ASSUME(gSpeciesInfo[SPECIES_SUDOWOODO].types[0] == TYPE_GRASS);
    ASSUME(gSpeciesInfo[SPECIES_SUDOWOODO].types[1] == TYPE_GRASS);
}

SINGLE_BATTLE_TEST("Fauxliage changes the user to Rock-type and raises Attack and Defense after being hit")
{
    GIVEN {
        PLAYER(SPECIES_SUDOWOODO) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_FAUXLIAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAUXLIAGE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_ROCK);
        EXPECT_EQ(player->type2, TYPE_ROCK);
        EXPECT_EQ(player->type3, TYPE_MYSTERY);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Fauxliage triggers only once against a multi-hit move")
{
    GIVEN {
        PLAYER(SPECIES_SUDOWOODO) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_FAUXLIAGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_HITMONLEE) { Ability(ABILITY_SKILL_LINK); Moves(MOVE_DOUBLE_KICK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DOUBLE_KICK); }
    } THEN {
        EXPECT_EQ(player->type1, TYPE_ROCK);
        EXPECT_EQ(player->type2, TYPE_ROCK);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}
