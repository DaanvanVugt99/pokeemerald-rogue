#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Whiteout increases ice move damage in snow", s16 damage)
{
    u32 move;
    u16 ability;
    PARAMETRIZE
    {
        move = MOVE_ICE_BEAM;
        ability = ABILITY_WHITEOUT;
    }
    PARAMETRIZE
    {
        move = MOVE_ICE_BEAM;
        ability = ABILITY_STEADFAST;
    }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_ICE_BEAM].type == TYPE_ICE);
        PLAYER(SPECIES_GLACEON) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_SNOWSCAPE); }
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Whiteout does not increase ice move damage without snow", s16 damage)
{
    u32 move;
    u16 ability;

    PARAMETRIZE
    {
        move = MOVE_ICE_BEAM;
        ability = ABILITY_WHITEOUT;
    }
    PARAMETRIZE
    {
        move = MOVE_ICE_BEAM;
        ability = ABILITY_STEADFAST;
    }

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_ICE_BEAM].type == TYPE_ICE);
        PLAYER(SPECIES_GLACEON) { Ability(ability); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); }
    }
    WHEN
    {
        TURN { MOVE(player, move); } // No Snow setup here
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        EXPECT_EQ(results[0].damage, results[1].damage); // Whiteout should have no effect without Snow
    }
}