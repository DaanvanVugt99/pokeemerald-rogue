#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WING_ATTACK].type == TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Tropical Canopy restores 1/16 HP at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_TROPIUS) { Ability(ABILITY_CHLOROPHYLL); HP(80); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TROPICAL_CANOPY);
        MESSAGE("Tropius's Tropical Canopy restored its HP a little!");
        HP_BAR(player, damage: -10);
    }
}

SINGLE_BATTLE_TEST("Tropical Canopy restores 1/8 HP in sun")
{
    GIVEN {
        PLAYER(SPECIES_TROPIUS) { Ability(ABILITY_CHLOROPHYLL); HP(80); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TROPICAL_CANOPY);
        MESSAGE("Tropius's Tropical Canopy restored its HP a little!");
        HP_BAR(player, damage: -20);
    }
}

SINGLE_BATTLE_TEST("Tropical Canopy boosts Flying-type move damage in sun", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_TROPICAL_CANOPY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE, MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SUNNY_DAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SUNNY_DAY); }
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}
