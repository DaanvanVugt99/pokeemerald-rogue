#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_NASTY_PLOT].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_THIEF].effect == EFFECT_THIEF);
    ASSUME(gBattleMoves[MOVE_COVET].effect == EFFECT_THIEF);
    ASSUME(gBattleMoves[MOVE_SWITCHEROO].effect == EFFECT_TRICK);
    ASSUME(gBattleMoves[MOVE_TRICK].effect == EFFECT_TRICK);
    ASSUME(gBattleMoves[MOVE_TAUNT].effect == EFFECT_TAUNT);
    ASSUME(gBattleMoves[MOVE_TORMENT].effect == EFFECT_TORMENT);
    ASSUME(gBattleMoves[MOVE_FAKE_TEARS].effect == EFFECT_SPECIAL_DEFENSE_DOWN_2);
    ASSUME(gBattleMoves[MOVE_FLATTER].effect == EFFECT_FLATTER);
    ASSUME(gBattleMoves[MOVE_SNARL].effect == EFFECT_SPECIAL_ATTACK_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_EMBARGO].effect == EFFECT_EMBARGO);
    ASSUME(gBattleMoves[MOVE_NASTY_PLOT].effect == EFFECT_SPECIAL_ATTACK_UP_2);
}

SINGLE_BATTLE_TEST("Bag of Tricks uses a random trick move after the first Dark-type move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_THIEVUL) { Speed(100); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_BAG_OF_TRICKS); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_BAG_OF_TRICKS, MOVE_NASTY_PLOT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Bag of Tricks uses an opposing target after a Dark-type self-targeting move")
{
    GIVEN {
        PLAYER(SPECIES_THIEVUL) { Speed(100); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_BAG_OF_TRICKS); Moves(MOVE_NASTY_PLOT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_NASTY_PLOT, WITH_RNG(RNG_ROGUE_BAG_OF_TRICKS, MOVE_FAKE_TEARS)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, player);
        ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_TEARS, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Bag of Tricks only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_THIEVUL) { Speed(100); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_BAG_OF_TRICKS); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_BAG_OF_TRICKS, MOVE_NASTY_PLOT)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_BAG_OF_TRICKS, MOVE_FAKE_TEARS)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, player);
        NOT ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Bag of Tricks refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_THIEVUL) { Speed(100); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_BAG_OF_TRICKS); Moves(MOVE_BITE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_BAG_OF_TRICKS, MOVE_NASTY_PLOT)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_BAG_OF_TRICKS, MOVE_FAKE_TEARS)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, player);
        ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_TEARS, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Bag of Tricks chooses from every trick move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_THIEF,
        MOVE_COVET,
        MOVE_SWITCHEROO,
        MOVE_TRICK,
        MOVE_TAUNT,
        MOVE_TORMENT,
        MOVE_FAKE_TEARS,
        MOVE_FLATTER,
        MOVE_SNARL,
        MOVE_EMBARGO,
        MOVE_NASTY_PLOT,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_BAG_OF_TRICKS);

    GIVEN {
        PLAYER(SPECIES_THIEVUL) { Speed(100); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_BAG_OF_TRICKS); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BAG_OF_TRICKS);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
