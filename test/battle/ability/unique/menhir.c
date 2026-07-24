#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
}

SINGLE_BATTLE_TEST("Menhir caps a hit at half max HP while above half HP")
{
    GIVEN {
        PLAYER(SPECIES_STONJOURNER) { HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(player, hp: 50);
    }
}

SINGLE_BATTLE_TEST("Menhir can leave its holder at one HP")
{
    GIVEN {
        PLAYER(SPECIES_STONJOURNER) { HP(51); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(player, hp: 1);
    }
}

SINGLE_BATTLE_TEST("Menhir does not protect at half HP")
{
    GIVEN {
        PLAYER(SPECIES_STONJOURNER) { HP(50); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        HP_BAR(player, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Menhir caps successful one-hit KO moves at half max HP")
{
    GIVEN {
        PLAYER(SPECIES_STONJOURNER) { Level(100); HP(100); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_FISSURE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        HP_BAR(player, hp: 50);
    }
}

SINGLE_BATTLE_TEST("Menhir caps each hit separately and does not stop multi-hit moves")
{
    GIVEN {
        PLAYER(SPECIES_STONJOURNER) { HP(100); MaxHP(100); Defense(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1000); Ability(ABILITY_SKILL_LINK); Moves(MOVE_BULLET_SEED); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BULLET_SEED); }
    } SCENE {
        HP_BAR(player, hp: 50);
        HP_BAR(player, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Menhir does not make a one HP holder immune to direct damage")
{
    GIVEN {
        PLAYER(SPECIES_SHEDINJA) { HP(1); MaxHP(1); Ability(ABILITY_KLUTZ); UniqueAbility(ABILITY_MENHIR); Defense(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(1000); Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BITE); }
    } SCENE {
        HP_BAR(player, hp: 0);
    }
}
