#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].power > 0);
}

SINGLE_BATTLE_TEST("Cheap Tactics uses Scratch immediately on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RATTATA_ALOLAN) { Ability(ABILITY_GLUTTONY); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->hp < player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Cheap Tactics coexists with primary switch-in abilities")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_CHEAP_TACTICS); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->hp < player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}
