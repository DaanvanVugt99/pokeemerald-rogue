#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_LICK].type == TYPE_GHOST);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Ultra Detonation lowers Sp. Def with the first Fire-type move after switch-in if it is the only Ultra Beast")
{
    GIVEN {
        PLAYER(SPECIES_BLACEPHALON) { SpAttack(1); Speed(100); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Ultra Detonation also lowers Sp. Def with Ghost-type moves")
{
    GIVEN {
        PLAYER(SPECIES_BLACEPHALON) { Attack(1); Speed(100); Moves(MOVE_LICK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LICK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Ultra Detonation ignores non Fire or Ghost moves without consuming its switch-in trigger")
{
    GIVEN {
        PLAYER(SPECIES_BLACEPHALON) { Attack(1); SpAttack(1); Speed(100); Moves(MOVE_TACKLE, MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(200); SpDefense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Ultra Detonation only lowers Sp. Def once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_BLACEPHALON) { SpAttack(1); Speed(100); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Ultra Detonation does not trigger if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_BLACEPHALON) { SpAttack(1); Speed(100); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_NIHILEGO) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(200); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_DETONATION);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}
