#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Spell Fist boosts punching moves by 1.3× and targets Sp. Def", s16 damage)
{
    u32 ability;
    PARAMETRIZE
    {
        ability = ABILITY_NONE;
    }
    PARAMETRIZE
    {
        ability = ABILITY_SPELL_FIST;
    }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_DRAIN_PUNCH].flags & FLAG_PUNCHING_BASED);

        PLAYER(SPECIES_HITMONCHAN)
        {
            Ability(ability);
            Attack(100);
        }

        OPPONENT(SPECIES_WOBBUFFET)
        {
            Defense(200);
            SpDefense(50);
            Defense(200);
            SpDefense(50);
        }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_DRAIN_PUNCH); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage); // Spell Fist multiplies damage by 1.3
    }
}
