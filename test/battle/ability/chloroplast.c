#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Fire moves gain weather boost from chloroplast", s16 damage)
{
    u32 move;
    u16 ability;
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_CHLOROPLAST;
    }
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_NONE;
    }
    PARAMETRIZE
    {
        move = MOVE_HEADBUTT;
        ability = ABILITY_CHLOROPLAST;
    }
    PARAMETRIZE
    {
        move = MOVE_HEADBUTT;
        ability = ABILITY_NONE;
    }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE);
        ASSUME(gBattleMoves[MOVE_SCRATCH].type != TYPE_FIRE);
        PLAYER(SPECIES_CHARIZARD) { Ability(ability); }
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
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage); // Fire move gains boost
        EXPECT_EQ(results[2].damage, results[3].damage);                  // Non-Fire move does not gain boost
    }
}

SINGLE_BATTLE_TEST("Solar Beam and Solar Blade can be used instantly with chloroplast")
{
    u32 move;
    PARAMETRIZE { move = MOVE_SOLAR_BEAM; }
    PARAMETRIZE { move = MOVE_SOLAR_BLADE; }
    GIVEN
    {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CHLOROPLAST); };
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN
        {
            MOVE(player, move);
        }
    }
    SCENE
    {
        NOT MESSAGE("Wobbuffet took in sunlight!");
    }
}