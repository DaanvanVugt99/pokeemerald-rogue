#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AQUA_STEP].danceMove);
    ASSUME(gBattleMoves[MOVE_AQUA_STEP].type == TYPE_WATER);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].danceMove);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Showstopper lowers Speed after resisted dance moves")
{
    u16 move;
    u16 targetSpecies;
    bool32 shouldLowerSpeed;

    PARAMETRIZE { move = MOVE_AQUA_STEP; targetSpecies = SPECIES_LOTAD; shouldLowerSpeed = TRUE; }
    PARAMETRIZE { move = MOVE_AQUA_STEP; targetSpecies = SPECIES_WOBBUFFET; shouldLowerSpeed = FALSE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; targetSpecies = SPECIES_LOTAD; shouldLowerSpeed = FALSE; }

    GIVEN {
        PLAYER(SPECIES_QUAQUAVAL) { Ability(ABILITY_TORRENT); Moves(move); }
        OPPONENT(targetSpecies) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (shouldLowerSpeed)
            ABILITY_POPUP(player, ABILITY_SHOWSTOPPER);
        else
            NOT ABILITY_POPUP(player, ABILITY_SHOWSTOPPER);
    } THEN {
        if (shouldLowerSpeed)
            EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
        else
            EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
