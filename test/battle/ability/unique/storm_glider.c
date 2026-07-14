#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
    ASSUME(gBattleMoves[MOVE_CHARGE].effect == EFFECT_CHARGE);
    ASSUME(gBattleMoves[MOVE_GUST].windMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].windMove);
}

SINGLE_BATTLE_TEST("Storm Glider sets Tailwind the first time the Wattrel line is hit each switch-in")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_WATTREL; }
    PARAMETRIZE { species = SPECIES_KILOWATTREL; }

    GIVEN {
        PLAYER(species) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_STORM_GLIDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        MESSAGE("The tailwind blew from\nbehind your team!");
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT(gSideTimers[B_SIDE_PLAYER].tailwindTimer > 0);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP));
    }
}

SINGLE_BATTLE_TEST("Storm Glider does not use Charge when the first hit is a wind move")
{
    GIVEN {
        PLAYER(SPECIES_KILOWATTREL) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GUST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_STORM_GLIDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        MESSAGE("The tailwind blew from\nbehind your team!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Storm Glider's Tailwind still activates Wattrel's Wind Power")
{
    GIVEN {
        PLAYER(SPECIES_WATTREL) { Speed(50); Ability(ABILITY_WIND_POWER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
    }
}

SINGLE_BATTLE_TEST("Storm Glider only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_KILOWATTREL) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_GUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GUST); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STORM_GLIDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STORM_GLIDER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP));
    }
}

SINGLE_BATTLE_TEST("Storm Glider does not trigger if the user is not damaged")
{
    GIVEN {
        PLAYER(SPECIES_KILOWATTREL) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STORM_GLIDER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
