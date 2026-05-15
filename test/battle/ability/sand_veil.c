#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sand Veil prevents damage from sandstorm")
{
    GIVEN {
        PLAYER(SPECIES_CACNEA) { Ability(ABILITY_SAND_VEIL); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SANDSTORM); }
        TURN {}
    } SCENE {
        NONE_OF { HP_BAR(player); }
    }
}

SINGLE_BATTLE_TEST("Sand Veil takes 30 percent less physical damage during sandstorm", s16 damage)
{
    u32 ability;
    u32 move;

    PARAMETRIZE { ability = ABILITY_NONE;      move = MOVE_TACKLE; }
    PARAMETRIZE { ability = ABILITY_SAND_VEIL; move = MOVE_TACKLE; }
    PARAMETRIZE { ability = ABILITY_NONE;      move = MOVE_ROUND; }
    PARAMETRIZE { ability = ABILITY_SAND_VEIL; move = MOVE_ROUND; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
        ASSUME(gBattleMoves[MOVE_ROUND].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM, MOVE_TACKLE, MOVE_ROUND); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_SANDSTORM); }
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.7), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
