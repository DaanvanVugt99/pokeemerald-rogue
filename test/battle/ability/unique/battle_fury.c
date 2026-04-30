#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Battle Fury raises Attack and critical-hit ratio when dropping below half HP")
{
    GIVEN {
        PLAYER(SPECIES_BOUFFALANT) { Ability(ABILITY_RECKLESS); UniqueAbility(ABILITY_BATTLE_FURY); HP(100); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BATTLE_FURY);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Bouffalant's Battle Fury raised its Attack!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Bouffalant is getting pumped!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(gBattleStruct->bonusCritStages[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], 1);
    }
}

SINGLE_BATTLE_TEST("Battle Fury does not trigger if the user stays above half HP")
{
    GIVEN {
        PLAYER(SPECIES_BOUFFALANT) { Ability(ABILITY_RECKLESS); UniqueAbility(ABILITY_BATTLE_FURY); HP(130); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BATTLE_FURY);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gBattleStruct->bonusCritStages[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], 0);
    }
}
