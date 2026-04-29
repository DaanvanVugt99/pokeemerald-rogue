#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GIGA_DRAIN].effect == EFFECT_ABSORB);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_FIRE_PLEDGE].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_TACKLE].split != SPLIT_STATUS);
    ASSUME(gSpeciesInfo[SPECIES_MAGIKARP].types[0] == TYPE_WATER || gSpeciesInfo[SPECIES_MAGIKARP].types[1] == TYPE_WATER);
    ASSUME(gSpeciesInfo[SPECIES_ODDISH].types[0] == TYPE_GRASS || gSpeciesInfo[SPECIES_ODDISH].types[1] == TYPE_GRASS);
    ASSUME(gSpeciesInfo[SPECIES_CHARMANDER].types[0] == TYPE_FIRE || gSpeciesInfo[SPECIES_CHARMANDER].types[1] == TYPE_FIRE);
}

SINGLE_BATTLE_TEST("Natural Flow boosts Grass move damage if the party contains a Water type", s16 damage)
{
    u16 teammate;

    PARAMETRIZE { teammate = SPECIES_MAGIKARP; }
    PARAMETRIZE { teammate = SPECIES_PIKACHU; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_NATURAL_FLOW); Moves(MOVE_MAGICAL_LEAF); }
        PLAYER(teammate) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MAGICAL_LEAF, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.2), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Natural Flow makes draining moves restore 1.5x HP if the party contains a Water type", s16 healed)
{
    u16 teammate;

    PARAMETRIZE { teammate = SPECIES_MAGIKARP; }
    PARAMETRIZE { teammate = SPECIES_PIKACHU; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_NATURAL_FLOW); HP(100); MaxHP(500); Moves(MOVE_GIGA_DRAIN); }
        PLAYER(teammate) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GIGA_DRAIN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].healed);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].healed, UQ_4_12(1.5), results[0].healed);
    }
}

SINGLE_BATTLE_TEST("Wild Ember boosts Fire move damage if the party contains a Grass type", s16 damage)
{
    u16 teammate;

    PARAMETRIZE { teammate = SPECIES_ODDISH; }
    PARAMETRIZE { teammate = SPECIES_PIKACHU; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_WILD_EMBER); Moves(MOVE_FIRE_PLEDGE); }
        PLAYER(teammate) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_PLEDGE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.2), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Wild Ember can burn if the party contains a Grass type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_WILD_EMBER); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_ODDISH) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_WILD_EMBER, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WILD_EMBER);
        ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_BRN, opponent);
        STATUS_ICON(opponent, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Wild Ember does not burn without a Grass type in the party")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_WILD_EMBER); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_PIKACHU) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_WILD_EMBER, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WILD_EMBER);
            STATUS_ICON(opponent, burn: TRUE);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Rush Current boosts Water move damage if the party contains a Fire type", s16 damage)
{
    u16 teammate;

    PARAMETRIZE { teammate = SPECIES_CHARMANDER; }
    PARAMETRIZE { teammate = SPECIES_PIKACHU; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_RUSH_CURRENT); Moves(MOVE_WATER_GUN); }
        PLAYER(teammate) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Ability(ABILITY_SHADOW_TAG); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.2), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Rush Current boosts Speed by 1.3x if the party contains a Fire type")
{
    u16 teammate;

    PARAMETRIZE { teammate = SPECIES_CHARMANDER; }
    PARAMETRIZE { teammate = SPECIES_PIKACHU; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(80); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_RUSH_CURRENT); Moves(MOVE_TACKLE); }
        PLAYER(teammate) { Speed(1); Ability(ABILITY_RUN_AWAY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (teammate == SPECIES_CHARMANDER)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        }
    }
}
