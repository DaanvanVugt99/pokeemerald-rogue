#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
}

SINGLE_BATTLE_TEST("Gravity Well sets Gravity on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GRAVITY_WELL); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_GRAVITY_WELL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponent);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Gravity Well sets Gravity when sent out at battle start")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GRAVITY_WELL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_GRAVITY_WELL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, opponent);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Gravity Well applies Gravity effect at battle start")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NOSEPASS) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GRAVITY_WELL); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Gravity Well applies Gravity effect at battle start on the player side")
{
    GIVEN {
        PLAYER(SPECIES_NOSEPASS) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_GRAVITY_WELL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT_EQ(gFieldTimers.gravityTimer, 4);
    }
}

SINGLE_BATTLE_TEST("Gravity Well is assigned to Nosepass line and Palkia forms")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_NOSEPASS), ABILITY_GRAVITY_WELL);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_PROBOPASS), ABILITY_GRAVITY_WELL);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_PALKIA), ABILITY_GRAVITY_WELL);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_PALKIA_ORIGIN), ABILITY_GRAVITY_WELL);
    }
}
