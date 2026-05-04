#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Immunity prevents Poison Sting poison")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POISON_STING].effect == EFFECT_POISON_HIT);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORLAX) { Ability(ABILITY_IMMUNITY); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_STING); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_STING, player);
        NOT STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Immunity prevents Toxic bad poison")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORLAX) { Ability(ABILITY_IMMUNITY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC); }
    } SCENE {
        MESSAGE("Wobbuffet used Toxic!");
        ABILITY_POPUP(opponent, ABILITY_IMMUNITY);
        MESSAGE("Foe Snorlax's Immunity prevents poisoning!");
        NOT STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Immunity prevents Toxic Spikes poison")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SNORLAX) { Ability(ABILITY_IMMUNITY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Unique Immunity prevents Toxic Spikes poison")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TOXIC_SPIKES].effect == EFFECT_TOXIC_SPIKES);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_IMMUNITY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Unique Immunity prevents Toxic bad poison")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_IMMUNITY); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC); }
    } SCENE {
        MESSAGE("Wobbuffet used Toxic!");
        ABILITY_POPUP(opponent, ABILITY_IMMUNITY);
        MESSAGE("Foe Wobbuffet's Immunity prevents poisoning!");
        NOT STATUS_ICON(opponent, poison: TRUE);
    }
}

SINGLE_BATTLE_TEST("Immunity halves damage taken from Poison-type moves only", s16 damage)
{
    u32 move;
    u32 ability;

    PARAMETRIZE { move = MOVE_SLUDGE; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_SLUDGE; ability = ABILITY_IMMUNITY; }
    PARAMETRIZE { move = MOVE_WATER_GUN; ability = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; ability = ABILITY_IMMUNITY; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SLUDGE].type == TYPE_POISON);
        ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(0.5), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Immunity prevents acid rain damage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_POISON && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_POISON);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_BUG && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_BUG);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_IMMUNITY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ACID_RAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ACID_RAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet is scorched by acid rain!");
            HP_BAR(player);
        }
    }
}
