#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
}

SINGLE_BATTLE_TEST("Bounty does not activate if the target is not knocked out")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Level(100); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_BOUNTY);
    }
}

SINGLE_BATTLE_TEST("Bounty is the unique ability for the Sandile line")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SANDILE), ABILITY_BOUNTY);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_KROKOROK), ABILITY_BOUNTY);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_KROOKODILE), ABILITY_BOUNTY);
    }
}
