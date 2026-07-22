#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
}

SINGLE_BATTLE_TEST("Tidal Switch boosts the incoming Pokemon's first-turn damage by 20 percent", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_SAMUROTT; uniqueAbility = ABILITY_TIDAL_SWITCH; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_TORRENT); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
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

SINGLE_BATTLE_TEST("Tidal Switch boosts the incoming Pokemon's first-turn damage by 50 percent in rain", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_SAMUROTT; uniqueAbility = ABILITY_TIDAL_SWITCH; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_TORRENT); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_RAIN_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Tidal Switch expires after the incoming Pokemon's first turn", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_SAMUROTT; uniqueAbility = ABILITY_TIDAL_SWITCH; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_TORRENT); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
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

SINGLE_BATTLE_TEST("Tidal Switch announces from the switching Pokemon when the incoming Pokemon is charged")
{
    GIVEN {
        PLAYER(SPECIES_SAMUROTT) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_TIDAL_SWITCH); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TIDAL_SWITCH);
        MESSAGE("Wobbuffet was charged by Tidal Switch!");
    }
}

