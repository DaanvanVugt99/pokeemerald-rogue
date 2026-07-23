#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_CHARGE].effect == EFFECT_CHARGE);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Surge Protocol uses Charge after Iron Thorns's first Electric move if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_THORNS) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_THUNDER_SHOCK); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_MOTH) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_SURGE_PROTOCOL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_SURGE_PROTOCOL);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP));
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}

SINGLE_BATTLE_TEST("Surge Protocol sets Stealth Rock after Iron Thorns's first Rock move if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_THORNS) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_ROCK_THROW); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_BUNDLE) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_SURGE_PROTOCOL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_SURGE_PROTOCOL);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        }
        else
        {
            EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        }
    }
}

SINGLE_BATTLE_TEST("Surge Protocol Electric and Rock triggers are independent")
{
    GIVEN {
        PLAYER(SPECIES_IRON_THORNS) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_THUNDER_SHOCK, MOVE_ROCK_THROW); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SURGE_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SURGE_PROTOCOL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}
