#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Tufted Away sets Tailwind for 2 turns after using a status move")
{
    GIVEN {
        PLAYER(SPECIES_WHIMSICOTT) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_TUFTED_AWAY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TUFTED_AWAY);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 1);
    }
}

SINGLE_BATTLE_TEST("Tufted Away sets Tailwind after using Cotton Guard")
{
    GIVEN {
        PLAYER(SPECIES_WHIMSICOTT) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_TUFTED_AWAY); Moves(MOVE_COTTON_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COTTON_GUARD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COTTON_GUARD, player);
        ABILITY_POPUP(player, ABILITY_TUFTED_AWAY);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}

SINGLE_BATTLE_TEST("Tufted Away sets Tailwind after using Leech Seed")
{
    GIVEN {
        PLAYER(SPECIES_WHIMSICOTT) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_TUFTED_AWAY); Moves(MOVE_LEECH_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEECH_SEED); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
        ABILITY_POPUP(player, ABILITY_TUFTED_AWAY);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Tufted Away can refresh Tailwind with another status move")
{
    GIVEN {
        PLAYER(SPECIES_WHIMSICOTT) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_TUFTED_AWAY); Moves(MOVE_CELEBRATE, MOVE_COTTON_GUARD); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_COTTON_GUARD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TUFTED_AWAY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COTTON_GUARD, player);
        ABILITY_POPUP(player, ABILITY_TUFTED_AWAY);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 1);
    }
}

SINGLE_BATTLE_TEST("Tufted Away does not trigger after a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_WHIMSICOTT) { Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_TUFTED_AWAY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TUFTED_AWAY);
            MESSAGE("Whimsicott's tailwind blew from behind!");
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}
