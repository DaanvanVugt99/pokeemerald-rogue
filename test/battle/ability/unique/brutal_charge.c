#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power == 40);
    ASSUME(gBattleMoves[MOVE_ABSORB].priority == 0);
}

SINGLE_BATTLE_TEST("Brutal Charge gives 2x Speed on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Speed(50); Ability(ABILITY_QUICK_FEET); UniqueAbility(ABILITY_BRUTAL_CHARGE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_ABSORB); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_ABSORB); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Brutal Charge gives 1.5x Attack on the first turn out")
{
    s16 turnOneDamage;
    s16 turnTwoDamage;

    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Ability(ABILITY_QUICK_FEET); UniqueAbility(ABILITY_BRUTAL_CHARGE); Moves(MOVE_TACKLE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &turnOneDamage);
        HP_BAR(opponent, captureDamage: &turnTwoDamage);
    } THEN {
        EXPECT_MUL_EQ(turnTwoDamage, UQ_4_12(1.5), turnOneDamage);
    }
}
