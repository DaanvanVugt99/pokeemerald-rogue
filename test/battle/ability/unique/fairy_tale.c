#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HEALING_WISH].effect == EFFECT_HEALING_WISH);
}

SINGLE_BATTLE_TEST("Fairy Tale boosts the incoming Pokemon after Healing Wish")
{
    GIVEN {
        ASSUME(B_HEALING_WISH_SWITCH >= GEN_5);
        PLAYER(SPECIES_RAPIDASH_GALARIAN) { Ability(ABILITY_PASTEL_VEIL); UniqueAbility(ABILITY_FAIRY_TALE); Moves(MOVE_HEALING_WISH); }
        PLAYER(SPECIES_WYNAUT) { HP(1); MaxHP(100); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_HEALING_WISH); SEND_OUT(player, 1); }
    } SCENE {
        MESSAGE("The healing wish came true for Wynaut!");
        MESSAGE("Wynaut regained health!");
        ABILITY_POPUP(player, ABILITY_FAIRY_TALE);
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}
