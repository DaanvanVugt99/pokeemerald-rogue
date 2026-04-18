#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLAME_WHEEL].power == 60);
    ASSUME(gBattleMoves[MOVE_BITE].power == 60);
    ASSUME(gBattleMoves[MOVE_FLAME_WHEEL].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_FLAME_WHEEL].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_BITE].split == SPLIT_PHYSICAL);
}

SINGLE_BATTLE_TEST("Infernal Rage boosts same-BP Fire moves over Dark moves", s16 damage)
{
    u32 move;

    PARAMETRIZE { move = MOVE_BITE; }
    PARAMETRIZE { move = MOVE_FLAME_WHEEL; }

    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { Moves(move); }
        OPPONENT(SPECIES_EEVEE);
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Infernal Rage applies recoil only to Fire-type moves")
{
    u32 move;
    u16 expectedHp;

    PARAMETRIZE { move = MOVE_BITE; expectedHp = 100; }
    PARAMETRIZE { move = MOVE_FLAME_WHEEL; expectedHp = 90; }

    GIVEN {
        PLAYER(SPECIES_HOUNDOUR) { MaxHP(100); HP(100); Moves(move); }
        OPPONENT(SPECIES_EEVEE);
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_EQ(player->hp, expectedHp);
    }
}
