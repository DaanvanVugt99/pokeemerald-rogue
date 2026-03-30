#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_KARATE_CHOP].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Soft Body blocks Fighting-type moves only")
{
    u32 move;
    bool8 shouldBlock;
    PARAMETRIZE { move = MOVE_KARATE_CHOP; shouldBlock = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldBlock = FALSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_JIGGLYPUFF) { Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SOFT_BODY); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(opponent->hp == opponent->maxHP, shouldBlock);
    }
}
