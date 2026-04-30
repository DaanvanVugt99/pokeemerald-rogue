#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAIL_SLAP].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect != EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
    ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
}

SINGLE_BATTLE_TEST("Spring Clean clears Spikes, Toxic Spikes, Stealth Rock, and Sticky Web from its side after a multi-strike move")
{
    GIVEN {
        PLAYER(SPECIES_CINCCINO) { Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SPRING_CLEAN); Speed(1); Moves(MOVE_CELEBRATE, MOVE_TAIL_SLAP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_SPIKES, MOVE_TOXIC_SPIKES, MOVE_STEALTH_ROCK, MOVE_STICKY_WEB); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STICKY_WEB); }
        TURN { MOVE(player, MOVE_TAIL_SLAP); MOVE(opponent, MOVE_STICKY_WEB); }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TOXIC_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STICKY_WEB));
    }
}

SINGLE_BATTLE_TEST("Spring Clean does not clear hazards after a non-multi-strike move")
{
    GIVEN {
        PLAYER(SPECIES_CINCCINO) { Ability(ABILITY_CUTE_CHARM); UniqueAbility(ABILITY_SPRING_CLEAN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(200); Moves(MOVE_STEALTH_ROCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SPRING_CLEAN);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK);
    }
}
