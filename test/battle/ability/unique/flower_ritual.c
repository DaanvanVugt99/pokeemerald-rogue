#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Flower Ritual uses Wish after a status move if the party has 2 other Fairy types")
{
    GIVEN {
        PLAYER(SPECIES_FLORGES_RED_FLOWER) { Speed(120); Ability(ABILITY_FLOWER_VEIL); UniqueAbility(ABILITY_FLOWER_RITUAL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_AZUMARILL) { Speed(90); Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_GRANBULL) { Speed(80); Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOWER_RITUAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
    }
}

SINGLE_BATTLE_TEST("Flower Ritual uses Wish after a status move in Misty Terrain without Fairy allies")
{
    GIVEN {
        PLAYER(SPECIES_FLORGES_RED_FLOWER) { Speed(50); Ability(ABILITY_FLOWER_VEIL); UniqueAbility(ABILITY_FLOWER_RITUAL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_MISTY_TERRAIN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MISTY_TERRAIN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOWER_RITUAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
    }
}

SINGLE_BATTLE_TEST("Flower Ritual does not use Wish without 2 other Fairy types or Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_FLORGES_RED_FLOWER) { Speed(120); Ability(ABILITY_FLOWER_VEIL); UniqueAbility(ABILITY_FLOWER_RITUAL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_AZUMARILL) { Speed(90); Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_PIDGEY) { Speed(70); Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FLOWER_RITUAL);
    }
}
