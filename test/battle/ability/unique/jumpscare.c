#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
}

SINGLE_BATTLE_TEST("Jumpscare lowers the foe's Speed by 1 on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BANETTE) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_JUMPSCARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_JUMPSCARE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Jumpscare uses Scary Face on switch-in during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BANETTE) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_JUMPSCARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ECLIPSE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECLIPSE, opponent);
        ABILITY_POPUP(player, ABILITY_JUMPSCARE);
        MESSAGE("Banette used Scary Face!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}
