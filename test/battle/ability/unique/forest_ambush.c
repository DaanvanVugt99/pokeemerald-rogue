#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Forest Ambush lowers Speed after the first status move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SHIFTRY) { Speed(100); Ability(ABILITY_CHLOROPHYLL); Moves(MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ABILITY_POPUP(player, ABILITY_FOREST_AMBUSH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Forest Ambush only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SHIFTRY) { Speed(100); Ability(ABILITY_CHLOROPHYLL); Moves(MOVE_TAIL_WHIP, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Forest Ambush also flinches during Eclipse or on Grassy Terrain")
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_ECLIPSE; }
    PARAMETRIZE { setupMove = MOVE_GRASSY_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_SHIFTRY) { Speed(100); Ability(ABILITY_CHLOROPHYLL); Moves(MOVE_TACKLE, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ABILITY_POPUP(player, ABILITY_FOREST_AMBUSH);
        MESSAGE("Foe Wobbuffet flinched!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Forest Ambush's Eclipse flinch is treated as an ability effect")
{
    GIVEN {
        PLAYER(SPECIES_SHIFTRY) { Speed(100); Ability(ABILITY_CHLOROPHYLL); Moves(MOVE_TACKLE, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_DUSTOX) { Speed(50); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECLIPSE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ABILITY_POPUP(player, ABILITY_FOREST_AMBUSH);
        MESSAGE("Foe Dustox flinched!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
