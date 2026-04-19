#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Repellant badly poisons contact attackers")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_DUSTOX) { Ability(ABILITY_COMPOUND_EYES); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Repellant does not trigger on non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_DUSTOX) { Ability(ABILITY_COMPOUND_EYES); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_REPELLANT);
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_PSN, player);
        }
    } THEN {
        EXPECT(!(player->status1 & STATUS1_TOXIC_POISON));
    }
}
