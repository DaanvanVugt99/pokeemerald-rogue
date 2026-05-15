#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].type == TYPE_NORMAL);
    ASSUME(!gBattleMoves[MOVE_SWIFT].soundMove);
    ASSUME(gBattleMoves[MOVE_SWIFT].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Dirge makes targets drowsy after resisted sound moves")
{
    u16 move;
    u16 targetSpecies;
    bool32 shouldDrowse;

    PARAMETRIZE { move = MOVE_HYPER_VOICE; targetSpecies = SPECIES_ARON; shouldDrowse = TRUE; }
    PARAMETRIZE { move = MOVE_HYPER_VOICE; targetSpecies = SPECIES_WOBBUFFET; shouldDrowse = FALSE; }
    PARAMETRIZE { move = MOVE_SWIFT; targetSpecies = SPECIES_ARON; shouldDrowse = FALSE; }

    GIVEN {
        PLAYER(SPECIES_SKELEDIRGE) { Ability(ABILITY_BLAZE); Moves(move); }
        OPPONENT(targetSpecies) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (shouldDrowse)
            ABILITY_POPUP(player, ABILITY_DIRGE);
        else
            NOT ABILITY_POPUP(player, ABILITY_DIRGE);
    } THEN {
        u32 target = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        if (shouldDrowse)
            EXPECT(gStatuses3[target] & STATUS3_YAWN);
        else
            EXPECT_EQ(gStatuses3[target] & STATUS3_YAWN, 0);
    }
}
