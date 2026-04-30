#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Livewire Nest sets Infested Terrain on the first Bug move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GALVANTULA) { Ability(ABILITY_COMPOUND_EYES); Moves(MOVE_BUG_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVEWIRE_NEST);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Livewire Nest does not trigger a second time in the same switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GALVANTULA) { Ability(ABILITY_COMPOUND_EYES); Moves(MOVE_BUG_BITE, MOVE_CELEBRATE, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVEWIRE_NEST);
        NOT ABILITY_POPUP(player, ABILITY_LIVEWIRE_NEST);
    }
}

SINGLE_BATTLE_TEST("Livewire Nest is not consumed when a qualifying move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_GALVANTULA) { Ability(ABILITY_COMPOUND_EYES); Moves(MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_SANDSHREW) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_INFESTED_TERRAIN));
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
