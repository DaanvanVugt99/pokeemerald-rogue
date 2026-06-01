#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mean Streak poisons a foe when the user's Attack changes")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MEAN_STREAK); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MEAN_STREAK);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

DOUBLE_BATTLE_TEST("Mean Streak poisons an opposing battler when the user's Attack changes")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MEAN_STREAK); Moves(MOVE_SWORDS_DANCE); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SWORDS_DANCE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_MEAN_STREAK);
    } THEN {
        EXPECT(!(playerLeft->status1 & STATUS1_POISON));
        EXPECT(!(playerRight->status1 & STATUS1_POISON));
        EXPECT(opponentLeft->status1 & STATUS1_POISON);
        EXPECT(!(opponentRight->status1 & STATUS1_POISON));
    }
}

SINGLE_BATTLE_TEST("Mean Streak restores the original target after poisoning during a multi-stat move")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TICKLE].effect == EFFECT_TICKLE);
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MEAN_STREAK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_TICKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TICKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MEAN_STREAK);
    } THEN {
        EXPECT(player->status1 == STATUS1_NONE);
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Mean Streak uses Payback the first time it hits a poisoned target each switch-in")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_PAYBACK].effect == EFFECT_PAYBACK);
        PLAYER(SPECIES_SENTRET) { Speed(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MEAN_STREAK); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); Status1(STATUS1_POISON); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MEAN_STREAK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAYBACK, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Mean Streak Payback deals double damage if the user moves after the target", u16 hp)
{
    u32 playerSpeed;
    PARAMETRIZE { playerSpeed = 100; }
    PARAMETRIZE { playerSpeed = 1; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_PAYBACK].effect == EFFECT_PAYBACK);
        PLAYER(SPECIES_SENTRET) { Speed(playerSpeed); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_MEAN_STREAK); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); HP(1000); MaxHP(1000); Status1(STATUS1_POISON); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(player, MOVE_TACKLE);
            MOVE(opponent, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MEAN_STREAK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAYBACK, player);
    } THEN {
        results[i].hp = opponent->hp;
    } FINALLY {
        EXPECT(results[1].hp < results[0].hp);
    }
}
