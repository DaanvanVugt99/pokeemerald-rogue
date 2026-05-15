#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAUNT].effect == EFFECT_TAUNT);
}

SINGLE_BATTLE_TEST("Raid Leader uses Taunt on switch-in with at least 2 other Steel-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PERRSERKER) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_RAID_LEADER); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGNEMITE) { Speed(40); Ability(ABILITY_MAGNET_PULL); }
        PLAYER(SPECIES_BRONZOR) { Speed(30); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RAID_LEADER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer != 0);
    }
}

SINGLE_BATTLE_TEST("Raid Leader works for Galarian Meowth")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MEOWTH_GALARIAN) { Speed(50); Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_RAID_LEADER); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGNEMITE) { Speed(40); Ability(ABILITY_MAGNET_PULL); }
        PLAYER(SPECIES_BRONZOR) { Speed(30); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RAID_LEADER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer != 0);
    }
}

SINGLE_BATTLE_TEST("Raid Leader does not use Taunt on switch-in without 2 other Steel-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PERRSERKER) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_RAID_LEADER); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_MAGNEMITE) { Speed(40); Ability(ABILITY_MAGNET_PULL); }
        PLAYER(SPECIES_PIKACHU) { Speed(30); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_RAID_LEADER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAUNT, player);
        }
    } THEN {
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 0);
    }
}
