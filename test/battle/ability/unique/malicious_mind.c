#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_DISABLE].effect == EFFECT_DISABLE);
    ASSUME(gBattleMoves[MOVE_CONFUSE_RAY].effect == EFFECT_CONFUSE);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Malicious Mind uses Disable after the first status move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Speed(1); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MALICIOUS_MIND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_MALICIOUS_MIND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DISABLE, player);
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT_GT((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Malicious Mind also uses Confuse Ray if the target is poisoned")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Speed(1); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MALICIOUS_MIND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Status1(STATUS1_POISON); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MALICIOUS_MIND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DISABLE, player);
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_TACKLE);
        EXPECT(opponent->status2 & STATUS2_CONFUSION);
    }
}

SINGLE_BATTLE_TEST("Malicious Mind does not confuse a poisoned target protected from Confuse Ray")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Speed(1); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MALICIOUS_MIND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Status1(STATUS1_POISON); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        ABILITY_POPUP(player, ABILITY_MALICIOUS_MIND);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CONFUSE_RAY, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DISABLE, player);
        }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_NONE);
        EXPECT(!(opponent->status2 & STATUS2_CONFUSION));
    }
}

SINGLE_BATTLE_TEST("Malicious Mind only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Speed(1); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MALICIOUS_MIND); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MALICIOUS_MIND);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DISABLE, player);
        NOT ABILITY_POPUP(player, ABILITY_MALICIOUS_MIND);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Malicious Mind does not trigger after non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Speed(1); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MALICIOUS_MIND); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MALICIOUS_MIND);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DISABLE, player);
        }
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
