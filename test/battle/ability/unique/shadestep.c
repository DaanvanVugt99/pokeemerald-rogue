#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].effect == EFFECT_HIT);
}

SINGLE_BATTLE_TEST("Shadestep lowers Speed on the first hit while terrain is active")
{
    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHADESTEP); Moves(MOVE_GRASSY_TERRAIN, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SHADESTEP);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Shadestep does not trigger without terrain")
{
    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHADESTEP); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHADESTEP);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shadestep only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GRENINJA) { Speed(100); Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_SHADESTEP); Moves(MOVE_GRASSY_TERRAIN, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHADESTEP);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHADESTEP);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
