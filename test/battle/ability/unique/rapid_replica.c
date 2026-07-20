#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRANSFORM].effect == EFFECT_TRANSFORM);
}

SINGLE_BATTLE_TEST("Rapid Replica raises Speed after Transform and then copies the target's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_DITTO) { Ability(ABILITY_LIMBER); Moves(MOVE_TRANSFORM); }
        OPPONENT(SPECIES_CLAYDOL) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRANSFORM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_ANCIENT_IDOL);
    }
}

SINGLE_BATTLE_TEST("Rapid Replica raises Speed only once per battle with Imposter")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DITTO) { Ability(ABILITY_IMPOSTER); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 0); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(gBattleStruct->transformationAbilityUsed[B_SIDE_OPPONENT] & gBitTable[0]);
    }
}
