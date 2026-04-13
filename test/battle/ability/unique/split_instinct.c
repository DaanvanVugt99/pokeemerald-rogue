#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Split Instinct can raise Speed by 1 at end of turn")
{
    PASSES_RANDOMLY(1, 3, RNG_ROGUE_SPLIT_INSTINCT);
    GIVEN {
        PLAYER(SPECIES_DODRIO) { HP(160); MaxHP(160); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_SPLIT_INSTINCT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->statStages[STAT_SPEED] > DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Split Instinct can heal 1/8 max HP at end of turn")
{
    PASSES_RANDOMLY(1, 3, RNG_ROGUE_SPLIT_INSTINCT);
    GIVEN {
        PLAYER(SPECIES_DODRIO) { HP(40); MaxHP(160); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_SPLIT_INSTINCT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 60);
    }
}

SINGLE_BATTLE_TEST("Split Instinct lowers an opposing stat on its debuff roll")
{
    PASSES_RANDOMLY(1, 3, RNG_ROGUE_SPLIT_INSTINCT);
    GIVEN {
        PLAYER(SPECIES_DODRIO) { HP(100); MaxHP(160); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_SPLIT_INSTINCT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->statStages[STAT_DEF] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_ATK] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_SPATK] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_SPDEF] < DEFAULT_STAT_STAGE
            || opponent->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE);
    }
}
