#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].power > 0);
    ASSUME(gBattleMoves[MOVE_SANDSTORM].effect == EFFECT_SANDSTORM);
    ASSUME(gSpeciesInfo[SPECIES_CAMERUPT].types[0] == TYPE_FIRE || gSpeciesInfo[SPECIES_CAMERUPT].types[1] == TYPE_FIRE);
    ASSUME(gSpeciesInfo[SPECIES_CAMERUPT].types[0] == TYPE_GROUND || gSpeciesInfo[SPECIES_CAMERUPT].types[1] == TYPE_GROUND);
}

SINGLE_BATTLE_TEST("Filter reduces super effective damage by 35 percent", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_FILTER; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_CAMERUPT) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.65), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Solid Rock reduces super effective damage by 25 percent normally and 50 percent in sandstorm", s16 damage)
{
    u32 ability;
    bool32 sandstorm;

    PARAMETRIZE { ability = ABILITY_NONE; sandstorm = FALSE; }
    PARAMETRIZE { ability = ABILITY_SOLID_ROCK; sandstorm = FALSE; }
    PARAMETRIZE { ability = ABILITY_NONE; sandstorm = TRUE; }
    PARAMETRIZE { ability = ABILITY_SOLID_ROCK; sandstorm = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN, MOVE_SANDSTORM); }
        OPPONENT(SPECIES_CAMERUPT) { Ability(ability); }
    } WHEN {
        if (sandstorm)
            TURN { MOVE(player, MOVE_SANDSTORM); }
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.75), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(0.5), results[3].damage);
    }
}
