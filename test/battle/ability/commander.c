#include "global.h"
#include "battle_script_commands.h"
#include "battle_util.h"
#include "test/battle.h"

DOUBLE_BATTLE_TEST("Commander partner ownership is checked per side")
{
    u32 battleTypeFlags;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } THEN {
        battleTypeFlags = gBattleTypeFlags;

        // Multi battle: both sides are controlled by separate trainers.
        gBattleTypeFlags = BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER | BATTLE_TYPE_TWO_OPPONENTS;
        EXPECT_EQ(IsPartnerMonFromSameTrainer(B_POSITION_PLAYER_LEFT), FALSE);
        EXPECT_EQ(IsPartnerMonFromSameTrainer(B_POSITION_OPPONENT_LEFT), FALSE);

        // 2-vs-1: only the player side has separate trainers.
        gBattleTypeFlags = BATTLE_TYPE_DOUBLE | BATTLE_TYPE_MULTI | BATTLE_TYPE_INGAME_PARTNER;
        EXPECT_EQ(IsPartnerMonFromSameTrainer(B_POSITION_PLAYER_LEFT), FALSE);
        EXPECT_EQ(IsPartnerMonFromSameTrainer(B_POSITION_OPPONENT_LEFT), TRUE);

        // 1-vs-2: only the opponent side has separate trainers.
        gBattleTypeFlags = BATTLE_TYPE_DOUBLE | BATTLE_TYPE_TWO_OPPONENTS;
        EXPECT_EQ(IsPartnerMonFromSameTrainer(B_POSITION_PLAYER_LEFT), TRUE);
        EXPECT_EQ(IsPartnerMonFromSameTrainer(B_POSITION_OPPONENT_LEFT), FALSE);

        gBattleTypeFlags = battleTypeFlags;
    }
}

DOUBLE_BATTLE_TEST("Commander activates when Tatsugiri enters beside Dondozo")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Tatsugiri was swallowed by Dondozo and became its commander!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
        MESSAGE("Dondozo's Attack sharply rose!");
        MESSAGE("Dondozo's Defense sharply rose!");
        MESSAGE("Dondozo's Sp. Atk sharply rose!");
        MESSAGE("Dondozo's Sp. Def sharply rose!");
        MESSAGE("Dondozo's Speed sharply rose!");
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Commander activates when Dondozo enters beside Tatsugiri")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        MESSAGE("Tatsugiri was swallowed by Dondozo and became its commander!");
    }
}

DOUBLE_BATTLE_TEST("Commander stat changes respect Contrary")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO) { Ability(ABILITY_CONTRARY); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

DOUBLE_BATTLE_TEST("Commander cancels Tatsugiri's selected action")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT MESSAGE("Tatsugiri used Celebrate!");
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri cannot be targeted even by No Guard")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentLeft);
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri is skipped by spread damage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SURF].target == MOVE_TARGET_FOES_AND_ALLY);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_SURF); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, opponentLeft);
        NOT HP_BAR(playerLeft);
        HP_BAR(playerRight);
        HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri remains protected when spread damage faints Dondozo")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EARTHQUAKE].target == MOVE_TARGET_FOES_AND_ALLY);
        PLAYER(SPECIES_DONDOZO) { HP(1); }
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_EARTHQUAKE); SEND_OUT(playerLeft, 2); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        HP_BAR(playerLeft);
        MESSAGE("Dondozo fainted!");
        NOT HP_BAR(playerRight);
        HP_BAR(opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri is missed by both hits of Dragon Darts")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); HP(100); MaxHP(100); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_DRAGON_DARTS, target: playerLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri still takes poison damage")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); Status1(STATUS1_POISON); HP(100); MaxHP(100); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
    } THEN {
        EXPECT_LT(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri still takes weather damage")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); HP(100); MaxHP(100); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_TYRANITAR) { Ability(ABILITY_SAND_STREAM); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Tatsugiri is buffeted by the sandstorm!");
    } THEN {
        EXPECT_LT(playerLeft->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Commander prevents Perish Song from newly affecting swallowed Tatsugiri")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PERISH_SONG); }
        TURN { }
        TURN { }
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        NONE_OF {
            MESSAGE("Tatsugiri's perish count fell to 0!");
            MESSAGE("Tatsugiri fainted!");
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 0);
        EXPECT_GT(playerRight->hp, 0);
    }
}

DOUBLE_BATTLE_TEST("Commander Tatsugiri keeps a Perish Song countdown started before activation")
{
    u32 perishSongTimer;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PERISH_SONG); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
    } THEN {
        EXPECT_GT(playerLeft->hp, 0);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)] & STATUS3_PERISH_SONG);
        perishSongTimer = gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)].perishSongTimer;
        EXPECT_LT(perishSongTimer, 3);
    }
}

DOUBLE_BATTLE_TEST("Commander still allows Haze to reset swallowed Tatsugiri's stat changes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_SWORDS_DANCE); }
        TURN { SWITCH(playerLeft, 2); MOVE(opponentRight, MOVE_HAZE); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE, opponentRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Commander still allows Transform to copy swallowed Tatsugiri")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_TRANSFORM, target: playerRight); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRANSFORM, opponentRight);
    }
}

DOUBLE_BATTLE_TEST("Commander still allows Imposter to copy swallowed Tatsugiri")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DITTO) { Ability(ABILITY_IMPOSTER); }
    } WHEN {
        TURN { }
        TURN { SWITCH(opponentRight, 2); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        ABILITY_POPUP(opponentRight, ABILITY_IMPOSTER);
        MESSAGE("Foe Ditto transformed into Tatsugiri using Imposter!");
    }
}

DOUBLE_BATTLE_TEST("Commander clears its pairing when Dondozo faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN {
            SWITCH(playerLeft, 2);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(opponentRight, MOVE_TACKLE, target: playerRight);
            MOVE(playerRight, MOVE_CELEBRATE);
            SEND_OUT(playerLeft, 0);
        }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        MESSAGE("Dondozo fainted!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponentRight);
        HP_BAR(playerRight);
        NOT MESSAGE("Tatsugiri used Celebrate!");
    } THEN {
        EXPECT_EQ(GET_COMMANDER_FORM(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), COMMANDER_FORM_NONE);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)] & STATUS3_COMMANDER));
    }
}

DOUBLE_BATTLE_TEST("Commander clears its pairing when Dondozo faints from residual damage")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO) { HP(1); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Status1(STATUS1_POISON); }
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Dondozo fainted!");
    } THEN {
        EXPECT_EQ(GET_COMMANDER_FORM(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)), COMMANDER_FORM_NONE);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_COMMANDER));
    }
}

DOUBLE_BATTLE_TEST("Commander pairing persists after Tatsugiri faints")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); HP(1); Status1(STATUS1_POISON); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Tatsugiri fainted!");
    } THEN {
        EXPECT_EQ(GET_COMMANDER_FORM(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)), COMMANDER_FORM_CURLY);
        EXPECT(!CanBattlerSwitch(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)));
    }
}

DOUBLE_BATTLE_TEST("Commander prevents Dondozo from switching voluntarily")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
    } THEN {
        EXPECT(!CanBattlerSwitch(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)));
        EXPECT(!CanBattlerEscape(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)));
    }
}

DOUBLE_BATTLE_TEST("Commander blocks a replacement Tatsugiri until Dondozo is revived")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); HP(1); Status1(STATUS1_POISON); }
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(playerLeft, 2); SEND_OUT(playerLeft, 3); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Tatsugiri fainted!");
        NOT ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
    } THEN {
        EXPECT_EQ(GET_COMMANDER_FORM(GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)), COMMANDER_FORM_CURLY);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_COMMANDER));
    }
}

DOUBLE_BATTLE_TEST("Commander prevents forced switching")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_WHIRLWIND, target: playerRight); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
    } THEN {
        EXPECT_EQ(playerRight->species, SPECIES_DONDOZO);
    }
}

DOUBLE_BATTLE_TEST("Commander consumes Red Card without switching Dondozo")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_TACKLE, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponentLeft);
    } THEN {
        EXPECT_EQ(opponentLeft->item, ITEM_NONE);
        EXPECT_EQ(playerRight->species, SPECIES_DONDOZO);
    }
}

DOUBLE_BATTLE_TEST("Commander blocks Dondozo's Eject Button and pivot moves")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO) { Item(ITEM_EJECT_BUTTON); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_TACKLE, target: playerRight); }
        TURN { MOVE(playerRight, MOVE_FLIP_TURN, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT MESSAGE("Dondozo's Eject Button activated!");
        MESSAGE("Dondozo used Flip Turn!");
    } THEN {
        EXPECT_EQ(playerRight->species, SPECIES_DONDOZO);
        EXPECT_EQ(playerRight->item, ITEM_EJECT_BUTTON);
    }
}

DOUBLE_BATTLE_TEST("Commander blocks Dondozo's Eject Pack")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO) { Item(ITEM_EJECT_PACK); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_CHARM, target: playerRight); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->species, SPECIES_DONDOZO);
        EXPECT_EQ(playerRight->item, ITEM_EJECT_PACK);
    }
}

DOUBLE_BATTLE_TEST("Commander blocks Tatsugiri's Eject Pack after Sticky Web")
{
    GIVEN {
        ASSUME(gItems[ITEM_EJECT_PACK].holdEffect == HOLD_EFFECT_EJECT_PACK);
        ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DONDOZO);
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); Item(ITEM_EJECT_PACK); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_STICKY_WEB); }
        TURN { SWITCH(playerLeft, 2); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STICKY_WEB, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_TATSUGIRI);
        EXPECT_EQ(playerLeft->item, ITEM_EJECT_PACK);
        EXPECT_EQ(playerRight->species, SPECIES_DONDOZO);
    }
}

DOUBLE_BATTLE_TEST("Commander prevents Ally Switch from moving Dondozo")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_ALLY_SWITCH); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Dondozo used Ally Switch!");
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(playerRight->species, SPECIES_DONDOZO);
        EXPECT_EQ(playerLeft->species, SPECIES_TATSUGIRI);
    }
}

SINGLE_BATTLE_TEST("Commander cannot be replaced by most ability-changing moves")
{
    u16 move;

    PARAMETRIZE { move = MOVE_GASTRO_ACID; }
    PARAMETRIZE { move = MOVE_WORRY_SEED; }
    PARAMETRIZE { move = MOVE_ENTRAINMENT; }
    PARAMETRIZE { move = MOVE_ROLE_PLAY; }
    PARAMETRIZE { move = MOVE_SKILL_SWAP; }
    PARAMETRIZE { move = MOVE_DOODLE; }

    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BLAZE); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        MESSAGE("But it failed!");
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_COMMANDER);
    }
}

SINGLE_BATTLE_TEST("Simple Beam can replace Commander")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SIMPLE_BEAM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SIMPLE_BEAM, opponent);
        MESSAGE("Tatsugiri acquired Simple!");
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_SIMPLE);
    }
}

SINGLE_BATTLE_TEST("Lingering Aroma cannot replace Commander")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        OPPONENT(SPECIES_OINKOLOGNE_FEMALE) { Ability(ABILITY_LINGERING_AROMA); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_LINGERING_AROMA);
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_COMMANDER);
    }
}

SINGLE_BATTLE_TEST("Trace cannot copy Commander")
{
    GIVEN {
        PLAYER(SPECIES_RALTS) { Ability(ABILITY_TRACE); }
        OPPONENT(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
    } WHEN {
        TURN { }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TRACE);
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_TRACE);
    }
}

DOUBLE_BATTLE_TEST("Receiver and Power of Alchemy cannot copy Commander")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_RECEIVER; }
    PARAMETRIZE { ability = ABILITY_POWER_OF_ALCHEMY; }

    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); HP(1); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_ROCK_SLIDE); }
    } SCENE {
        MESSAGE("Tatsugiri fainted!");
        NOT ABILITY_POPUP(playerRight, ability);
    } THEN {
        EXPECT_EQ(playerRight->ability, ability);
    }
}

DOUBLE_BATTLE_TEST("Commander can activate again after Dondozo faints and is revived")
{
    GIVEN {
        PLAYER(SPECIES_DONDOZO) { HP(1); }
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_PAWMOT);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft); SEND_OUT(playerLeft, 2); }
        TURN { MOVE(playerLeft, MOVE_REVIVAL_BLESSING); SEND_OUT(playerLeft, 0); }
        TURN { SWITCH(playerLeft, 0); }
    } SCENE {
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
        MESSAGE("Dondozo fainted!");
        MESSAGE("Dondozo was revived and is ready to fight again!");
        ABILITY_POPUP(playerRight, ABILITY_COMMANDER);
    } THEN {
        EXPECT_EQ(GET_COMMANDER_FORM(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), COMMANDER_FORM_CURLY);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_RIGHT)] & STATUS3_COMMANDER);
    }
}

DOUBLE_BATTLE_TEST("Commander cancels Tatsugiri's pending Mega Evolution")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); Item(ITEM_TATSUGIRINITE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE, megaEvolve: TRUE); SWITCH(playerRight, 2); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_MEGA_EVOLUTION, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_TATSUGIRI);
    }
}

DOUBLE_BATTLE_TEST("Commander cancels Tatsugiri's pending Dynamax")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft, dynamax: TRUE); SWITCH(playerRight, 2); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_DYNAMAX_GROWTH, playerLeft);
    } THEN {
        EXPECT(!gBattleStruct->dynamax.dynamaxed[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)]);
    }
}

DOUBLE_BATTLE_TEST("Commander cancels Tatsugiri's pending Terastallization")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI) { Ability(ABILITY_COMMANDER); TeraType(TYPE_FIRE); }
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE, tera: TRUE); SWITCH(playerRight, 2); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_TERA_ACTIVATE, playerLeft);
    } THEN {
        EXPECT(!gBattleStruct->tera.alreadyTerastallized[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)]);
    }
}
