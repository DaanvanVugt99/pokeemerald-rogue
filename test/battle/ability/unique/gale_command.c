#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Gale Command sets Tailwind after KO with two other Flying allies")
{
    GIVEN {
        PLAYER(SPECIES_TORNADUS_INCARNATE) { Speed(100); Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_GALE_COMMAND); Moves(MOVE_AIR_SLASH); }
        PLAYER(SPECIES_PIDOVE) { Speed(90); Ability(ABILITY_SUPER_LUCK); }
        PLAYER(SPECIES_WOOBAT) { Speed(80); Ability(ABILITY_UNAWARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_AIR_SLASH); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GALE_COMMAND);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}

SINGLE_BATTLE_TEST("Gale Command does not trigger without two other Flying allies")
{
    GIVEN {
        PLAYER(SPECIES_TORNADUS_INCARNATE) { Speed(100); Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_GALE_COMMAND); Moves(MOVE_AIR_SLASH); }
        PLAYER(SPECIES_SQUIRTLE) { Speed(90); Ability(ABILITY_TORRENT); }
        PLAYER(SPECIES_WOOBAT) { Speed(80); Ability(ABILITY_UNAWARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_AIR_SLASH); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_GALE_COMMAND);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}

SINGLE_BATTLE_TEST("Gale Command does not trigger if the target is not knocked out")
{
    GIVEN {
        PLAYER(SPECIES_TORNADUS_INCARNATE) { Speed(100); Ability(ABILITY_PRANKSTER); UniqueAbility(ABILITY_GALE_COMMAND); Moves(MOVE_AIR_SLASH); }
        PLAYER(SPECIES_PIDOVE) { Speed(90); Ability(ABILITY_SUPER_LUCK); }
        PLAYER(SPECIES_WOOBAT) { Speed(80); Ability(ABILITY_UNAWARE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(200); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_AIR_SLASH); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_GALE_COMMAND);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}
