#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AROMATHERAPY].effect == EFFECT_HEAL_BELL);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
}

SINGLE_BATTLE_TEST("Season's Greeting uses Aromatherapy on switch-in in Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_GRASSY_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DEERLING) { Speed(80); Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_SEASONS_GREETING); Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SEASONS_GREETING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AROMATHERAPY, opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Season's Greeting uses Aromatherapy on switch-in in Plain Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_PLAIN_TERRAIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DEERLING) { Speed(80); Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_SEASONS_GREETING); Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_SEASONS_GREETING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AROMATHERAPY, opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Season's Greeting does not activate on switch-in without Grassy or Plain Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DEERLING) { Speed(80); Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_SEASONS_GREETING); Status1(STATUS1_POISON); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_SEASONS_GREETING);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}
