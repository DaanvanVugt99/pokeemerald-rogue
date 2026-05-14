#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ASTONISH].power > 0);
}

SINGLE_BATTLE_TEST("Bitter Brew uses 20 BP Astonish after a successful status move")
{
    GIVEN {
        PLAYER(SPECIES_POLTEAGEIST) { Speed(200); Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_BITTER_BREW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BITTER_BREW);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Bitter Brew does not trigger from non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_POLTEAGEIST) { Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_BITTER_BREW); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BITTER_BREW);
        }
    }
}
