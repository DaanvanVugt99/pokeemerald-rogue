#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Drizzle summons rain", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_DRIZZLE; }
    PARAMETRIZE { ability = ABILITY_DAMP; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_BUBBLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BUBBLE); }
    } SCENE {
        if (ability == ABILITY_DRIZZLE) {
            ABILITY_POPUP(player, ABILITY_DRIZZLE);
            MESSAGE("Wobbuffet's Drizzle made it rain!");
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Drizzle weather lasts for 8 turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_DRIZZLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_RAIN);
        EXPECT_EQ(gWishFutureKnock.weatherDuration, WEATHER_DURATION_TURNS - 1);
    }
}
