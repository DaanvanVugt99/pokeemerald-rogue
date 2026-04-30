#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNARL].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].type == TYPE_NORMAL);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
}

SINGLE_BATTLE_TEST("Encore Aria uses Encore after the first successful sound move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_ARIA) { Speed(50); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_ENCORE_ARIA); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPLASH, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        ABILITY_POPUP(player, ABILITY_ENCORE_ARIA);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ENCORE, player);
    }
}

SINGLE_BATTLE_TEST("Encore Aria triggers only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_ARIA) { Speed(50); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_ENCORE_ARIA); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SNARL); FORCED_MOVE(opponent); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ENCORE_ARIA);
        NOT ABILITY_POPUP(player, ABILITY_ENCORE_ARIA);
    }
}

SINGLE_BATTLE_TEST("Encore Aria is not consumed when a sound move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_ARIA) { Speed(50); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_ENCORE_ARIA); Moves(MOVE_HYPER_VOICE, MOVE_SNARL); }
        OPPONENT(SPECIES_SABLEYE) { Speed(100); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_SPLASH); }
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        MESSAGE("It doesn't affect Foe Sableye…");
        ABILITY_POPUP(player, ABILITY_ENCORE_ARIA);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ENCORE, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
