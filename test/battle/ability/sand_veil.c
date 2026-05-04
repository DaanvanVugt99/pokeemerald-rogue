#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sand Veil prevents damage from sandstorm")
{
    GIVEN {
        PLAYER(SPECIES_CACNEA) { Ability(ABILITY_SAND_VEIL); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
        TURN {}
    } SCENE {
        NONE_OF { HP_BAR(player); }
    }
}

SINGLE_BATTLE_TEST("Sand Veil takes 30 percent less damage during sandstorm", s16 damage)
{
    bool32 sandstorm;
    u32 ability;

    PARAMETRIZE { sandstorm = FALSE; ability = ABILITY_NONE; }
    PARAMETRIZE { sandstorm = FALSE; ability = ABILITY_SAND_VEIL; }
    PARAMETRIZE { sandstorm = TRUE; ability = ABILITY_NONE; }
    PARAMETRIZE { sandstorm = TRUE; ability = ABILITY_SAND_VEIL; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        if (sandstorm)
            TURN { MOVE(player, MOVE_SANDSTORM); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.7), results[3].damage);
    }
}
