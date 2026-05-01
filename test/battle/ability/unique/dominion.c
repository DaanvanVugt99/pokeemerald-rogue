#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Dominion summons sandstorm on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_DOMINION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
    }
}

SINGLE_BATTLE_TEST("Dominion makes the last remaining Pokemon immune to status conditions")
{
    u16 move;

    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    PARAMETRIZE { move = MOVE_SING; }

    GIVEN {
        PLAYER(SPECIES_TYRANITAR) { Ability(ABILITY_SAND_STREAM); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DOMINION);
        if (move == MOVE_TOXIC) {
            MESSAGE("Tyranitar's Dominion prevents poisoning!");
        } else if (move == MOVE_WILL_O_WISP) {
            MESSAGE("Tyranitar's Dominion prevents burns!");
        } else if (move == MOVE_THUNDER_WAVE) {
            MESSAGE("Tyranitar's Dominion prevents paralysis!");
        } else {
            MESSAGE("It doesn't affect Tyranitar…");
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Dominion does not give status immunity while another party Pokemon is still alive")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_DOMINION); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC); }
    } THEN {
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Dominion prevents Toxic Spikes from poisoning the last remaining Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_DOMINION); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC_SPIKES); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(opponent, 1); }
        TURN {}
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, player);
            STATUS_ICON(player, poison: TRUE);
            STATUS_ICON(player, badPoison: TRUE);
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Dominion prevents frostbite on the last remaining Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_DOMINION); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_POWDER_SNOW); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_POWDER_SNOW, WITH_RNG(RNG_FROZEN, TRUE)); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_FRZ, player);
            STATUS_ICON(player, freeze: TRUE);
            STATUS_ICON(player, frostbite: TRUE);
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
