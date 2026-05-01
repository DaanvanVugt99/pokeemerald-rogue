#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Flower Rite uses Wish after a status move if the party has 3 other Fairy-types")
{
    GIVEN {
        PLAYER(SPECIES_FLORGES_RED_FLOWER) { Speed(120); Ability(ABILITY_FLOWER_VEIL); UniqueAbility(ABILITY_FLOWER_RITUAL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_AZUMARILL) { Speed(90); Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_GRANBULL) { Speed(80); Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_CLEFAIRY) { Speed(70); Ability(ABILITY_CUTE_CHARM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FLOWER_RITUAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
    }
}

SINGLE_BATTLE_TEST("Flower Rite does not use Wish without 3 other Fairy-types in party")
{
    GIVEN {
        PLAYER(SPECIES_FLORGES_RED_FLOWER) { Speed(120); Ability(ABILITY_FLOWER_VEIL); UniqueAbility(ABILITY_FLOWER_RITUAL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_AZUMARILL) { Speed(90); Ability(ABILITY_THICK_FAT); }
        PLAYER(SPECIES_GRANBULL) { Speed(80); Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_PIDGEY) { Speed(70); Ability(ABILITY_KEEN_EYE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FLOWER_RITUAL);
    }
}
