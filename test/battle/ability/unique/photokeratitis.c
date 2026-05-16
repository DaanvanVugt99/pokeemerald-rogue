#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Photokeratitis lowers accuracy after contact moves in Snow")
{
    u16 setupMove;
    u16 attackMove;
    bool32 shouldLowerAccuracy;

    PARAMETRIZE { setupMove = MOVE_SNOWSCAPE; attackMove = MOVE_TACKLE; shouldLowerAccuracy = TRUE; }
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; attackMove = MOVE_TACKLE; shouldLowerAccuracy = FALSE; }
    PARAMETRIZE { setupMove = MOVE_SNOWSCAPE; attackMove = MOVE_WATER_GUN; shouldLowerAccuracy = FALSE; }

    GIVEN {
        PLAYER(SPECIES_CETITAN) { Speed(100); Ability(ABILITY_THICK_FAT); Moves(MOVE_SNOWSCAPE, MOVE_CELEBRATE, MOVE_TACKLE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, attackMove); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, attackMove, player);
        HP_BAR(opponent);
        if (shouldLowerAccuracy)
        {
            ABILITY_POPUP(player, ABILITY_PHOTOKERATITIS);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        }
        else
        {
            NOT ABILITY_POPUP(player, ABILITY_PHOTOKERATITIS);
        }
    } THEN {
        if (shouldLowerAccuracy)
            EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
        else
            EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}
