#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_IRON_HEAD].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_MAGNET_RISE].effect == EFFECT_MAGNET_RISE);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Fault Finder uses Magnet Rise after the first Ground move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Fault Finder sets Stealth Rock after the first Steel move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_HEAD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
        EXPECT(gDisableStructs[B_POSITION_PLAYER_LEFT].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Fault Finder sets Stealth Rock if the Steel move knocks out the active target")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_HEAD); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Fault Finder uses Magnet Rise if the Ground move knocks out the active target")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(1); Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
    }
}

SINGLE_BATTLE_TEST("Fault Finder Ground and Steel triggers are independent")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_EARTHQUAKE, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_HEAD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Fault Finder uses Magnet Rise only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
        }
    }
}

SINGLE_BATTLE_TEST("Fault Finder sets Stealth Rock only once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_HEAD); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_HEAD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
        }
    }
}

SINGLE_BATTLE_TEST("Fault Finder does not consume on non-Ground or Steel moves")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO_ALOLAN) { Speed(100); Ability(ABILITY_TANGLING_HAIR); Moves(MOVE_TACKLE, MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FAULT_FINDER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNET_RISE, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_MAGNET_RISE);
    }
}
