#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
}

SINGLE_BATTLE_TEST("Ripjaw applies a two-turn Heal Block after a biting move")
{
    GIVEN {
        PLAYER(SPECIES_DREDNAW) { Speed(100); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_RIPJAW); Moves(MOVE_BITE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RIPJAW);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_HEAL_BLOCK);
        EXPECT_EQ(gDisableStructs[B_POSITION_OPPONENT_LEFT].healBlockTimer, 2);
    }
}

SINGLE_BATTLE_TEST("Ripjaw does not trigger on non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_DREDNAW) { Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_RIPJAW); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_RECOVER); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RIPJAW);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_HEAL_BLOCK));
    }
}
