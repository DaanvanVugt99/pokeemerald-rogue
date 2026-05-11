#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SOAK].effect == EFFECT_SOAK);
}

SINGLE_BATTLE_TEST("Washed Ashore uses Soak on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PYUKUMUKU) { Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_WASHED_ASHORE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_WASHED_ASHORE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SOAK, opponent);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_WATER);
        EXPECT_EQ(player->type2, TYPE_WATER);
    }
}
