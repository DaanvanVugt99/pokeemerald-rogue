#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STUN_SPORE].powderMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].powderMove);
}

SINGLE_BATTLE_TEST("Seedcloud restores 1/4 max HP after using a powder move")
{
    GIVEN {
        PLAYER(SPECIES_ELDEGOSS) { HP(120); MaxHP(200); Ability(ABILITY_COTTON_DOWN); UniqueAbility(ABILITY_SEEDCLOUD); Moves(MOVE_STUN_SPORE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STUN_SPORE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SEEDCLOUD);
    } THEN {
        EXPECT_EQ(player->hp, 170);
    }
}

SINGLE_BATTLE_TEST("Seedcloud does not restore HP after a non-powder move")
{
    GIVEN {
        PLAYER(SPECIES_ELDEGOSS) { HP(120); MaxHP(200); Ability(ABILITY_COTTON_DOWN); UniqueAbility(ABILITY_SEEDCLOUD); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SEEDCLOUD);
    } THEN {
        EXPECT_EQ(player->hp, 120);
    }
}
