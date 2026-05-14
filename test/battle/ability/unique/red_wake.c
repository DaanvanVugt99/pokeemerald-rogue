#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].power > 0);
}

SINGLE_BATTLE_TEST("Red Wake raises Speed after dealing super effective damage")
{
    GIVEN {
        PLAYER(SPECIES_BARRASKEWDA) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_RED_WAKE); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_GEODUDE) { Ability(ABILITY_ROCK_HEAD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Red Wake does not raise Speed when damage is not super effective")
{
    GIVEN {
        PLAYER(SPECIES_BARRASKEWDA) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_RED_WAKE); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_RED_WAKE);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
