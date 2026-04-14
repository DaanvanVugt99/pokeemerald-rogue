#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ionize damages non-Electric and non-Ground Pokemon at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_JOLTEON) { Ability(ABILITY_VOLT_ABSORB); UniqueAbility(ABILITY_IONIZE); HP(320); MaxHP(320); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(320); MaxHP(320); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IONIZE);
        HP_BAR(opponent, damage: 20);
    } THEN {
        EXPECT_EQ(player->hp, 320);
        EXPECT_EQ(opponent->hp, 300);
    }
}

DOUBLE_BATTLE_TEST("Ionize does not damage Electric- or Ground-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_JOLTEON) { Ability(ABILITY_VOLT_ABSORB); UniqueAbility(ABILITY_IONIZE); HP(320); MaxHP(320); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(320); MaxHP(320); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { HP(320); MaxHP(320); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SANDSLASH) { HP(320); MaxHP(320); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 320);
        EXPECT_EQ(playerRight->hp, 300);
        EXPECT_EQ(opponentLeft->hp, 320);
        EXPECT_EQ(opponentRight->hp, 320);
    }
}

SINGLE_BATTLE_TEST("Ionize does not trigger if all battlers are Electric or Ground")
{
    GIVEN {
        PLAYER(SPECIES_JOLTEON) { Ability(ABILITY_VOLT_ABSORB); UniqueAbility(ABILITY_IONIZE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOLEM) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_IONIZE);
            MESSAGE("Golem was hurt by ionization!");
        }
    }
}
