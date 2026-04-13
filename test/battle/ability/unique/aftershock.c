#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BULLDOZE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_BULLDOZE].power > 30);
}

SINGLE_BATTLE_TEST("Aftershock adds a 30 BP Bulldoze after Ground-type moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_GOLEM) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_AFTERSHOCK); Moves(MOVE_BULLDOZE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BULLDOZE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLDOZE, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_AFTERSHOCK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BULLDOZE, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
        EXPECT_LT(secondHit, firstHit);
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Aftershock does not trigger after non-Ground moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_AFTERSHOCK; }

    GIVEN {
        PLAYER(SPECIES_GOLEM) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
