#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_METRONOME].effect == EFFECT_METRONOME);
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Storyboard fails cleanly if there is no previous move to copy")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Ability(ABILITY_OWN_TEMPO); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_STORYBOARD);
        MESSAGE("Smeargle used Copycat!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Storyboard only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Speed(1); Ability(ABILITY_OWN_TEMPO); Moves(MOVE_GROWL, MOVE_TAIL_WHIP, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_TAIL_WHIP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_TAIL_WHIP); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_TAIL_WHIP); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 3);
    }
}

SINGLE_BATTLE_TEST("Storyboard Copycat uses the move from before the triggering status move")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Ability(ABILITY_OWN_TEMPO); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TAIL_WHIP, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_GROWL); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Tail Whip!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_STORYBOARD);
        MESSAGE("Smeargle used Copycat!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Storyboard uses the pre-trigger move even when the triggering status move is called")
{
    GIVEN {
        PLAYER(SPECIES_SMEARGLE) { Speed(1); Ability(ABILITY_OWN_TEMPO); Moves(MOVE_METRONOME); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Moves(MOVE_TAIL_WHIP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_METRONOME, WITH_RNG(RNG_METRONOME, MOVE_GROWL)); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}
