#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SUNNY_DAY].effect == EFFECT_SUNNY_DAY);
    ASSUME(gBattleMoves[MOVE_FLOWER_SHIELD].effect == EFFECT_FLOWER_SHIELD);
}

SINGLE_BATTLE_TEST("Bloom Burst uses Flower Shield on switch-in during sun")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY); }
        PLAYER(SPECIES_CHERUBI) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_BLOOM_BURST); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUNNY_DAY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, player);
        ABILITY_POPUP(player, ABILITY_BLOOM_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Bloom Burst uses Flower Shield when sun starts while active")
{
    GIVEN {
        PLAYER(SPECIES_CHERUBI) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_BLOOM_BURST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, opponent);
        ABILITY_POPUP(player, ABILITY_BLOOM_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Bloom Burst works with Cherrim's Flower Gift when sun starts")
{
    GIVEN {
        PLAYER(SPECIES_CHERRIM_OVERCAST) { Ability(ABILITY_FLOWER_GIFT); UniqueAbility(ABILITY_BLOOM_BURST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, opponent);
        ABILITY_POPUP(player, ABILITY_FLOWER_GIFT);
        ABILITY_POPUP(player, ABILITY_BLOOM_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_CHERRIM_SUNSHINE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

