#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AROMATHERAPY].effect == EFFECT_HEAL_BELL);
}

SINGLE_BATTLE_TEST("Blooming Cascade uses Aromatherapy on switch-in if the party has 2 other Grass-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(100); Ability(ABILITY_OVERGROW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SHAYMIN) { Speed(50); Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_BLOOMING_CASCADE); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_ODDISH) { Speed(40); Ability(ABILITY_CHLOROPHYLL); }
        OPPONENT(SPECIES_BELLSPROUT) { Speed(30); Ability(ABILITY_CHLOROPHYLL); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_BLOOMING_CASCADE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AROMATHERAPY, opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Blooming Cascade does not use Aromatherapy on switch-in without 2 other Grass-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Speed(100); Ability(ABILITY_OVERGROW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SHAYMIN) { Speed(50); Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_BLOOMING_CASCADE); Status1(STATUS1_POISON); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(40); Ability(ABILITY_SWIFT_SWIM); }
        OPPONENT(SPECIES_PIKACHU) { Speed(30); Ability(ABILITY_STATIC); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_BLOOMING_CASCADE);
    } THEN {
        EXPECT(player->status1 == STATUS1_NONE);
        EXPECT(opponent->status1 & STATUS1_POISON);
    }
}

SINGLE_BATTLE_TEST("Blooming Cascade is assigned to Shaymin")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SHAYMIN_LAND), ABILITY_BLOOMING_CASCADE);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SHAYMIN_SKY), ABILITY_BLOOMING_CASCADE);
    }
}
