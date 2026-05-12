#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CUT].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
}

SINGLE_BATTLE_TEST("Ultra Edge ignores Defense boosts with slicing moves", s16 damage)
{
    bool32 raiseDef;
    PARAMETRIZE { raiseDef = FALSE; }
    PARAMETRIZE { raiseDef = TRUE; }

    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE, MOVE_CUT); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE, MOVE_IRON_DEFENSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, raiseDef ? MOVE_IRON_DEFENSE : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CUT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Ultra Edge does not ignore Defense boosts with non-slicing moves", s16 damage)
{
    bool32 raiseDef;
    PARAMETRIZE { raiseDef = FALSE; }
    PARAMETRIZE { raiseDef = TRUE; }

    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE, MOVE_IRON_DEFENSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, raiseDef ? MOVE_IRON_DEFENSE : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Ultra Edge does not ignore Defense boosts if another Ultra Beast is in the party", s16 damage)
{
    bool32 raiseDef;
    PARAMETRIZE { raiseDef = FALSE; }
    PARAMETRIZE { raiseDef = TRUE; }

    GIVEN {
        PLAYER(SPECIES_KARTANA) { Speed(100); Attack(100); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE, MOVE_CUT); }
        PLAYER(SPECIES_NIHILEGO) { Speed(1); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Defense(100); MaxHP(500); HP(500); Moves(MOVE_CELEBRATE, MOVE_IRON_DEFENSE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, raiseDef ? MOVE_IRON_DEFENSE : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CUT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
