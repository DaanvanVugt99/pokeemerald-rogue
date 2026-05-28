#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Puppet Master uses Copycat after a status move if the party shares a type")
{
    GIVEN {
        PLAYER(SPECIES_PECHARUNT) { Speed(1); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_GASTLY) { Speed(1); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TAIL_WHIP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_GROWL); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Tail Whip!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_PUPPET_MASTER);
        MESSAGE("Pecharunt used Copycat!");
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Puppet Master can trigger more than once")
{
    GIVEN {
        PLAYER(SPECIES_PECHARUNT) { Speed(1); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_GASTLY) { Speed(1); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TAIL_WHIP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_GROWL); }
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PUPPET_MASTER);
        ABILITY_POPUP(player, ABILITY_PUPPET_MASTER);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Puppet Master does not trigger if a teammate does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_PECHARUNT) { Speed(1); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_GROWL); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TAIL_WHIP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_GROWL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PUPPET_MASTER);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Puppet Master does not trigger after a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_PECHARUNT) { Speed(1); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_GASTLY) { Speed(1); Ability(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TAIL_WHIP); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAIL_WHIP); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PUPPET_MASTER);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}
