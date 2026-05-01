#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_KARATE_CHOP].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIGHTING);
    ASSUME(gSpeciesInfo[SPECIES_MACHOP].types[0] == TYPE_FIGHTING || gSpeciesInfo[SPECIES_MACHOP].types[1] == TYPE_FIGHTING);
    ASSUME(gSpeciesInfo[SPECIES_WURMPLE].types[0] != TYPE_FIGHTING && gSpeciesInfo[SPECIES_WURMPLE].types[1] != TYPE_FIGHTING);
}

SINGLE_BATTLE_TEST("Main Event halves damage from Fighting-type moves", s16 damage)
{
    bool32 hasMainEvent;

    PARAMETRIZE { hasMainEvent = FALSE; }
    PARAMETRIZE { hasMainEvent = TRUE; }

    GIVEN {
        if (hasMainEvent)
            PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_MAIN_EVENT); }
        else
            PLAYER(SPECIES_WOBBUFFET) { }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_KARATE_CHOP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_KARATE_CHOP); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Main Event restores half HP after knocking out a Fighting-type target")
{
    GIVEN {
        PLAYER(SPECIES_HAWLUCHA) { HP(100); MaxHP(300); Ability(ABILITY_UNBURDEN); UniqueAbility(ABILITY_MAIN_EVENT); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_MACHOP) { HP(1); MaxHP(300); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MAIN_EVENT);
        HP_BAR(player);
        MESSAGE("Hawlucha regained health!");
    } THEN {
        EXPECT_EQ(player->hp, 250);
    }
}

SINGLE_BATTLE_TEST("Main Event does not restore HP after knocking out a non-Fighting target")
{
    GIVEN {
        PLAYER(SPECIES_HAWLUCHA) { HP(100); MaxHP(300); Ability(ABILITY_UNBURDEN); UniqueAbility(ABILITY_MAIN_EVENT); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WURMPLE) { HP(1); MaxHP(300); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_MAIN_EVENT);
    } THEN {
        EXPECT_EQ(player->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Main Event does not halve non-Fighting-type damage", s16 damage)
{
    bool32 hasMainEvent;

    PARAMETRIZE { hasMainEvent = FALSE; }
    PARAMETRIZE { hasMainEvent = TRUE; }

    GIVEN {
        if (hasMainEvent)
            PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_MAIN_EVENT); }
        else
            PLAYER(SPECIES_WOBBUFFET) { }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
