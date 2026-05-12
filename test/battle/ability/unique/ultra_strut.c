#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].effect != EFFECT_CONFUSE_HIT);
}

SINGLE_BATTLE_TEST("Ultra Strut confuses a full-HP target with its first move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_PHEROMOSA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_ULTRA_STRUT);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_CONFUSION);
    }
}

SINGLE_BATTLE_TEST("Ultra Strut does not confuse targets that were not at full HP")
{
    GIVEN {
        PLAYER(SPECIES_PHEROMOSA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(90); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_STRUT);
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_CONFUSION));
    }
}

SINGLE_BATTLE_TEST("Ultra Strut only checks the first successful move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_PHEROMOSA) { Ability(ABILITY_BEAST_BOOST); Attack(1); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(90); MaxHP(100); Defense(999); Speed(1); Moves(MOVE_RECOVER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_RECOVER); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_STRUT);
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_CONFUSION));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Ultra Strut does not confuse if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_PHEROMOSA) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_STRUT);
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_CONFUSION));
    }
}
