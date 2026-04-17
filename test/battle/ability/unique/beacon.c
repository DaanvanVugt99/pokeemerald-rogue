#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].pulseMove);
    ASSUME(!gBattleMoves[MOVE_THUNDERBOLT].pulseMove);
}

SINGLE_BATTLE_TEST("Beacon lowers the target's Sp. Def by 1 after a pulse move hits")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_BEACON); Moves(MOVE_DRAGON_PULSE); }
        OPPONENT(SPECIES_BLISSEY);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_PULSE); }
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Beacon does not trigger for non-pulse moves")
{
    GIVEN {
        PLAYER(SPECIES_AMPHAROS) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_BEACON); Moves(MOVE_THUNDERBOLT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BEACON);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
            MESSAGE("Foe Wobbuffet's Sp. Def fell!");
        }
    }
}
