#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sticky Hold has a 50 percent chance to lower attackers' Speed on contact")
{
    PASSES_RANDOMLY(1, 2, RNG_STICKY_HOLD);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_STICKY_HOLD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ABILITY_POPUP(player, ABILITY_STICKY_HOLD);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("Foe Wobbuffet's Speed fell!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Sticky Hold can fail to lower attackers' Speed on contact")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_STICKY_HOLD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_STICKY_HOLD, FALSE)); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STICKY_HOLD);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
            MESSAGE("Foe Wobbuffet's Speed fell!");
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sticky Hold does not lower attackers' Speed on non-contact")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power > 0);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        PLAYER(SPECIES_GRIMER) { Ability(ABILITY_STICKY_HOLD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWIFT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWIFT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STICKY_HOLD);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
            MESSAGE("Foe Wobbuffet's Speed fell!");
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sticky Hold blocks Klutz item removal")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SWIFT].power > 0);
        ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_STICKY_HOLD); Item(ITEM_POTION); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        ABILITY_POPUP(opponent, ABILITY_STICKY_HOLD);
        MESSAGE("Foe Wobbuffet's Sticky Hold made Swift ineffective!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_KLUTZ);
            MESSAGE("Wobbuffet knocked off Foe Wobbuffet's Potion!");
            MESSAGE("Wobbuffet's Speed fell!");
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_POTION);
    }
}
