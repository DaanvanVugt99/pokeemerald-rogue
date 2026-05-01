#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].makesContact);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gBattleMoves[MOVE_FORESTS_CURSE].effect == EFFECT_THIRD_TYPE);
    ASSUME(gBattleMoves[MOVE_FORESTS_CURSE].argument == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_GRASS && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_GRASS);
}

SINGLE_BATTLE_TEST("Grave Grove uses Forest's Curse after being hit by a contact move")
{
    GIVEN {
        PLAYER(SPECIES_TREVENANT) { Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_GRAVE_GROVE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GRAVE_GROVE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FORESTS_CURSE, player);
    } THEN {
        EXPECT_EQ(opponent->type3, TYPE_GRASS);
    }
}

SINGLE_BATTLE_TEST("Grave Grove does not trigger after a non-contact move")
{
    GIVEN {
        PLAYER(SPECIES_TREVENANT) { Ability(ABILITY_NATURAL_CURE); UniqueAbility(ABILITY_GRAVE_GROVE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_GRAVE_GROVE);
    } THEN {
        EXPECT_EQ(opponent->type3, TYPE_MYSTERY);
    }
}

SINGLE_BATTLE_TEST("Grave Grove makes Ghost-type moves super effective against Grass-type Pokemon", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_GRAVE_GROVE; }
    PARAMETRIZE { uniqueAbility = ABILITY_MOONGLASS; }

    GIVEN {
        PLAYER(SPECIES_TREVENANT) { Ability(ABILITY_NATURAL_CURE); UniqueAbility(uniqueAbility); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_ODDISH);
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}
