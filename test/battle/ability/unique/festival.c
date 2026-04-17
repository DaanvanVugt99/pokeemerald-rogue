#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUIVER_DANCE].danceMove);
    ASSUME(!gBattleMoves[MOVE_GIGA_DRAIN].danceMove);
}

SINGLE_BATTLE_TEST("Festival sets Grassy Terrain when Bellossom uses a dance move")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_FESTIVAL); Moves(MOVE_QUIVER_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_QUIVER_DANCE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Festival does not set Grassy Terrain for non-dance moves")
{
    GIVEN {
        PLAYER(SPECIES_BELLOSSOM) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_FESTIVAL); Moves(MOVE_GIGA_DRAIN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GIGA_DRAIN); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN));
    }
}
