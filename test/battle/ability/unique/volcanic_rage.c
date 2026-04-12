#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_EMBER].power == 40);
    ASSUME(gBattleMoves[MOVE_ERUPTION].power > 40);
}

SINGLE_BATTLE_TEST("Volcanic Rage adds a second hit after Fire-type moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_CAMERUPT) { Ability(ABILITY_MAGMA_ARMOR); UniqueAbility(ABILITY_VOLCANIC_RAGE); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_VOLCANIC_RAGE);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_EQ(firstHit, secondHit);
    }
}

SINGLE_BATTLE_TEST("Volcanic Rage does not trigger after non-Fire moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_VOLCANIC_RAGE; }

    GIVEN {
        PLAYER(SPECIES_CAMERUPT) { Ability(ABILITY_MAGMA_ARMOR); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
