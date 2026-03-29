#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Gnaw Down lowers Defense with biting moves only")
{
    u16 move;
    bool8 shouldDropDef;
    PARAMETRIZE { move = MOVE_BITE; shouldDropDef = TRUE; }
    PARAMETRIZE { move = MOVE_TACKLE; shouldDropDef = FALSE; }
    GIVEN {
        PLAYER(SPECIES_RATICATE) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_GNAW_DOWN); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - shouldDropDef);
    }
}

SINGLE_BATTLE_TEST("Gnaw Down can trigger alongside other move-end attacker abilities")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
        ASSUME(gBattleMoves[MOVE_BITE].power > 0);
        PLAYER(SPECIES_RATICATE) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_GNAW_DOWN); Speed(100); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); MaxHP(999); HP(999); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_STENCH, TRUE)); MOVE(opponent, MOVE_AERIAL_ACE); }
    } THEN {
        EXPECT_GT(opponent->hp, 0);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
