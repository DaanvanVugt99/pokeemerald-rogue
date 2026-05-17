#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MAGNET_RISE].effect == EFFECT_MAGNET_RISE);
    ASSUME(gBattleMoves[MOVE_SANDSTORM].effect == EFFECT_SANDSTORM);
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
}

SINGLE_BATTLE_TEST("Primal Current uses Magnet Rise when Sandstorm starts if Sandy Shocks is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SANDY_SHOCKS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_SCREAM_TAIL) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SANDSTORM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SANDSTORM); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SANDSTORM, opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_PRIMAL_CURRENT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_PRIMAL_CURRENT);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
            }
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
        if (hasOtherParadox)
            EXPECT_EQ(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE, 0);
        else
            EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
    }
}

SINGLE_BATTLE_TEST("Primal Current uses Magnet Rise when Electric Terrain starts if Sandy Shocks is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SANDY_SHOCKS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_GREAT_TUSK) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_ELECTRIC_TERRAIN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_PRIMAL_CURRENT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_PRIMAL_CURRENT);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
            }
        }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        if (hasOtherParadox)
            EXPECT_EQ(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE, 0);
        else
            EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
    }
}

SINGLE_BATTLE_TEST("Primal Current uses Magnet Rise when Sandy Shocks switches into active Sandstorm or Electric Terrain")
{
    u16 setupMove;

    PARAMETRIZE { setupMove = MOVE_SANDSTORM; }
    PARAMETRIZE { setupMove = MOVE_ELECTRIC_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SANDY_SHOCKS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGIKARP) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, setupMove, opponent);
        ABILITY_POPUP(player, ABILITY_PRIMAL_CURRENT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
    }
}
