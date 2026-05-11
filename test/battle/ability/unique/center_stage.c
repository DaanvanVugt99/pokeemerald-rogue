#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUIVER_DANCE].danceMove);
    ASSUME(!gBattleMoves[MOVE_CELEBRATE].danceMove);
    ASSUME(gBattleMoves[MOVE_FOLLOW_ME].effect == EFFECT_FOLLOW_ME);
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].strikeCount < 2);
}

DOUBLE_BATTLE_TEST("Center Stage uses Follow Me after dance moves and primes the next Flying move")
{
    GIVEN {
        PLAYER(SPECIES_ORICORIO_BAILE) { Ability(ABILITY_DANCER); UniqueAbility(ABILITY_CENTER_STAGE); Moves(MOVE_QUIVER_DANCE, MOVE_AERIAL_ACE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_QUIVER_DANCE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_AERIAL_ACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_CENTER_STAGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOLLOW_ME, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentLeft);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

DOUBLE_BATTLE_TEST("Center Stage does not prime Flying moves after non-dance status moves")
{
    GIVEN {
        PLAYER(SPECIES_ORICORIO_BAILE) { Ability(ABILITY_DANCER); UniqueAbility(ABILITY_CENTER_STAGE); Moves(MOVE_CELEBRATE, MOVE_AERIAL_ACE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_AERIAL_ACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, playerLeft);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_CENTER_STAGE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FOLLOW_ME, playerLeft);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, playerLeft);
        HP_BAR(opponentLeft);
        NONE_OF {
            HP_BAR(opponentLeft);
        }
    }
}

DOUBLE_BATTLE_TEST("Center Stage second Flying hit uses 25 percent damage", s16 hit1, s16 hit2)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_CENTER_STAGE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_NONE); UniqueAbility(uniqueAbility); Moves(MOVE_QUIVER_DANCE, MOVE_AERIAL_ACE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(70); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_QUIVER_DANCE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_AERIAL_ACE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUIVER_DANCE, playerLeft);
        if (uniqueAbility == ABILITY_CENTER_STAGE)
        {
            ABILITY_POPUP(playerLeft, ABILITY_CENTER_STAGE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FOLLOW_ME, playerLeft);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, playerLeft);
        HP_BAR(opponentLeft, captureDamage: &results[i].hit1);
        if (uniqueAbility == ABILITY_CENTER_STAGE)
            HP_BAR(opponentLeft, captureDamage: &results[i].hit2);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].hit1, Q_4_12(1.25), results[1].hit1 + results[1].hit2);
    }
}
