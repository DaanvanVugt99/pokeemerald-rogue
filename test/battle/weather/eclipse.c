#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_FAIRY_WIND].type == TYPE_FAIRY);
}

SINGLE_BATTLE_TEST("Eclipse multiplies the power of Dark-type moves by 1.5x", s16 damage)
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_ECLIPSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Eclipse multiplies the power of Fairy-type moves by 0.5x", s16 damage)
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_ECLIPSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, setupMove); }
        TURN { MOVE(player, MOVE_FAIRY_WIND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAIRY_WIND, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Eclipse lasts for 12 turns with Dim Rock")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_DIM_ROCK); Moves(MOVE_ECLIPSE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_EXTENDED - 1);
    }
}

SINGLE_BATTLE_TEST("Eclipse move animation can be used from either side")
{
    bool32 playerUsesEclipse;
    PARAMETRIZE { playerUsesEclipse = TRUE; }
    PARAMETRIZE { playerUsesEclipse = FALSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            if (playerUsesEclipse) {
                MOVE(player, MOVE_ECLIPSE);
                MOVE(opponent, MOVE_CELEBRATE);
            } else {
                MOVE(player, MOVE_CELEBRATE);
                MOVE(opponent, MOVE_ECLIPSE);
            }
        }
    } SCENE {
        if (playerUsesEclipse)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ECLIPSE, player);
        else
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ECLIPSE, opponent);
        MESSAGE("An eclipse blotted out the sun!");
    }
}
