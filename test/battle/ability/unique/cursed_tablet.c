#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBARGO].effect == EFFECT_EMBARGO);
}

SINGLE_BATTLE_TEST("Cursed Tablet uses Embargo on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_YAMASK_GALARIAN) { Ability(ABILITY_WANDERING_SPIRIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBARGO, player);
    } THEN {
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_EMBARGO);
    }
}

SINGLE_BATTLE_TEST("Cursed Tablet uses Embargo at battle start")
{
    GIVEN {
        PLAYER(SPECIES_RUNERIGUS) { Ability(ABILITY_WANDERING_SPIRIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBARGO, player);
    } THEN {
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_EMBARGO);
    }
}
