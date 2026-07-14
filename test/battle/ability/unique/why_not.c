#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Why Not lets Counter answer special damage for 1.5x damage")
{
    GIVEN {
        ASSUME(IS_MOVE_SPECIAL(MOVE_DRAGON_RAGE));
        PLAYER(SPECIES_WYNAUT) { Speed(1); HP(200); MaxHP(200); Moves(MOVE_COUNTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Level(40); HP(200); MaxHP(200); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COUNTER); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WHY_NOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COUNTER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 160);
        EXPECT_EQ(opponent->hp, 140);
    }
}

SINGLE_BATTLE_TEST("Why Not lets Mirror Coat answer physical damage for 1.5x damage")
{
    GIVEN {
        ASSUME(IS_MOVE_PHYSICAL(MOVE_SEISMIC_TOSS));
        PLAYER(SPECIES_WYNAUT) { Speed(1); HP(200); MaxHP(200); Moves(MOVE_MIRROR_COAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Level(40); HP(200); MaxHP(200); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(player, MOVE_MIRROR_COAT); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WHY_NOT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MIRROR_COAT, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 160);
        EXPECT_EQ(opponent->hp, 140);
    }
}

SINGLE_BATTLE_TEST("Why Not keeps the normal 2x damage for a correct Counter or Mirror Coat read")
{
    u16 counterMove;
    u16 attackingMove;

    PARAMETRIZE { counterMove = MOVE_COUNTER;     attackingMove = MOVE_SEISMIC_TOSS; }
    PARAMETRIZE { counterMove = MOVE_MIRROR_COAT; attackingMove = MOVE_DRAGON_RAGE; }

    GIVEN {
        ASSUME(IS_MOVE_PHYSICAL(MOVE_SEISMIC_TOSS));
        ASSUME(IS_MOVE_SPECIAL(MOVE_DRAGON_RAGE));
        PLAYER(SPECIES_WYNAUT) { Speed(1); HP(200); MaxHP(200); Moves(counterMove); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Level(40); HP(200); MaxHP(200); Moves(attackingMove); }
    } WHEN {
        TURN { MOVE(player, counterMove); MOVE(opponent, attackingMove); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_WHY_NOT);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->hp, 160);
        EXPECT_EQ(opponent->hp, 120);
    }
}
