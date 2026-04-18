#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
    ASSUME(gItems[ITEM_AIR_BALLOON].holdEffect == HOLD_EFFECT_AIR_BALLOON);
}

SINGLE_BATTLE_TEST("Lightning Field sets permanent Electric Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_RAIKOU) { Ability(ABILITY_PRESSURE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Lightning Field upgrades active Electric Terrain to permanent on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_ELECTRIC_TERRAIN); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_LIGHTNING_FIELD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT);
    }
}

SINGLE_BATTLE_TEST("Lightning Field halves the user's Sp. Def while Electric Terrain is active", s16 damage)
{
    bool32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = FALSE; }
    PARAMETRIZE { uniqueAbility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility ? ABILITY_LIGHTNING_FIELD : ABILITY_NONE); Moves(MOVE_CELEBRATE); HP(300); }
        OPPONENT(SPECIES_ALAKAZAM) { Moves(uniqueAbility ? MOVE_CELEBRATE : MOVE_ELECTRIC_TERRAIN, MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, uniqueAbility ? MOVE_CELEBRATE : MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Lightning Field does not halve Sp. Def while the user is not affected by Electric Terrain", s16 damage)
{
    bool32 airborne;

    PARAMETRIZE { airborne = FALSE; }
    PARAMETRIZE { airborne = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            UniqueAbility(ABILITY_LIGHTNING_FIELD);
            Item(airborne ? ITEM_AIR_BALLOON : ITEM_NONE);
            Moves(MOVE_CELEBRATE);
            HP(300);
        }
        OPPONENT(SPECIES_ALAKAZAM) { Moves(MOVE_CELEBRATE, MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}
