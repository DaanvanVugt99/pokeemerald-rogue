#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Curtain Call clears boosts on entry and sets snow on a manual switch-out")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Speed(50); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_JYNX) { Speed(100); Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_CURTAIN_CALL); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_CALM_MIND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CALM_MIND); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponent);
        MESSAGE("Go! Jynx!");
        ABILITY_POPUP(player, ABILITY_CURTAIN_CALL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE);
        ABILITY_POPUP(player, ABILITY_CURTAIN_CALL);
        MESSAGE("It started to snow!");
        MESSAGE("Go! Oddish!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
    }
}

SINGLE_BATTLE_TEST("Curtain Call sets snow when its holder pivots out")
{
    u16 move;
    PARAMETRIZE { move = MOVE_BATON_PASS; }
    PARAMETRIZE { move = MOVE_U_TURN; }

    GIVEN {
        PLAYER(SPECIES_JYNX) { Speed(100); Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_CURTAIN_CALL); Moves(move); }
        PLAYER(SPECIES_ODDISH) { Speed(50); }
        OPPONENT(SPECIES_WYNAUT) { Speed(30); HP(1000); MaxHP(1000); Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE);
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ABILITY_POPUP(player, ABILITY_CURTAIN_CALL);
        MESSAGE("It started to snow!");
        MESSAGE("Go! Oddish!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
    }
}

SINGLE_BATTLE_TEST("Curtain Call does not set snow when its holder faints")
{
    GIVEN {
        PLAYER(SPECIES_JYNX) { HP(1); Speed(50); Ability(ABILITY_OBLIVIOUS); UniqueAbility(ABILITY_CURTAIN_CALL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_ODDISH) { Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HAZE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT MESSAGE("It started to snow!");
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SNOW));
    }
}
