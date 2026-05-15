#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].type == TYPE_DRAGON);
    ASSUME(IS_MOVE_SPECIAL(MOVE_DRAGON_PULSE));
}

SINGLE_BATTLE_TEST("Crown of Fangs makes Dragon moves target the lower defensive stat", s16 damage)
{
    bool32 defenseIsLower;

    PARAMETRIZE { defenseIsLower = TRUE; }
    PARAMETRIZE { defenseIsLower = FALSE; }

    GIVEN {
        PLAYER(SPECIES_REGIDRAGO) { Level(100); Speed(100); SpAttack(100); Ability(ABILITY_DRAGONS_MAW); Moves(MOVE_DRAGON_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(500); MaxHP(500); Speed(50); Defense(defenseIsLower ? 50 : 200); SpDefense(defenseIsLower ? 200 : 50); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_PULSE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
