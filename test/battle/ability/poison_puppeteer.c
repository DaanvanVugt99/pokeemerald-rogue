#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Poison Puppeteer confuses after a damaging move poisons the target")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SLUDGE_BOMB].effect == EFFECT_POISON_HIT);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_POISON_PUPPETEER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SLUDGE_BOMB, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT(opponent->status2 & STATUS2_CONFUSION);
    }
}
