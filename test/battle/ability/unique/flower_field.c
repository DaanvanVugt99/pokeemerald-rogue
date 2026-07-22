#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Flower Field requires one turn out and a status move")
{
    GIVEN {
        PLAYER(SPECIES_CHIKORITA)   { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_FLOWER_FIELD); Moves(MOVE_GROWL, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FLOWER_FIELD);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY));
    }
}

SINGLE_BATTLE_TEST("Flower Field can set every terrain after turn 1")
{
    u32 terrainRoll;
    u32 terrainStatus;

    PARAMETRIZE { terrainRoll = 0; terrainStatus = STATUS_FIELD_PLAIN_TERRAIN; }
    PARAMETRIZE { terrainRoll = 1; terrainStatus = STATUS_FIELD_GRASSY_TERRAIN; }
    PARAMETRIZE { terrainRoll = 2; terrainStatus = STATUS_FIELD_ELECTRIC_TERRAIN; }
    PARAMETRIZE { terrainRoll = 3; terrainStatus = STATUS_FIELD_PSYCHIC_TERRAIN; }
    PARAMETRIZE { terrainRoll = 4; terrainStatus = STATUS_FIELD_MISTY_TERRAIN; }
    PARAMETRIZE { terrainRoll = 5; terrainStatus = STATUS_FIELD_INFESTED_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_MEGANIUM)    { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_FLOWER_FIELD); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_FLOWER_FIELD, terrainRoll)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOWER_FIELD);
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, terrainStatus);
    }
}

SINGLE_BATTLE_TEST("Flower Field excludes the active terrain from later rolls")
{
    GIVEN {
        PLAYER(SPECIES_MEGANIUM)    { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_FLOWER_FIELD); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_FLOWER_FIELD, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_FLOWER_FIELD, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOWER_FIELD);
        ABILITY_POPUP(player, ABILITY_FLOWER_FIELD);
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, STATUS_FIELD_GRASSY_TERRAIN);
    }
}
