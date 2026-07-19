#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ORDER_UP].effect == EFFECT_ORDER_UP);
}

DOUBLE_BATTLE_TEST("Order Up raises the stat matching Tatsugiri's form")
{
    u32 species;
    u32 expectedAnimArg;

    PARAMETRIZE { species = SPECIES_TATSUGIRI_CURLY;    expectedAnimArg = MOVE_EFFECT_ATK_PLUS_1; }
    PARAMETRIZE { species = SPECIES_TATSUGIRI_DROOPY;   expectedAnimArg = MOVE_EFFECT_DEF_PLUS_1; }
    PARAMETRIZE { species = SPECIES_TATSUGIRI_STRETCHY; expectedAnimArg = MOVE_EFFECT_SPD_PLUS_1; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_HAZE); MOVE(playerRight, MOVE_ORDER_UP, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_UP, playerRight);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (species == SPECIES_TATSUGIRI_CURLY));
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (species == SPECIES_TATSUGIRI_DROOPY));
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (species == SPECIES_TATSUGIRI_STRETCHY));
        EXPECT_EQ(gBattleScripting.animArg1, expectedAnimArg);
    }
}

DOUBLE_BATTLE_TEST("Order Up remembers Tatsugiri's form after Tatsugiri faints")
{
    u32 species;

    PARAMETRIZE { species = SPECIES_TATSUGIRI_CURLY; }
    PARAMETRIZE { species = SPECIES_TATSUGIRI_DROOPY; }
    PARAMETRIZE { species = SPECIES_TATSUGIRI_STRETCHY; }

    GIVEN {
        PLAYER(species) { HP(1); Status1(STATUS1_POISON); Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); }
    } WHEN {
        TURN { }
        TURN { MOVE(opponentRight, MOVE_HAZE); MOVE(playerRight, MOVE_ORDER_UP, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Tatsugiri fainted!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_UP, playerRight);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + (species == SPECIES_TATSUGIRI_CURLY));
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE + (species == SPECIES_TATSUGIRI_DROOPY));
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + (species == SPECIES_TATSUGIRI_STRETCHY));
    }
}

DOUBLE_BATTLE_TEST("Order Up does not raise a stat without Commander")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_ORDER_UP, target: opponentLeft); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_UP, playerRight);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(playerRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Order Up does not raise a stat through Protect")
{
    GIVEN {
        PLAYER(SPECIES_TATSUGIRI_CURLY) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponentLeft, MOVE_PROTECT); MOVE(playerRight, MOVE_ORDER_UP, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        MESSAGE("Foe Wobbuffet protected itself!");
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Order Up is boosted by Sheer Force without suppressing its stat increase", s16 damage)
{
    bool32 sheerForce;

    PARAMETRIZE { sheerForce = FALSE; }
    PARAMETRIZE { sheerForce = TRUE; }

    GIVEN {
        PLAYER(SPECIES_TATSUGIRI_CURLY) { Ability(ABILITY_COMMANDER); }
        PLAYER(SPECIES_DONDOZO) { Ability(sheerForce ? ABILITY_SHEER_FORCE : ABILITY_WATER_VEIL); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_VOLBEAT) { Ability(ABILITY_PRANKSTER); }
    } WHEN {
        TURN { MOVE(opponentRight, MOVE_HAZE); MOVE(playerRight, MOVE_ORDER_UP, target: opponentLeft); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_COMMANDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE, opponentRight);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ORDER_UP, playerRight);
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, playerRight);
    } THEN {
        EXPECT_EQ(playerRight->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}
