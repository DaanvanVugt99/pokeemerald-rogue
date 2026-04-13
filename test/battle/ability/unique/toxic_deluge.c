#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GUNK_SHOT].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_GUNK_SHOT].accuracy < 100);
}

SINGLE_BATTLE_TEST("Toxic Deluge sets Acid Rain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_TOXIC_DELUGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Toxic Deluge makes Poison-type moves bypass accuracy in Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_TOXIC_DELUGE); Moves(MOVE_GUNK_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_GUNK_SHOT, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUNK_SHOT, player);
    }
}

SINGLE_BATTLE_TEST("Toxic Deluge does not bypass Poison accuracy when Acid Rain ends")
{
    GIVEN {
        PLAYER(SPECIES_MUK) { Speed(50); Ability(ABILITY_STICKY_HOLD); UniqueAbility(ABILITY_TOXIC_DELUGE); Moves(MOVE_GUNK_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SUNNY_DAY); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_GUNK_SHOT, hit: FALSE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GUNK_SHOT, player);
        }
    }
}
