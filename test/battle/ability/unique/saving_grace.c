#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Saving Grace saves its grounded user from fainting in Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) {
            Speed(100);
            Ability(ABILITY_HEALER);
            UniqueAbility(ABILITY_SAVING_GRACE);
            HP(1);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_WOBBUFFET) { Speed(25); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_MISTY_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SAVING_GRACE);
        MESSAGE("The mist disappeared from the battlefield.");
        NOT MESSAGE("Audino fainted!");
    } THEN {
        EXPECT_EQ(player->hp, 1);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

DOUBLE_BATTLE_TEST("Saving Grace saves a grounded ally from fainting in Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) {
            Ability(ABILITY_HEALER);
            UniqueAbility(ABILITY_SAVING_GRACE);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MISTY_TERRAIN, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_MISTY_TERRAIN);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_TACKLE, target: playerRight);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_SAVING_GRACE);
        NOT MESSAGE("Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(playerRight->hp, 1);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}

DOUBLE_BATTLE_TEST("Saving Grace cannot save an airborne ally")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) {
            Ability(ABILITY_HEALER);
            UniqueAbility(ABILITY_SAVING_GRACE);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_PIDGEOT) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MISTY_TERRAIN, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_MISTY_TERRAIN);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(opponentLeft, MOVE_TACKLE, target: playerRight);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NOT ABILITY_POPUP(playerLeft, ABILITY_SAVING_GRACE);
        MESSAGE("Pidgeot fainted!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Saving Grace only saves an ally once per battle")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) {
            Speed(100);
            Ability(ABILITY_HEALER);
            UniqueAbility(ABILITY_SAVING_GRACE);
            HP(1);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_WOBBUFFET) { Speed(25); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_MISTY_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(opponent, MOVE_MISTY_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SAVING_GRACE);
        NOT ABILITY_POPUP(player, ABILITY_SAVING_GRACE);
        MESSAGE("Audino fainted!");
    }
}
