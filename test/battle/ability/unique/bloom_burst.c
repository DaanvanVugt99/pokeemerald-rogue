#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLOWER_SHIELD].effect == EFFECT_FLOWER_SHIELD);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].power > 0);
    ASSUME(gBattleMoves[MOVE_GROWTH].effect == EFFECT_GROWTH);
}

SINGLE_BATTLE_TEST("Bloom Burst uses a random flower move after Flower Shield")
{
    GIVEN {
        PLAYER(SPECIES_CHERRIM_OVERCAST) { Ability(ABILITY_FLOWER_GIFT); UniqueAbility(ABILITY_BLOOM_BURST); Moves(MOVE_FLOWER_SHIELD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLOWER_SHIELD, WITH_RNG(RNG_ROGUE_BLOOM_BURST, MOVE_MAGICAL_LEAF)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, player);
        ABILITY_POPUP(player, ABILITY_BLOOM_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Bloom Burst can choose a self-target flower move after Flower Shield")
{
    GIVEN {
        PLAYER(SPECIES_CHERRIM_SUNSHINE) { Ability(ABILITY_FLOWER_GIFT); UniqueAbility(ABILITY_BLOOM_BURST); Moves(MOVE_FLOWER_SHIELD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLOWER_SHIELD, WITH_RNG(RNG_ROGUE_BLOOM_BURST, MOVE_GROWTH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLOWER_SHIELD, player);
        ABILITY_POPUP(player, ABILITY_BLOOM_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWTH, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Bloom Burst does not trigger after other moves")
{
    GIVEN {
        PLAYER(SPECIES_CHERUBI) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_BLOOM_BURST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_BLOOM_BURST, MOVE_MAGICAL_LEAF)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BLOOM_BURST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGICAL_LEAF, player);
        }
    }
}
