#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_RECOVER].healBlockBanned);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Ultra Ascent reduces damage after the user uses a Flying-type move", s16 damage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_AERIAL_ACE; }

    GIVEN {
        PLAYER(SPECIES_CELESTEELA) { Speed(1); SpDefense(100); MaxHP(500); HP(500); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE, MOVE_AERIAL_ACE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(100); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        if (setupMove == MOVE_AERIAL_ACE)
            ABILITY_POPUP(player, ABILITY_ULTRA_ASCENT);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.75), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Ultra Ascent reduces damage after the user uses a healing move", s16 damage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_RECOVER; }

    GIVEN {
        PLAYER(SPECIES_CELESTEELA) { Speed(1); SpDefense(100); MaxHP(500); HP(250); Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE, MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); SpAttack(100); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        if (setupMove == MOVE_RECOVER)
            ABILITY_POPUP(player, ABILITY_ULTRA_ASCENT);
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.75), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Ultra Ascent does not activate if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_CELESTEELA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_AERIAL_ACE); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_ASCENT);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].ultraAscentActive);
    }
}
