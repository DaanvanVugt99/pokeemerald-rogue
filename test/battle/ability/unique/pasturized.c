#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MILK_DRINK].effect == EFFECT_SOFTBOILED);
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
}

SINGLE_BATTLE_TEST("Pasturized sets Plain Terrain when Miltank uses Milk Drink")
{
    GIVEN {
        PLAYER(SPECIES_MILTANK) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_PASTURIZED); Moves(MOVE_MILK_DRINK); HP(50); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MILK_DRINK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
    }
}

DOUBLE_BATTLE_TEST("Pasturized heals friendly Pokemon in Plain Terrain at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_MILTANK) { Ability(ABILITY_SCRAPPY); UniqueAbility(ABILITY_PASTURIZED); Moves(MOVE_MILK_DRINK); HP(50); MaxHP(100); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); HP(50); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); HP(50); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); HP(50); MaxHP(100); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_MILK_DRINK);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 100);
        EXPECT_EQ(playerRight->hp, 56);
        EXPECT_EQ(opponentLeft->hp, 50);
        EXPECT_EQ(opponentRight->hp, 50);
    }
}
