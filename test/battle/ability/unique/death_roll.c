#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Death Roll traps the target after the first biting move used each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE)   { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
    }
}

SINGLE_BATTLE_TEST("Death Roll does not trigger on non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE)   { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_ESCAPE_PREVENTION));
    }
}

SINGLE_BATTLE_TEST("Death Roll only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE)   { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
    }
}

DOUBLE_BATTLE_TEST("Death Roll traps the target of the biting move")
{
    GIVEN {
        PLAYER(SPECIES_FERALIGATR)  { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_BITE); }
        PLAYER(SPECIES_WOBBUFFET)   { Speed(40); Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_BITE, target: opponentRight); }
    } THEN {
        EXPECT(!(opponentLeft->status2 & STATUS2_ESCAPE_PREVENTION));
        EXPECT(opponentRight->status2 & STATUS2_ESCAPE_PREVENTION);
    }
}
