#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_JETSTREAM].effect == EFFECT_BODY_PRESS);
    ASSUME(gBattleMoves[MOVE_JETSTREAM].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_JETSTREAM].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_JETSTREAM].power == 70);
    ASSUME(gBattleMoves[MOVE_JETSTREAM].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_JETSTREAM].pp == 10);
}

SINGLE_BATTLE_TEST("Jetstream damage increases after Agility, unlike normal physical moves", s16 damage)
{
    u16 move;
    bool32 useAgility;

    PARAMETRIZE { move = MOVE_TACKLE; useAgility = FALSE; }
    PARAMETRIZE { move = MOVE_TACKLE; useAgility = TRUE; }
    PARAMETRIZE { move = MOVE_JETSTREAM; useAgility = FALSE; }
    PARAMETRIZE { move = MOVE_JETSTREAM; useAgility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Speed(100); Moves(MOVE_AGILITY, move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        if (useAgility)
            TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, Q_4_12(2.0), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Jetstream is not reduced by burn", s16 damage)
{
    bool32 burned;
    u16 move;

    PARAMETRIZE { move = MOVE_TACKLE; burned = FALSE; }
    PARAMETRIZE { move = MOVE_TACKLE; burned = TRUE; }
    PARAMETRIZE { move = MOVE_JETSTREAM; burned = FALSE; }
    PARAMETRIZE { move = MOVE_JETSTREAM; burned = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { if (burned) Status1(STATUS1_BURN); Attack(100); Speed(100); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Jetstream is not boosted by Tailwind", s16 damage)
{
    bool32 useTailwind;

    PARAMETRIZE { useTailwind = FALSE; }
    PARAMETRIZE { useTailwind = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TAILWIND, MOVE_JETSTREAM, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        if (useTailwind)
            TURN { MOVE(player, MOVE_TAILWIND); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_JETSTREAM); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
