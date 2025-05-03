#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Color Change activates and changes the user's type to counter the move's type")
{
    u16 move;
    u8 expectedType;

    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        expectedType = TYPE_ROCK;
    }
    PARAMETRIZE
    {
        move = MOVE_THUNDERBOLT;
        expectedType = TYPE_GROUND;
    }
    PARAMETRIZE
    {
        move = MOVE_ICE_BEAM;
        expectedType = TYPE_WATER;
    }
    PARAMETRIZE
    {
        move = MOVE_SLUDGE_BOMB;
        expectedType = TYPE_STEEL;
    }
    PARAMETRIZE
    {
        move = MOVE_PSYCHIC;
        expectedType = TYPE_DARK;
    }
    PARAMETRIZE
    {
        move = MOVE_SHADOW_BALL;
        expectedType = TYPE_NORMAL;
    }

    GIVEN
    {
        PLAYER(SPECIES_KECLEON)
        {
            Ability(ABILITY_COLOR_CHANGE);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, move); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used");
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_COLOR_CHANGE);
        MESSAGE("Kecleon transformed into the");
    }
    THEN
    {
        EXPECT_EQ(player->type1, expectedType);
        EXPECT_EQ(player->type2, expectedType);
    }
}
