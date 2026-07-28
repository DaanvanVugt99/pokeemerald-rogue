#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STRING_SHOT].effect == EFFECT_SPEED_DOWN_2 || gBattleMoves[MOVE_STRING_SHOT].effect == EFFECT_SPEED_DOWN);
    ASSUME(gBattleMoves[MOVE_SPIDER_WEB].effect == EFFECT_MEAN_LOOK);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
    ASSUME(gBattleMoves[MOVE_TOXIC_THREAD].effect == EFFECT_TOXIC_THREAD);
    ASSUME(gBattleMoves[MOVE_ROCK_TOMB].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_ELECTROWEB].effect == EFFECT_SPEED_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_ELECTROWEB].target == MOVE_TARGET_BOTH);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
    ASSUME(gBattleMoves[MOVE_SYRUP_BOMB].effect == EFFECT_SYRUP_BOMB);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Web Trap follows Spider Web's Speed drop with another web move")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_SPIDER_WEB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIDER_WEB, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_STICKY_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, player);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STICKY_WEB);
    }
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

SINGLE_BATTLE_TEST("Web Trap triggers after Toxic Thread lowers Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_TOXIC_THREAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_THREAD, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TOXIC_THREAD, player);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Web Trap triggers after a damaging move lowers Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_ROCK_TOMB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_TOMB, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_TOMB, player);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Web Trap waits for a spread move to finish lowering Speed before triggering")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_ELECTROWEB); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ELECTROWEB, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet's Speed fell!");
        MESSAGE("Foe Wynaut's Speed fell!");
        ABILITY_POPUP(playerLeft, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, playerLeft);
    } THEN {
        EXPECT_LT(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_LT(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Web Trap triggers when an ally lowers an enemy's Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_SCARY_FACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_SCARY_FACE, target: opponentLeft, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, playerLeft);
    } THEN {
        EXPECT_LT(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Web Trap triggers after Sticky Web lowers an enemy's Speed on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_STICKY_WEB, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH_WITH_RNG(opponent, 1, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, player);
        MESSAGE("Foe Wynaut was caught in a Sticky Web!");
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Web Trap triggers after a switch-in ability lowers an enemy's Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUGTRIO) { Speed(60); Ability(ABILITY_ARENA_TRAP); UniqueAbility(ABILITY_SINKHOLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIDGEY) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); SWITCH_WITH_RNG(playerRight, 2, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_SINKHOLE);
        ABILITY_POPUP(playerLeft, ABILITY_WEB_TRAP);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, playerLeft);
    } THEN {
        EXPECT_LT(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Web Trap triggers after a residual effect lowers an enemy's Speed")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_SYRUP_BOMB); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SYRUP_BOMB, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SYRUP_BOMB, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SYRUP_BOMB_SPEED_DROP, opponent);
        ABILITY_POPUP(player, ABILITY_WEB_TRAP);
    } THEN {
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
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

SINGLE_BATTLE_TEST("Web Trap does not trigger when the user's Speed is lowered")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Speed(40); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SCARY_FACE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCARY_FACE, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WEB_TRAP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
        }
    } THEN {
        EXPECT_LT(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Web Trap does not trigger if Speed lowering is prevented")
{
    GIVEN {
        PLAYER(SPECIES_ARIADOS) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_WEB_TRAP); Moves(MOVE_SCARY_FACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_CLEAR_BODY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCARY_FACE, WITH_RNG(RNG_ROGUE_WEB_TRAP, MOVE_SPIDER_WEB)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WEB_TRAP);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIDER_WEB, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
