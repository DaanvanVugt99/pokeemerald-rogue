#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Royal Garden sets Grassy Terrain after knocking out a target")
{
    GIVEN {
        PLAYER(SPECIES_TSAREENA) { Ability(ABILITY_LEAF_GUARD); UniqueAbility(ABILITY_ROYAL_GARDEN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_METAPOD) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROYAL_GARDEN);
        MESSAGE("Grass grew to cover\nthe battlefield!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Royal Garden lowers attackers' Speed on contact in Grassy Terrain")
{
    GIVEN {
        PLAYER(SPECIES_TSAREENA) { Ability(ABILITY_LEAF_GUARD); UniqueAbility(ABILITY_ROYAL_GARDEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GRASSY_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_ROYAL_GARDEN);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("Foe Wobbuffet's Speed fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Royal Garden does not lower Speed from non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_TSAREENA) { Ability(ABILITY_LEAF_GUARD); UniqueAbility(ABILITY_ROYAL_GARDEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_GRASSY_TERRAIN, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ROYAL_GARDEN);
            MESSAGE("Foe Wobbuffet's Speed fell!");
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
