#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(!IS_MOVE_STATUS(MOVE_HYPER_VOICE));
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
}

SINGLE_BATTLE_TEST("Distortion's damaging sound moves always taunt or confuse")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE((opponent->status2 & STATUS2_CONFUSION) || gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer > 0, 0);
    }
}

SINGLE_BATTLE_TEST("Distortion confuses 50 percent of the time when taunt is blocked")
{
    PASSES_RANDOMLY(1, 2, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OBLIVIOUS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISTORTION);
        MESSAGE("Foe Wobbuffet became confused!");
    } THEN {
        EXPECT_NE(opponent->status2 & STATUS2_CONFUSION, 0);
        EXPECT_EQ((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Distortion taunts 50 percent of the time when confusion is blocked")
{
    PASSES_RANDOMLY(1, 2, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
        EXPECT_EQ(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Distortion does not trigger for non-sound damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DISTORTION);
            MESSAGE("Foe Wobbuffet became confused!");
            MESSAGE("Foe Wobbuffet fell for the taunt!");
        }
    } THEN {
        EXPECT_EQ((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
        EXPECT_EQ(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Distortion's taunt branch respects Aroma Veil")
{
    PASSES_RANDOMLY(1, 2, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_MISTY_TERRAIN, MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_AROMATISSE) { Ability(ABILITY_AROMA_VEIL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MISTY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DISTORTION);
        MESSAGE("Foe Aromatisse is protected by an aromatic veil!");
    } THEN {
        EXPECT_EQ((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
        EXPECT_EQ(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}
