#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect != EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].effect != EFFECT_SLEEP);
}

SINGLE_BATTLE_TEST("Sleep Dust triggers only on contact moves")
{
    u16 move;
    bool8 shouldSleep;
    PARAMETRIZE { move = MOVE_TACKLE; shouldSleep = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; shouldSleep = FALSE; }
    GIVEN {
        PLAYER(SPECIES_BUTTERFREE) { Ability(ABILITY_COMPOUND_EYES); UniqueAbility(ABILITY_SLEEP_DUST); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(!!(opponent->status1 & STATUS1_SLEEP), shouldSleep);
    }
}

SINGLE_BATTLE_TEST("Sleep Dust can trigger alongside other move-end defender abilities")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
        ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BUTTERFREE) { Ability(ABILITY_TOXIC_DEBRIS); UniqueAbility(ABILITY_SLEEP_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TOXIC_SPIKES);
        EXPECT(player->status1 & STATUS1_SLEEP);
    }
}
