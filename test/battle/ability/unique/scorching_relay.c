#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SUNNY_DAY].effect == EFFECT_SUNNY_DAY);
}

SINGLE_BATTLE_TEST("Scorching Relay boosts the incoming Pokemon's first-turn damage by 20 percent", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_EMBOAR; uniqueAbility = ABILITY_SCORCHING_RELAY; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay boosts the incoming Pokemon's first-turn damage by 50 percent in sunlight", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_EMBOAR; uniqueAbility = ABILITY_SCORCHING_RELAY; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_BLAZE); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
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
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Scorching Relay announces from the switching Pokemon when the incoming Pokemon is charged")
{
    GIVEN {
        PLAYER(SPECIES_EMBOAR) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SCORCHING_RELAY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SCORCHING_RELAY);
        MESSAGE("Wobbuffet was charged by Scorch Relay!");
    }
}

