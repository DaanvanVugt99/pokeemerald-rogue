#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SWIFT].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
}

SINGLE_BATTLE_TEST("Grazing Field sets Plain Terrain after being hit by a physical move with 2 other Normal-type allies")
{
    GIVEN {
        PLAYER(SPECIES_WOOLOO) { Ability(ABILITY_FLUFFY); UniqueAbility(ABILITY_GRAZING_FIELD); }
        PLAYER(SPECIES_SNORLAX) { Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_WIGGLYTUFF) { Ability(ABILITY_CUTE_CHARM); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Grazing Field does not set Plain Terrain without 2 other Normal-type allies")
{
    GIVEN {
        PLAYER(SPECIES_WOOLOO) { Ability(ABILITY_FLUFFY); UniqueAbility(ABILITY_GRAZING_FIELD); }
        PLAYER(SPECIES_BULBASAUR) { Ability(ABILITY_OVERGROW); }
        PLAYER(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_GRAZING_FIELD);
            MESSAGE("Plain terrain spread\nacross the battlefield!");
        }
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN, 0);
    }
}
