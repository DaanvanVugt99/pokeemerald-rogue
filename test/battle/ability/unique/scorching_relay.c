#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SUNNY_DAY].effect == EFFECT_SUNNY_DAY);
    ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].strikeCount == 2);
}

SINGLE_BATTLE_TEST("Scorching Relay reduces the incoming Pokemon's first hit by 20 percent", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_EMBOAR; uniqueAbility = ABILITY_SCORCHING_RELAY; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.8), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay halves the incoming Pokemon's first hit in sunlight", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_EMBOAR; uniqueAbility = ABILITY_SCORCHING_RELAY; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SUNNY_DAY, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay expires after the incoming Pokemon's first turn", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_EMBOAR; uniqueAbility = ABILITY_SCORCHING_RELAY; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay only reduces the first hit the incoming Pokemon takes")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_EMBOAR) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SCORCHING_RELAY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DOUBLE_KICK); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_DOUBLE_KICK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, opponent);
        HP_BAR(player, captureDamage: &firstHit);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_KICK, opponent);
        HP_BAR(player, captureDamage: &secondHit);
    } THEN {
        EXPECT_MUL_EQ(firstHit, UQ_4_12(1.25), secondHit);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay does not guard a replacement if the user faints instead of switching out", s16 damage)
{
    u16 playerSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { playerSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { playerSpecies = SPECIES_EMBOAR; uniqueAbility = ABILITY_SCORCHING_RELAY; }

    GIVEN {
        PLAYER(playerSpecies) { HP(1); Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_QUICK_ATTACK, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay announces from the switching Pokemon when the incoming Pokemon is guarded")
{
    GIVEN {
        PLAYER(SPECIES_EMBOAR) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SCORCHING_RELAY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SCORCHING_RELAY);
        MESSAGE("Wobbuffet was guarded by Scorch Relay!");
    }
}

