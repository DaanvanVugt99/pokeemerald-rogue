#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_EMBER].effect == EFFECT_BURN_HIT);
    ASSUME(gBattleMoves[MOVE_EMBER].secondaryEffectChance == 10);
    ASSUME(gBattleMoves[MOVE_SCALD].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SCALD].effect == EFFECT_BURN_HIT);
    ASSUME(gBattleMoves[MOVE_SCALD].secondaryEffectChance == 30);
}

SINGLE_BATTLE_TEST("Pyromancy makes Fire-type burn chances 5x higher")
{
    PASSES_RANDOMLY(50, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_CHANDELURE) { Ability(ABILITY_FLASH_FIRE); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_BURN, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Pyromancy does not boost non-Fire burn chances")
{
    PASSES_RANDOMLY(30, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_CHANDELURE) { Ability(ABILITY_FLASH_FIRE); Moves(MOVE_SCALD); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(500); HP(500); SpDefense(255); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCALD); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_BURN, STATUS1_BURN);
    }
}
