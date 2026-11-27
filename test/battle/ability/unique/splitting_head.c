#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Splitting Head copies the foe's stat stages on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Speed(50); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_GOLDUCK) { Speed(80); Ability(ABILITY_DAMP); UniqueAbility(ABILITY_SPLITTING_HEAD); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_CALM_MIND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CALM_MIND); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponent);
        MESSAGE("Go! Golduck!");
        ABILITY_POPUP(player, ABILITY_SPLITTING_HEAD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCH_UP, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Splitting Head leaves neutral stat stages unchanged on entry")
{
    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Ability(ABILITY_DAMP); UniqueAbility(ABILITY_SPLITTING_HEAD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPLITTING_HEAD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCH_UP, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Splitting Head copies boosts when brought in by a pivot move")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN) { Speed(40); Attack(60); Moves(MOVE_U_TURN); }
        PLAYER(SPECIES_GOLDUCK) { Speed(80); Ability(ABILITY_DAMP); UniqueAbility(ABILITY_SPLITTING_HEAD); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); HP(500); MaxHP(500); Ability(ABILITY_TELEPATHY); Moves(MOVE_CALM_MIND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CALM_MIND); MOVE(player, MOVE_U_TURN); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        MESSAGE("Go! Golduck!");
        ABILITY_POPUP(player, ABILITY_SPLITTING_HEAD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCH_UP, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

DOUBLE_BATTLE_TEST("Splitting Head doubles confusion self-hit damage for either side", s16 playerDamage; s16 opponentDamage)
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_GLUTTONY; }
    PARAMETRIZE { ability = ABILITY_SPLITTING_HEAD; }

    GIVEN {
        PLAYER(SPECIES_PSYDUCK) { Speed(50); Ability(ABILITY_DAMP); UniqueAbility(ability); Moves(MOVE_CONFUSE_RAY, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(80); HP(500); MaxHP(500); Attack(100); Defense(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); Moves(MOVE_CONFUSE_RAY, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(70); HP(500); MaxHP(500); Attack(100); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
            MOVE(playerLeft, MOVE_CONFUSE_RAY, target: opponentRight);
            MOVE(opponentLeft, MOVE_CONFUSE_RAY, target: playerRight);
        }
        TURN {
            MOVE(playerRight, MOVE_CELEBRATE, WITH_RNG(RNG_CONFUSION, TRUE));
            MOVE(opponentRight, MOVE_CELEBRATE, WITH_RNG(RNG_CONFUSION, TRUE));
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(playerRight, captureDamage: &results[i].playerDamage);
        HP_BAR(opponentRight, captureDamage: &results[i].opponentDamage);
    } FINALLY {
        EXPECT_GT(results[0].playerDamage, 0);
        EXPECT_GT(results[0].opponentDamage, 0);
        EXPECT_EQ(results[1].playerDamage, 2 * results[0].playerDamage);
        EXPECT_EQ(results[1].opponentDamage, 2 * results[0].opponentDamage);
    }
}
