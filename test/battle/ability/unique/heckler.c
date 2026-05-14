#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(!gBattleMoves[MOVE_HYPER_VOICE].makesContact);
    ASSUME(!gBattleMoves[MOVE_CELEBRATE].soundMove);
    ASSUME(!gBattleMoves[MOVE_CELEBRATE].makesContact);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
}

SINGLE_BATTLE_TEST("Heckler uses Taunt after the first sound-based move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_OBSTAGOON) { Speed(100); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HECKLER); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        ABILITY_POPUP(player, ABILITY_HECKLER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    }
}

SINGLE_BATTLE_TEST("Heckler uses Taunt after the first contact move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_OBSTAGOON) { Speed(100); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HECKLER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_HECKLER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    }
}

SINGLE_BATTLE_TEST("Heckler only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_OBSTAGOON) { Speed(100); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HECKLER); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        ABILITY_POPUP(player, ABILITY_HECKLER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HECKLER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        }
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
