#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Lightning Rod raises Attack or Sp. Atk depending on the higher attacking stat")
{
    u32 species;
    u32 boostedStat;

    PARAMETRIZE { species = SPECIES_MACHAMP; boostedStat = STAT_ATK; }
    PARAMETRIZE { species = SPECIES_ALAKAZAM; boostedStat = STAT_SPATK; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].type == TYPE_ELECTRIC);
        ASSUME(gSpeciesInfo[SPECIES_MACHAMP].baseAttack > gSpeciesInfo[SPECIES_MACHAMP].baseSpAttack);
        ASSUME(gSpeciesInfo[SPECIES_ALAKAZAM].baseSpAttack > gSpeciesInfo[SPECIES_ALAKAZAM].baseAttack);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER_WAVE); }
        OPPONENT(species) { Ability(ABILITY_LIGHTNING_ROD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } FINALLY {
        EXPECT_EQ(opponent->statStages[boostedStat], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[boostedStat == STAT_ATK ? STAT_SPATK : STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
