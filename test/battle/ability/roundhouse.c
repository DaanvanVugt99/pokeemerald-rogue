#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Roundhouse ensures kicking moves always hit and targets lowest defense", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_ROUNDHOUSE; }
    PARAMETRIZE { ability = ABILITY_INTIMIDATE; }

    ASSUME(gBattleMoves[MOVE_HIGH_JUMP_KICK].flags & FLAG_KICKING_BASED);
    GIVEN
    {
        PLAYER(SPECIES_HITMONTOP)
        {
            Ability(ability);
        }
        OPPONENT(SPECIES_WOBBUFFET)
        {
            Defense(200);
            SpDefense(50);
        }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_HIGH_JUMP_KICK); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_GT(results[0].damage, results[1].damage); // Hits SpDef for higher dmg
    }
}
