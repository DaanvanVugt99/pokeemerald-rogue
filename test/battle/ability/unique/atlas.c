#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
}

SINGLE_BATTLE_TEST("Atlas uses Gravity on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_CAMERUPT) { Ability(ABILITY_MAGMA_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GROUDON) { Ability(ABILITY_DROUGHT); UniqueAbility(ABILITY_ATLAS); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_ATLAS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponent);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Atlas does not use Gravity on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GROUDON) { Ability(ABILITY_DROUGHT); UniqueAbility(ABILITY_ATLAS); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_GRAVITY));
    }
}
