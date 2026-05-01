#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Life Blossom sets Misty Terrain on switch-in if no ally has fainted")
{
    GIVEN {
        PLAYER(SPECIES_XERNEAS) { Ability(ABILITY_FAIRY_AURA); UniqueAbility(ABILITY_LIFE_BLOSSOM); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PIKACHU);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT));
    }
}

SINGLE_BATTLE_TEST("Life Blossom does not set Misty Terrain on switch-in if an ally has fainted")
{
    GIVEN {
        PLAYER(SPECIES_XERNEAS) { Ability(ABILITY_FAIRY_AURA); UniqueAbility(ABILITY_LIFE_BLOSSOM); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PIKACHU) { HP(0); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_LIFE_BLOSSOM);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}
