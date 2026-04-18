#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Battle Trance uses Bulk Up after landing a critical hit")
{
    GIVEN {
        PLAYER(SPECIES_HERACROSS) { Ability(ABILITY_GUTS); UniqueAbility(ABILITY_BATTLE_TRANCE); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK, criticalHit: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Battle Trance does not trigger without a critical hit")
{
    GIVEN {
        PLAYER(SPECIES_HERACROSS) { Ability(ABILITY_GUTS); UniqueAbility(ABILITY_BATTLE_TRANCE); Moves(MOVE_PECK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PECK, criticalHit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
