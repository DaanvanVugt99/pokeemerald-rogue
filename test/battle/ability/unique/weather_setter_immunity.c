#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Acid Rain-setting unique abilities block its damage without healing non-Poison users")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_CREATION; }
    PARAMETRIZE { ability = ABILITY_METEOROLOGY; }
    PARAMETRIZE { ability = ABILITY_TOXIC_DELUGE; }
    PARAMETRIZE { ability = ABILITY_TOXISPHERE; }
    PARAMETRIZE { ability = ABILITY_TOXIC_MONSOON; }
    PARAMETRIZE { ability = ABILITY_ULTRA_FALLOUT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_POISON
            && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_POISON);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ACID_RAIN); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            UniqueAbility(ability);
            HP(80);
            MaxHP(160);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Foe Wobbuffet is scorched by acid rain!");
            MESSAGE("The acid rain restored Foe Wobbuffet's HP a little!");
        }
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
    }
}

SINGLE_BATTLE_TEST("Acid Rain-setting unique abilities still heal Poison users")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_CREATION; }
    PARAMETRIZE { ability = ABILITY_METEOROLOGY; }
    PARAMETRIZE { ability = ABILITY_TOXIC_DELUGE; }
    PARAMETRIZE { ability = ABILITY_TOXISPHERE; }
    PARAMETRIZE { ability = ABILITY_TOXIC_MONSOON; }
    PARAMETRIZE { ability = ABILITY_ULTRA_FALLOUT; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
        ASSUME(gSpeciesInfo[SPECIES_KOFFING].types[0] == TYPE_POISON
            || gSpeciesInfo[SPECIES_KOFFING].types[1] == TYPE_POISON);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_ACID_RAIN); }
        OPPONENT(SPECIES_KOFFING) {
            Ability(ABILITY_LEVITATE);
            UniqueAbility(ability);
            HP(80);
            MaxHP(160);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID_RAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("The acid rain restored Foe Koffing's HP a little!");
    } THEN {
        EXPECT_EQ(opponent->hp, 90);
    }
}

SINGLE_BATTLE_TEST("Sandstorm-setting unique abilities block its damage")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_CREATION; }
    PARAMETRIZE { ability = ABILITY_METEOROLOGY; }
    PARAMETRIZE { ability = ABILITY_DOMINION; }
    PARAMETRIZE { ability = ABILITY_GEODE_HEART; }
    PARAMETRIZE { ability = ABILITY_SAND_COMMAND; }
    PARAMETRIZE { ability = ABILITY_TYRANT_STORM; }

    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_ROCK
            && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_ROCK);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_GROUND
            && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_GROUND);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_STEEL
            && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_STEEL);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SANDSTORM); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            UniqueAbility(ability);
            HP(80);
            MaxHP(160);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_SANDSTORM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT MESSAGE("Foe Wobbuffet is buffeted by the sandstorm!");
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
    }
}

SINGLE_BATTLE_TEST("Hail-setting unique abilities block its damage")
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_CREATION; }
    PARAMETRIZE { ability = ABILITY_METEOROLOGY; }
    PARAMETRIZE { ability = ABILITY_COLD_SNAP; }
    PARAMETRIZE { ability = ABILITY_FROSTBITE_RITUAL; }
    PARAMETRIZE { ability = ABILITY_ICE_FLOE; }

    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_ICE
            && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_ICE);
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_HAIL); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            UniqueAbility(ability);
            HP(80);
            MaxHP(160);
            Moves(MOVE_CELEBRATE);
        }
    } WHEN {
        TURN { MOVE(player, MOVE_HAIL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT MESSAGE("Foe Wobbuffet is buffeted by the hail!");
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
    }
}
