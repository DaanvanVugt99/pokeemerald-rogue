#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_KARATE_CHOP].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_SING].accuracy == 55);
}

SINGLE_BATTLE_TEST("Soothing Voice blocks Fighting-type moves only")
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

SINGLE_BATTLE_TEST("Soothing Voice makes Sing 90 percent accurate")
{
    PASSES_RANDOMLY(90, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_JIGGLYPUFF) { Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SOFT_BODY); Moves(MOVE_SING); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(opponent, sleep: TRUE);
    }
}
