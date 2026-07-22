#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PROTECT].protectionMove);
    ASSUME(gBattleMoves[MOVE_DETECT].protectionMove);
    ASSUME(gBattleMoves[MOVE_SPIKY_SHIELD].protectionMove);
}

SINGLE_BATTLE_TEST("Sword and Board raises Attack after using a protection move")
{
    u32 species;
    u32 protectMove;

    PARAMETRIZE { protectMove = MOVE_PROTECT; species = SPECIES_FARFETCHD_GALARIAN; }
    PARAMETRIZE { protectMove = MOVE_DETECT; species = SPECIES_SIRFETCHD; }
    PARAMETRIZE { protectMove = MOVE_SPIKY_SHIELD; species = SPECIES_FARFETCHD_GALARIAN; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_STEADFAST); UniqueAbility(ABILITY_SWORD_AND_BOARD); Moves(protectMove); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, protectMove); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, protectMove, player);
        ABILITY_POPUP(player, ABILITY_SWORD_AND_BOARD);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Sword and Board does not trigger when the protection move fails")
{
    GIVEN {
        PLAYER(SPECIES_FARFETCHD_GALARIAN) { Speed(50); Ability(ABILITY_STEADFAST); UniqueAbility(ABILITY_SWORD_AND_BOARD); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_MAGIC_COAT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_MAGIC_COAT); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SWORD_AND_BOARD);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
