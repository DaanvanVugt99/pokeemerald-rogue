#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROCK_SLIDE].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_STONE_EDGE].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ROCK);
}

SINGLE_BATTLE_TEST("Sky Tyrant gives +1 priority to Rock-type moves if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Speed(50); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SKY_TYRANT); Moves(MOVE_ROCK_SLIDE); }
        PLAYER(SPECIES_CHARIZARD)  { Speed(30); Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_GOLEM)      { Speed(20); Ability(ABILITY_ROCK_HEAD); }

        OPPONENT(SPECIES_HYPNO)    { Speed(60); Ability(ABILITY_INNER_FOCUS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_SLIDE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_SLIDE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Sky Tyrant does not give priority if even one Pokémon on the team does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Speed(50); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SKY_TYRANT); Moves(MOVE_ROCK_SLIDE); }
        PLAYER(SPECIES_CHARIZARD)  { Speed(30); Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_PIKACHU)    { Speed(20); Ability(ABILITY_STATIC); }

        OPPONENT(SPECIES_BLISSEY)  { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_SLIDE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_SLIDE, player);
    }
}

SINGLE_BATTLE_TEST("Sky Tyrant only affects Rock-type moves")
{
    GIVEN {
        PLAYER(SPECIES_AERODACTYL) { Speed(50); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SKY_TYRANT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_CHARIZARD)  { Speed(30); Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_GOLEM)      { Speed(20); Ability(ABILITY_ROCK_HEAD); }

        OPPONENT(SPECIES_BLISSEY)  { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}
