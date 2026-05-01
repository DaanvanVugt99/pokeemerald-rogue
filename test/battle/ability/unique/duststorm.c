#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STUN_SPORE].powderMove);
    ASSUME(gBattleMoves[MOVE_POISON_POWDER].powderMove);
    ASSUME(!gBattleMoves[MOVE_CELEBRATE].powderMove);
    ASSUME(gBattleMoves[MOVE_MIST].effect == EFFECT_MIST);
    ASSUME(gBattleMoves[MOVE_MISTY_TERRAIN].effect == EFFECT_MISTY_TERRAIN);
}

SINGLE_BATTLE_TEST("Duststorm uses Mist and Misty Terrain after the first powder move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_VIVILLON) { Ability(ABILITY_NO_GUARD); Moves(MOVE_STUN_SPORE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STUN_SPORE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STUN_SPORE, player);
        ABILITY_POPUP(player, ABILITY_DUSTSTORM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MIST, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_MIST);
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Duststorm does not trigger a second time in the same switch-in")
{
    GIVEN {
        PLAYER(SPECIES_VIVILLON) { Ability(ABILITY_NO_GUARD); Moves(MOVE_STUN_SPORE, MOVE_POISON_POWDER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STUN_SPORE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POISON_POWDER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DUSTSTORM);
        NOT ABILITY_POPUP(player, ABILITY_DUSTSTORM);
    }
}

SINGLE_BATTLE_TEST("Duststorm does not trigger from non-powder moves")
{
    GIVEN {
        PLAYER(SPECIES_VIVILLON) { Ability(ABILITY_NO_GUARD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DUSTSTORM);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MIST, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MISTY_TERRAIN, player);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_MIST));
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
