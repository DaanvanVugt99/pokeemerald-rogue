#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Colossal makes Slow Start end after 1 turn if the team has Steel, Ice, and Rock")
{
    GIVEN {
        PLAYER(SPECIES_REGIGIGAS) { Speed(100); Ability(ABILITY_SLOW_START); UniqueAbility(ABILITY_COLOSSAL); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_STEELIX) { Speed(1); Ability(ABILITY_STURDY); }
        PLAYER(SPECIES_JYNX) { Speed(1); Ability(ABILITY_OBLIVIOUS); }
        PLAYER(SPECIES_GEODUDE) { Speed(1); Ability(ABILITY_ROCK_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].slowStartTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Colossal does not shorten Slow Start without Steel, Ice, and Rock")
{
    GIVEN {
        PLAYER(SPECIES_REGIGIGAS) { Speed(100); Ability(ABILITY_SLOW_START); UniqueAbility(ABILITY_COLOSSAL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_STEELIX) { Speed(1); Ability(ABILITY_STURDY); }
        PLAYER(SPECIES_GEODUDE) { Speed(1); Ability(ABILITY_ROCK_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].slowStartTimer, 3);
    }
}
