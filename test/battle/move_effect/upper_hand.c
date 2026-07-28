#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_UPPER_HAND].effect == EFFECT_UPPER_HAND);
    ASSUME(gBattleMoves[MOVE_UPPER_HAND].priority == 3);
    ASSUME(gBattleMoves[MOVE_UPPER_HAND].secondaryEffectChance == 100);
}

SINGLE_BATTLE_TEST("Upper Hand interrupts and flinches a target using a damaging priority move")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EXTREME_SPEED].priority == 2);
        PLAYER(SPECIES_MIENSHAO);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EXTREME_SPEED); MOVE(player, MOVE_UPPER_HAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_UPPER_HAND, player);
        HP_BAR(opponent);
        MESSAGE("Foe Wobbuffet flinched!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_EXTREME_SPEED, opponent);
    }
}

SINGLE_BATTLE_TEST("Upper Hand fails against priority status moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BABY_DOLL_EYES].priority == 1);
        ASSUME(IS_MOVE_STATUS(MOVE_BABY_DOLL_EYES));
        PLAYER(SPECIES_MIENSHAO);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_BABY_DOLL_EYES); MOVE(player, MOVE_UPPER_HAND); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_UPPER_HAND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BABY_DOLL_EYES, opponent);
    }
}

SINGLE_BATTLE_TEST("Upper Hand fails against moves without positive priority")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_WATER_GUN].priority == 0);
        PLAYER(SPECIES_MIENSHAO);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_UPPER_HAND); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_UPPER_HAND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
    }
}

SINGLE_BATTLE_TEST("Upper Hand works against a healing attack given priority by Triage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRAINING_KISS].priority == 0);
        PLAYER(SPECIES_MIENSHAO) { Speed(10); }
        OPPONENT(SPECIES_COMFEY) { Speed(5); Ability(ABILITY_TRIAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAINING_KISS); MOVE(player, MOVE_UPPER_HAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_UPPER_HAND, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAINING_KISS, opponent);
    }
}

SINGLE_BATTLE_TEST("Upper Hand is boosted by Sheer Force without flinching the target")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EXTREME_SPEED].priority == 2);
        PLAYER(SPECIES_HARIYAMA) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_EXTREME_SPEED); MOVE(player, MOVE_UPPER_HAND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_UPPER_HAND, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EXTREME_SPEED, opponent);
        HP_BAR(player);
    }
}
