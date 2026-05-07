#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROUND].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Solar Power boosts Special Attack in sun", s16 damage)
{
    bool32 sun;
    u32 ability;

    PARAMETRIZE { sun = FALSE; ability = ABILITY_NONE; }
    PARAMETRIZE { sun = FALSE; ability = ABILITY_SOLAR_POWER; }
    PARAMETRIZE { sun = TRUE; ability = ABILITY_NONE; }
    PARAMETRIZE { sun = TRUE; ability = ABILITY_SOLAR_POWER; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_SUNNY_DAY, MOVE_ROUND); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (sun)
            TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_ROUND); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROUND, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(1.5), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Solar Power does not damage the holder in sun")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SOLAR_POWER); Speed(20); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SOLAR_POWER);
            HP_BAR(player);
            MESSAGE("The Wobbuffet's Solar Power takes its toll!");
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
