#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
    ASSUME(gBattleMoves[MOVE_HYDRO_PUMP].split != SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_HYDRO_PUMP].accuracy != 0);
    ASSUME(gBattleMoves[MOVE_SWIFT].split != SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_SWIFT].accuracy == 0);
}

SINGLE_BATTLE_TEST("Breach Point makes the incoming ally's first damaging move unable to miss")
{
    GIVEN {
        PLAYER(SPECIES_LOKIX) { UniqueAbility(ABILITY_BREACH_POINT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_HYDRO_PUMP); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BREACH_POINT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOCK_ON, player, target: opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet's attack missed!");
    }
}

SINGLE_BATTLE_TEST("Breach Point transfers after U-turn")
{
    GIVEN {
        PLAYER(SPECIES_LOKIX) { Speed(100); UniqueAbility(ABILITY_BREACH_POINT); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_HYDRO_PUMP); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        ABILITY_POPUP(player, ABILITY_BREACH_POINT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOCK_ON, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Breach Point is not consumed by status moves")
{
    GIVEN {
        PLAYER(SPECIES_LOKIX) { UniqueAbility(ABILITY_BREACH_POINT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_HYPNOSIS, MOVE_HYDRO_PUMP); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BREACH_POINT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOCK_ON, player);
        MESSAGE("Wobbuffet's attack missed!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Breach Point is consumed by an inherently accurate damaging move")
{
    GIVEN {
        PLAYER(SPECIES_LOKIX) { UniqueAbility(ABILITY_BREACH_POINT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SWIFT, MOVE_HYDRO_PUMP); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BREACH_POINT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LOCK_ON, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        HP_BAR(opponent);
        MESSAGE("Wobbuffet's attack missed!");
    }
}
