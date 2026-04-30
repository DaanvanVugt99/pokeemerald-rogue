#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_BREATH].effect == EFFECT_PARALYZE_HIT);
    ASSUME(gBattleMoves[MOVE_DRAGON_BREATH].power == 70);
    ASSUME(gBattleMoves[MOVE_DRAGON_BREATH].type == TYPE_FIRE);
    ASSUME(gSpeciesInfo[SPECIES_DRATINI].types[0] == TYPE_DRAGON || gSpeciesInfo[SPECIES_DRATINI].types[1] == TYPE_DRAGON);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].types[0] == TYPE_NORMAL && gSpeciesInfo[SPECIES_SNORLAX].types[1] == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Dragon Breath is super effective against Dragon-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_BREATH); }
        OPPONENT(SPECIES_DRATINI);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); }
    } SCENE {
        MESSAGE("Wobbuffet used Dragon Breath!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_BREATH, player);
        MESSAGE("It's super effective!");
    }
}

SINGLE_BATTLE_TEST("Dragon Breath behaves normally against non-Dragon Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_BREATH); }
        OPPONENT(SPECIES_SNORLAX);
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); }
    } SCENE {
        MESSAGE("Wobbuffet used Dragon Breath!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_BREATH, player);
        NOT MESSAGE("It's super effective!");
    }
}
