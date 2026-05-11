#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_SPOTLIGHT].effect == EFFECT_FOLLOW_ME);
}

DOUBLE_BATTLE_TEST("Sporelight uses Spotlight after the first healing move each battle")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) { HP(50); MaxHP(400); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPORELIGHT); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RECOVER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

DOUBLE_BATTLE_TEST("Sporelight does not trigger after non-healing moves")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) { Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPORELIGHT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
        }
    } THEN {
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}

DOUBLE_BATTLE_TEST("Sporelight only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) { HP(50); MaxHP(400); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPORELIGHT); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RECOVER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_RECOVER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerLeft);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
        }
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
