#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PRESENT].effect == EFFECT_PRESENT);
    ASSUME(gBattleMoves[MOVE_RECYCLE].effect == EFFECT_RECYCLE);
    ASSUME(gBattleMoves[MOVE_HAPPY_HOUR].power == 0);
    ASSUME(gBattleMoves[MOVE_ICE_SHARD].power > 0);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].power == 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Delivery Bag uses a random gift move after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag can choose Happy Hour after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_HAPPY_HOUR)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAPPY_HOUR, player);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag can choose Baby-Doll Eyes after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_BABY_DOLL_EYES)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BABY_DOLL_EYES, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Delivery Bag does not trigger after using a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_DELIBIRD) { Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_DELIVERY_BAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_DELIVERY_BAG, MOVE_ICE_SHARD)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DELIVERY_BAG);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ICE_SHARD, player);
        }
    }
}
