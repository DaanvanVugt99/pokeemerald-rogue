#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Low Tide grants Water immunity, triggers Mud Shot, and keeps Water Compaction")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_MUD_SHOT].effect == EFFECT_SPEED_DOWN_HIT);
        PLAYER(SPECIES_PALOSSAND) { Ability(ABILITY_WATER_COMPACTION); UniqueAbility(ABILITY_LOW_TIDE); HP(200); MaxHP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MACHAMP) { Ability(ABILITY_NO_GUARD); Speed(50); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 3);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
