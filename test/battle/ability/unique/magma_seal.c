#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
}

SINGLE_BATTLE_TEST("Magma Seal traps the target when the user hits with a Fire-type move")
{
    GIVEN {
        PLAYER(SPECIES_HEATRAN) { Speed(100); Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_MAGMA_SEAL); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MAGMA_SEAL);
    } THEN {
        EXPECT_NE(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].battlerPreventingEscape,
                  GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Magma Seal does not trap for non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_HEATRAN) { Speed(100); Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_MAGMA_SEAL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_MAGMA_SEAL);
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
    }
}

SINGLE_BATTLE_TEST("Magma Seal is assigned to Heatran")
{
    GIVEN {
        PLAYER(SPECIES_HEATRAN) { Ability(ABILITY_FLASH_FIRE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_HEATRAN), ABILITY_MAGMA_SEAL);
    }
}
