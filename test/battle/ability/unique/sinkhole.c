#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gSpeciesInfo[SPECIES_PIDGEY].types[0] == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_PIDGEY].types[1] == TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Sinkhole lowers grounded foe Speed on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_ARENA_TRAP); UniqueAbility(ABILITY_SINKHOLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Sinkhole only affects grounded foes")
{
    u16 species;
    u16 item;
    bool8 shouldDrop;
    PARAMETRIZE { species = SPECIES_PIDGEY; item = ITEM_NONE; shouldDrop = FALSE; }
    PARAMETRIZE { species = SPECIES_PIDGEY; item = ITEM_IRON_BALL; shouldDrop = TRUE; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; item = ITEM_NONE; shouldDrop = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_ARENA_TRAP); UniqueAbility(ABILITY_SINKHOLE); }
        OPPONENT(species) { Item(item); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE, shouldDrop);
    }
}

DOUBLE_BATTLE_TEST("Sinkhole affects all grounded opposing Pokemon in doubles")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_ARENA_TRAP); UniqueAbility(ABILITY_SINKHOLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIDGEY) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            SWITCH(playerRight, 2);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
