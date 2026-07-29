#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(!IS_MOVE_STATUS(MOVE_CONFUSION));
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
    ASSUME(gBattleMoves[MOVE_LIGHT_SCREEN].effect == EFFECT_LIGHT_SCREEN);
}

SINGLE_BATTLE_TEST("Screen Test uses Reflect after the first status move when chosen")
{
    GIVEN {
        PLAYER(SPECIES_ESPURR) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_SCREEN_TEST, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_SCREEN_TEST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Screen Test uses Light Screen after the first status move when chosen")
{
    GIVEN {
        PLAYER(SPECIES_ESPURR) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_SCREEN_TEST, 1)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_SCREEN_TEST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT));
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
    }
}

SINGLE_BATTLE_TEST("Screen Test always chooses the other screen when triggered by Reflect or Light Screen")
{
    u16 triggerMove;
    u16 calledMove;

    PARAMETRIZE { triggerMove = MOVE_REFLECT; calledMove = MOVE_LIGHT_SCREEN; }
    PARAMETRIZE { triggerMove = MOVE_LIGHT_SCREEN; calledMove = MOVE_REFLECT; }

    GIVEN {
        PLAYER(SPECIES_ESPURR) { Ability(ABILITY_KEEN_EYE); Moves(triggerMove); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, triggerMove); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, triggerMove, player);
        ABILITY_POPUP(player, ABILITY_SCREEN_TEST);
        ANIMATION(ANIM_TYPE_MOVE, calledMove, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
    }
}

SINGLE_BATTLE_TEST("Screen Test only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ESPURR) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_SCREEN_TEST, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP, WITH_RNG(RNG_ROGUE_SCREEN_TEST, 1)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SCREEN_TEST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SCREEN_TEST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
    }
}

SINGLE_BATTLE_TEST("Screen Test does not trigger after damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_ESPURR) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_CONFUSION); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONFUSION); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSION, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SCREEN_TEST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_REFLECT, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_REFLECT));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN));
    }
}
