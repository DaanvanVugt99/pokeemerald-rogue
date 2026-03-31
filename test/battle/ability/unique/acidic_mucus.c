#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POISON_JAB].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Acidic Mucus lowers Sp. Def by 2 only on contact moves")
{
    u16 move;
    bool8 shouldDrop;
    PARAMETRIZE { move = MOVE_POISON_JAB; shouldDrop = TRUE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; shouldDrop = FALSE; }

    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_ACIDIC_MUCUS); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - (shouldDrop ? 2 : 0));
    }
}

SINGLE_BATTLE_TEST("Acidic Mucus can trigger alongside other move-end attacker abilities")
{
    GIVEN {
        PLAYER(SPECIES_TENTACRUEL) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_ACIDIC_MUCUS); Speed(100); Moves(MOVE_POISON_JAB); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); MaxHP(999); HP(999); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_JAB, WITH_RNG(RNG_STENCH, TRUE)); MOVE(opponent, MOVE_AERIAL_ACE); }
    } THEN {
        EXPECT_GT(opponent->hp, 0);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
