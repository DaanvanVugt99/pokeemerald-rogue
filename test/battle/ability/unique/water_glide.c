#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Water Glide gives Flying moves STAB", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_WATER_GLIDE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
        ASSUME(gBattleMoves[MOVE_AERIAL_ACE].power > 0);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(uniqueAbility); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Water Glide lowers the target's Sp. Def by 1 after a Flying move hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_WATER_GLIDE); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_BLISSEY);
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WATER_GLIDE);
    } THEN {
        EXPECT_EQ(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Water Glide does not trigger for non-Flying moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_WATER_GLIDE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WATER_GLIDE);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
            MESSAGE("Foe Wobbuffet's Sp. Def fell!");
        }
    }
}

SINGLE_BATTLE_TEST("Masquerain is no longer immune to Ground-type moves")
{
    GIVEN {
        PLAYER(SPECIES_MASQUERAIN) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_EARTHQUAKE); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
