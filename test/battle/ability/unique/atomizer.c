#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Atomizer uses Misty Terrain after the user uses a healing move")
{
    GIVEN {
        PLAYER(SPECIES_AROMATISSE) { HP(50); MaxHP(100); Ability(ABILITY_AROMA_VEIL); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ABILITY_POPUP(player, ABILITY_ATOMIZER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Atomizer does not trigger after non-healing moves")
{
    GIVEN {
        PLAYER(SPECIES_AROMATISSE) { Ability(ABILITY_AROMA_VEIL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ATOMIZER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, player);
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}
