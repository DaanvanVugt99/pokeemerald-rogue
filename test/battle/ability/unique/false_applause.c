#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
    ASSUME(gBattleMoves[MOVE_SWIFT].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("False Applause torments targets hit by resisted contact moves")
{
    u16 move;
    u16 targetSpecies;
    bool32 shouldTorment;

    PARAMETRIZE { move = MOVE_TACKLE; targetSpecies = SPECIES_ARON; shouldTorment = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; targetSpecies = SPECIES_WOBBUFFET; shouldTorment = FALSE; }
    PARAMETRIZE { move = MOVE_SWIFT; targetSpecies = SPECIES_ARON; shouldTorment = FALSE; }

    GIVEN {
        PLAYER(SPECIES_MEOWSCARADA) { Ability(ABILITY_OVERGROW); Moves(move); }
        OPPONENT(targetSpecies) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (shouldTorment)
        {
            ABILITY_POPUP(player, ABILITY_FALSE_APPLAUSE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TORMENT, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_FALSE_APPLAUSE);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_TORMENT, player);
            }
        }
    } THEN {
        if (shouldTorment)
        {
            EXPECT(opponent->status2 & STATUS2_TORMENT);
            EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tormentTimer, PERMANENT_TORMENT);
        }
        else
        {
            EXPECT_EQ(opponent->status2 & STATUS2_TORMENT, 0);
        }
    }
}
