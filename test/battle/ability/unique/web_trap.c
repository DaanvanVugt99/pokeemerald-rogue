#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STRING_SHOT].effect == EFFECT_SPEED_DOWN_2 || gBattleMoves[MOVE_STRING_SHOT].effect == EFFECT_SPEED_DOWN);
    ASSUME(gBattleMoves[MOVE_SPIDER_WEB].effect == EFFECT_MEAN_LOOK);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Web Trap uses a random web move after lowering a target's Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_STRING_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STRING_SHOT, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRING_SHOT, player);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Web Trap can choose Sticky Web after lowering a target's Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_STRING_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STRING_SHOT, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_STICKY_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRING_SHOT, player);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STICKY_WEB);
    }
}

SINGLE_BATTLE_TEST("Web Trap does not trigger if Speed is not lowered")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WEB_TRAP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
        }
    }
}
