#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
    ASSUME(gBattleMoves[MOVE_DOUBLE_KICK].strikeCount == 2);
}

SINGLE_BATTLE_TEST("Tidal Switch reduces the incoming Pokemon's first-turn damage by 20 percent", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_SAMUROTT; uniqueAbility = ABILITY_TIDAL_SWITCH; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_TORRENT); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.8), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Tidal Switch halves the incoming Pokemon's first-turn damage in rain", s16 damage)
{
    u16 switchOutSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { switchOutSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { switchOutSpecies = SPECIES_SAMUROTT; uniqueAbility = ABILITY_TIDAL_SWITCH; }

    GIVEN {
        PLAYER(switchOutSpecies) { Ability(ABILITY_TORRENT); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_RAIN_DANCE, MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
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

SINGLE_BATTLE_TEST("Tidal Switch only reduces the first hit the incoming Pokemon takes")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_SAMUROTT) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_TIDAL_SWITCH); Moves(MOVE_CELEBRATE); }
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

SINGLE_BATTLE_TEST("Tidal Switch does not guard a replacement if the user faints instead of switching out", s16 damage)
{
    u16 playerSpecies;
    u16 uniqueAbility;

    PARAMETRIZE { playerSpecies = SPECIES_WOBBUFFET; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { playerSpecies = SPECIES_SAMUROTT; uniqueAbility = ABILITY_TIDAL_SWITCH; }

    GIVEN {
        PLAYER(playerSpecies) { HP(1); Ability(ABILITY_TORRENT); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
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

SINGLE_BATTLE_TEST("Tidal Switch announces from the switching Pokemon when the incoming Pokemon is guarded")
{
    GIVEN {
        PLAYER(SPECIES_SAMUROTT) { Ability(ABILITY_TORRENT); UniqueAbility(ABILITY_TIDAL_SWITCH); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TIDAL_SWITCH);
        MESSAGE("Wobbuffet was guarded by Tidal Switch!");
    }
}

SINGLE_BATTLE_TEST("Tidal Switch is the Samurott line's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_OSHAWOTT), ABILITY_TIDAL_SWITCH);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_DEWOTT), ABILITY_TIDAL_SWITCH);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SAMUROTT), ABILITY_TIDAL_SWITCH);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SAMUROTT_HISUIAN), ABILITY_TIDAL_SWITCH);
    }
}
