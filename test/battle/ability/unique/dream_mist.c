#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_SANDSTORM].effect == EFFECT_SANDSTORM);
    ASSUME(gBattleMoves[MOVE_TELEPORT].effect == EFFECT_TELEPORT);
}

SINGLE_BATTLE_TEST("Dream Mist uses Hypnosis at end of turn if the user took no damage")
{
    GIVEN {
        PLAYER(SPECIES_MUNNA) { Speed(10); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_DREAM_MIST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DREAM_MIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT_NE(opponent->status1 & STATUS1_SLEEP, 0);
    }
}

SINGLE_BATTLE_TEST("Dream Mist uses Hypnosis at end of turn after Teleport switches it in")
{
    GIVEN {
        PLAYER(SPECIES_ABRA) { Speed(5); Ability(ABILITY_NO_GUARD); Moves(MOVE_TELEPORT); }
        PLAYER(SPECIES_MUSHARNA) { Speed(10); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_DREAM_MIST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TELEPORT); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TELEPORT, player);
        MESSAGE("Go! Musharna!");
        ABILITY_POPUP(player, ABILITY_DREAM_MIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        STATUS_ICON(opponent, sleep: TRUE);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MUSHARNA);
        EXPECT_NE(opponent->status1 & STATUS1_SLEEP, 0);
    }
}

SINGLE_BATTLE_TEST("Dream Mist does not use Hypnosis if the user took damage that turn")
{
    GIVEN {
        PLAYER(SPECIES_MUNNA) { Speed(5); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_DREAM_MIST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DREAM_MIST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Dream Mist does not use Hypnosis if the user took passive weather damage")
{
    GIVEN {
        PLAYER(SPECIES_MUNNA) { Speed(10); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_DREAM_MIST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SANDSTORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SANDSTORM); }
    } SCENE {
        MESSAGE("Munna is buffeted by the sandstorm!");
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DREAM_MIST);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

DOUBLE_BATTLE_TEST("Dream Mist targets the remaining foe if the opposite foe fainted")
{
    GIVEN {
        PLAYER(SPECIES_MUNNA) { Speed(10); Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_DREAM_MIST); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(20); SpAttack(999); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); Speed(5); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_WATER_GUN, target: opponentLeft);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(opponentLeft);
        ABILITY_POPUP(playerLeft, ABILITY_DREAM_MIST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, playerLeft);
        STATUS_ICON(opponentRight, sleep: TRUE);
    } THEN {
        EXPECT_EQ(opponentLeft->hp, 0);
        EXPECT_NE(opponentRight->status1 & STATUS1_SLEEP, 0);
    }
}
