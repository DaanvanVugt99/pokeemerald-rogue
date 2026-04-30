#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sporefield prevents opposing Fairy-types from escaping")
{
    GIVEN {
        PLAYER(SPECIES_CLEFAIRY) { Ability(ABILITY_CUTE_CHARM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_AMOONGUSS) { Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPOREFIELD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(IsAbilityPreventingEscape(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), 0);
    }
}

SINGLE_BATTLE_TEST("Sporefield does not prevent opposing non-Fairy-types from escaping")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_AMOONGUSS) { Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPOREFIELD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(IsAbilityPreventingEscape(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), 0);
    }
}
