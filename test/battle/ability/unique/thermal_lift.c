#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_SKY_ATTACK].effect == EFFECT_TWO_TURNS_ATTACK);
}

SINGLE_BATTLE_TEST("Thermal Lift uses Sky Attack after the first Fire-type move each battle")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_THERMAL_LIFT); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_SKY_ATTACK);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Thermal Lift waits for a Fire-type move")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_THERMAL_LIFT); Moves(MOVE_TACKLE, MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Thermal Lift only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_THERMAL_LIFT); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        }
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
