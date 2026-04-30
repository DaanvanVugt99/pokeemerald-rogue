#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Restorative Aura heals all active battlers by 1/8 max HP at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) { Ability(ABILITY_HEALER); UniqueAbility(ABILITY_RESTORATIVE_AURA); HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 62);
        EXPECT_EQ(opponent->hp, 62);
    }
}

DOUBLE_BATTLE_TEST("Restorative Aura heals all four battlers at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) { Ability(ABILITY_HEALER); UniqueAbility(ABILITY_RESTORATIVE_AURA); HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 62);
        EXPECT_EQ(playerRight->hp, 62);
        EXPECT_EQ(opponentLeft->hp, 62);
        EXPECT_EQ(opponentRight->hp, 62);
    }
}
