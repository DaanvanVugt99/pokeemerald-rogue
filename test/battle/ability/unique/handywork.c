#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].makesContact);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
    ASSUME(gBattleMoves[MOVE_SCRATCH].power > 20);
}

SINGLE_BATTLE_TEST("Handywork uses a 20 BP Scratch after contact moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_AIPOM) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_HANDYWORK); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_HANDYWORK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
        EXPECT_LT(secondHit, firstHit);
    }
}

SINGLE_BATTLE_TEST("Handywork does not trigger after non-contact moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_HANDYWORK; }

    GIVEN {
        PLAYER(SPECIES_AIPOM) { Ability(ABILITY_RUN_AWAY); UniqueAbility(uniqueAbility); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
