#include "global.h"
#include "test/battle.h"

// Test: Fighting Spirit changes Fighting moves to Ghost type and boosts damage by 1.3x
SINGLE_BATTLE_TEST("Fighting Spirit changes Fighting move to Ghost and boosts damage", s16 damage)
{
    u32 move;
    u16 ability;
    PARAMETRIZE
    {
        move = MOVE_CLOSE_COMBAT;
        ability = ABILITY_FIGHTING_SPIRIT;
    }
    PARAMETRIZE
    {
        move = MOVE_CLOSE_COMBAT;
        ability = ABILITY_GUTS;
    }
    GIVEN
    {
        PLAYER(SPECIES_LUCARIO) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage); // Fighting Spirit boosts damage
    }
}
