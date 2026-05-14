#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
}

SINGLE_BATTLE_TEST("Witching Hour sets Trick Room after this Pokemon knocks out a target")
{
    GIVEN {
        PLAYER(SPECIES_HATTERENE) { Ability(ABILITY_MAGIC_BOUNCE); UniqueAbility(ABILITY_WITCHING_HOUR); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WITCHING_HOUR);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
    }
}

SINGLE_BATTLE_TEST("Witching Hour does not set Trick Room if the target is not knocked out")
{
    GIVEN {
        PLAYER(SPECIES_HATTERENE) { Ability(ABILITY_MAGIC_BOUNCE); UniqueAbility(ABILITY_WITCHING_HOUR); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WITCHING_HOUR);
        }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
    }
}
