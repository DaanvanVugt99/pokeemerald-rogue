#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
}

SINGLE_BATTLE_TEST("Battlecry uses Growl on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PYROAR) { Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_BATTLECRY); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_BATTLECRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Battlecry uses Growl at battle start")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PYROAR) { Ability(ABILITY_RIVALRY); UniqueAbility(ABILITY_BATTLECRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_BATTLECRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}
