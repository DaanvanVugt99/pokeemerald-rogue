#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Hydration cures non-volatile Status conditions if it is raining")
{
    GIVEN {
        PLAYER(SPECIES_VAPOREON) { Ability(ABILITY_HYDRATION); Status1(STATUS1_BURN); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HYDRATION);
        MESSAGE("Vaporeon's Hydration cured its burn problem!");
        STATUS_ICON(player, none: TRUE);
    }
}

SINGLE_BATTLE_TEST("Hydration takes 20 percent less damage during rain", s16 damage)
{
    bool32 rain;
    u32 ability;

    PARAMETRIZE { rain = FALSE; ability = ABILITY_NONE; }
    PARAMETRIZE { rain = FALSE; ability = ABILITY_HYDRATION; }
    PARAMETRIZE { rain = TRUE; ability = ABILITY_NONE; }
    PARAMETRIZE { rain = TRUE; ability = ABILITY_HYDRATION; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        if (rain)
            TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.8), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Hydration takes 20 percent less damage during rain as a unique ability", s16 damage)
{
    u32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_PRESSURE; }
    PARAMETRIZE { uniqueAbility = ABILITY_HYDRATION; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NONE); UniqueAbility(uniqueAbility); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.8), results[1].damage);
    }
}
