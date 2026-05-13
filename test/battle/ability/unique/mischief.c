#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WORRY_SEED].effect == EFFECT_WORRY_SEED);
}

SINGLE_BATTLE_TEST("Mischief uses Worry Seed on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_RILLABOOM) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_MISCHIEF); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MISCHIEF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORRY_SEED, opponent);
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_INSOMNIA);
    }
}
