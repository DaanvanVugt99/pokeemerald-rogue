#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
    ASSUME(gBattleMoves[MOVE_LIGHT_SCREEN].effect == EFFECT_LIGHT_SCREEN);
}

SINGLE_BATTLE_TEST("Regal Aegis uses a random screen after the first status move each switch-in")
{
    u32 rngResult;
    u16 screenMove;
    u32 sideStatus;

    PARAMETRIZE { rngResult = 0; screenMove = MOVE_REFLECT; sideStatus = SIDE_STATUS_REFLECT; }
    PARAMETRIZE { rngResult = 1; screenMove = MOVE_LIGHT_SCREEN; sideStatus = SIDE_STATUS_LIGHTSCREEN; }

    GIVEN {
        PLAYER(SPECIES_CALYREX) { Speed(100); Ability(ABILITY_UNNERVE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_REGAL_AEGIS, rngResult)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_REGAL_AEGIS);
        ANIMATION(ANIM_TYPE_MOVE, screenMove, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & sideStatus);
        EXPECT_EQ(gCalledMove, screenMove);
    }
}

SINGLE_BATTLE_TEST("Regal Aegis only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_CALYREX) { Speed(100); Ability(ABILITY_UNNERVE); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_REGAL_AEGIS, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP, WITH_RNG(RNG_ROGUE_REGAL_AEGIS, 1)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_REGAL_AEGIS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_REGAL_AEGIS);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
    }
}
