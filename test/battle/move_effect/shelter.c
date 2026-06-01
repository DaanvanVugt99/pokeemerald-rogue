#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHELTER].effect == EFFECT_SHELTER);
}

SINGLE_BATTLE_TEST("Shelter raises Defense by one stage and restores one third max HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(120); MaxHP(300); Moves(MOVE_SHELTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHELTER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHELTER, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Defense rose!");
        HP_BAR(player, damage: -100);
        MESSAGE("Wobbuffet regained health!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->hp, 220);
    }
}

SINGLE_BATTLE_TEST("Shelter succeeds if only the Defense boost can apply")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Moves(MOVE_SHELTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHELTER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->hp, 300);
    }
}

