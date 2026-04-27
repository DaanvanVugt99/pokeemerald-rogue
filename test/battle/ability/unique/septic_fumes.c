#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POISON_GAS].effect == EFFECT_POISON);
    ASSUME(gBattleMoves[MOVE_POISON_GAS].accuracy != 0);
}

SINGLE_BATTLE_TEST("Septic Fumes lowers Attack and poisons a random opposing party member when poisoning a foe")
{
    GIVEN {
        PLAYER(SPECIES_SKUNTANK) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_SEPTIC_FUMES); Moves(MOVE_POISON_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT(GetMonData(&gEnemyParty[1], MON_DATA_STATUS) & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Septic Fumes does not activate when poison fails")
{
    GIVEN {
        PLAYER(SPECIES_SKUNTANK) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_SEPTIC_FUMES); Moves(MOVE_POISON_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { Status1(STATUS1_POISON); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SEPTIC_FUMES);
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(GetMonData(&gEnemyParty[1], MON_DATA_STATUS), 0);
    }
}
