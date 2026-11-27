#include "global.h"
#include "battle_util.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Adventure balance: unavailable move selection comparison")
{
    u16 move;
    bool32 blocked;
    PARAMETRIZE { move = MOVE_FAKE_OUT; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_FIRST_IMPRESSION; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_SPIT_UP; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_BURN_UP; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_DOUBLE_SHOCK; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_LAST_RESORT; blocked = FALSE; }
    PARAMETRIZE { move = MOVE_BELCH; blocked = TRUE; }
    PARAMETRIZE { move = MOVE_STUFF_CHEEKS; blocked = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(move, MOVE_CELEBRATE, MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(CheckMoveLimitations(B_POSITION_PLAYER_LEFT, 0, MOVE_LIMITATIONS_ALL) & 1, blocked);
    }
}

SINGLE_BATTLE_TEST("Adventure balance: contact KO effect timing comparison")
{
    u16 move = MOVE_NONE, defenderAbility = ABILITY_NONE;
    static const u16 moves[] = {
        MOVE_KNOCK_OFF, MOVE_THIEF, MOVE_RAPID_SPIN, MOVE_ICE_SPINNER,
        MOVE_CEASELESS_EDGE, MOVE_STONE_AXE, MOVE_SCRATCH,
    };
    u32 index;
    for (index = 0; index < ARRAY_COUNT(moves); index++) {
        PARAMETRIZE { move = moves[index]; defenderAbility = ABILITY_ROUGH_SKIN; }
        PARAMETRIZE { move = moves[index]; defenderAbility = ABILITY_IRON_BARBS; }
    }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Ability(move == MOVE_SCRATCH ? ABILITY_MAGICIAN : ABILITY_NONE); }
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(defenderAbility); Item(ITEM_METAL_COAT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, move); SEND_OUT(player, 1); }
    } THEN {
        EXPECT_EQ(opponent->item, (move == MOVE_KNOCK_OFF || move == MOVE_THIEF) ? ITEM_NONE : ITEM_METAL_COAT);
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK), move != MOVE_RAPID_SPIN);
        EXPECT_EQ(!!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN), move != MOVE_ICE_SPINNER);
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_SPIKES), move == MOVE_CEASELESS_EDGE);
        EXPECT_EQ(!!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK), move == MOVE_STONE_AXE);
    }
}
