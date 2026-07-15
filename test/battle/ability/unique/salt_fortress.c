#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_IRON_DEFENSE].effect == EFFECT_DEFENSE_UP_2);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_BLOCK].effect == EFFECT_MEAN_LOOK);
}

SINGLE_BATTLE_TEST("Salt Fortress uses Block after Defense rises")
{
    GIVEN {
        PLAYER(SPECIES_GARGANACL) { Speed(100); UniqueAbility(ABILITY_SALT_FORTRESS); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ABILITY_POPUP(player, ABILITY_SALT_FORTRESS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BLOCK, player);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].battlerPreventingEscape,
                  GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
    }
}

SINGLE_BATTLE_TEST("Salt Fortress only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GARGANACL) { Speed(100); UniqueAbility(ABILITY_SALT_FORTRESS); Moves(MOVE_IRON_DEFENSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SALT_FORTRESS);
        NOT ABILITY_POPUP(player, ABILITY_SALT_FORTRESS);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 4);
    }
}

SINGLE_BATTLE_TEST("Salt Fortress resets after switching out")
{
    GIVEN {
        PLAYER(SPECIES_GARGANACL) { Speed(100); UniqueAbility(ABILITY_SALT_FORTRESS); Moves(MOVE_IRON_DEFENSE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_DEFENSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SALT_FORTRESS);
        ABILITY_POPUP(player, ABILITY_SALT_FORTRESS);
    }
}

SINGLE_BATTLE_TEST("Salt Fortress does not trigger for non-Defense boosts")
{
    GIVEN {
        PLAYER(SPECIES_GARGANACL) { Speed(100); UniqueAbility(ABILITY_SALT_FORTRESS); Moves(MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_SALT_FORTRESS);
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
    }
}
