#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TEETER_DANCE].effect == EFFECT_TEETER_DANCE);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_CONFUSION].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_BODY_PRESS].effect == EFFECT_BODY_PRESS);
    ASSUME(gBattleMoves[MOVE_FOUL_PLAY].effect == EFFECT_FOUL_PLAY);
}

SINGLE_BATTLE_TEST("Stumble uses Teeter Dance on switch-in and confuses the user too")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPINDA) { Ability(ABILITY_TANGLED_FEET); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_STUMBLE);
    } THEN {
        EXPECT_NE(player->status2 & STATUS2_CONFUSION, 0);
        EXPECT_NE(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Stumble does not confuse the user through Own Tempo")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPINDA) { Ability(ABILITY_OWN_TEMPO); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_STUMBLE);
    } THEN {
        EXPECT_NE(player->status2 & STATUS2_CONFUSION, 0);
        EXPECT_EQ(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Stumble does not confuse the user through Safeguard")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SAFEGUARD, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPINDA) { Ability(ABILITY_TANGLED_FEET); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SAFEGUARD); MOVE(player, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(player->status2 & STATUS2_CONFUSION, 0);
        EXPECT_EQ(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Stumble still activates on switch-in while asleep or frozen")
{
    u32 status1;

    PARAMETRIZE { status1 = STATUS1_SLEEP_TURN(2); }
    PARAMETRIZE { status1 = STATUS1_FREEZE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SPINDA) { Ability(ABILITY_TANGLED_FEET); Status1(status1); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(player->status2 & STATUS2_CONFUSION, 0);
        EXPECT_NE(opponent->status2 & STATUS2_CONFUSION, 0);
    }
}

SINGLE_BATTLE_TEST("Stumble doubles both physical and special damage", s16 damage)
{
    u16 move;
    u16 uniqueAbility;

    PARAMETRIZE { move = MOVE_TACKLE; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_TACKLE; uniqueAbility = ABILITY_STUMBLE; }
    PARAMETRIZE { move = MOVE_CONFUSION; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_CONFUSION; uniqueAbility = ABILITY_STUMBLE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(uniqueAbility); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(2.0), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Stumble does not boost Body Press or Foul Play", s16 damage)
{
    u16 move;
    u16 uniqueAbility;

    PARAMETRIZE { move = MOVE_BODY_PRESS; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_BODY_PRESS; uniqueAbility = ABILITY_STUMBLE; }
    PARAMETRIZE { move = MOVE_FOUL_PLAY; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_FOUL_PLAY; uniqueAbility = ABILITY_STUMBLE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(uniqueAbility); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OWN_TEMPO); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}
