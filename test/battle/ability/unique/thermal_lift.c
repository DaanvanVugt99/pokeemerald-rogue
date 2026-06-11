#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_SKY_ATTACK].effect == EFFECT_TWO_TURNS_ATTACK);
}

SINGLE_BATTLE_TEST("Thermal Lift starts Sky Attack charging after the first Fire-type move each battle")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_THERMAL_LIFT); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_SKY_ATTACK);
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].status2 & STATUS2_MULTIPLETURNS);
        EXPECT_EQ(gLockedMoves[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)], MOVE_SKY_ATTACK);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Thermal Lift's Sky Attack hits on the next turn")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_THERMAL_LIFT); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SKIP_TURN(player); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(opponent->hp < opponent->maxHP);
        EXPECT(!(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].status2 & STATUS2_MULTIPLETURNS));
    }
}

SINGLE_BATTLE_TEST("Thermal Lift's Sky Attack consumes Power Herb to hit immediately")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_THERMAL_LIFT); Item(ITEM_POWER_HERB); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_THERMAL_LIFT);
        MESSAGE("Charizard became fully charged due to its Power Herb!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SKY_ATTACK, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(opponent->hp < opponent->maxHP);
        EXPECT(!(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].status2 & STATUS2_MULTIPLETURNS));
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
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].status2 & STATUS2_MULTIPLETURNS);
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
        TURN { SKIP_TURN(player); MOVE(opponent, MOVE_CELEBRATE); }
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
        EXPECT(!(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].status2 & STATUS2_MULTIPLETURNS));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
