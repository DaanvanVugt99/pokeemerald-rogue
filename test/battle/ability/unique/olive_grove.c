#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_ENERGY_BALL].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_SWIFT].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Olive Grove sets Grassy Terrain when hit by an attack")
{
    GIVEN {
        PLAYER(SPECIES_SMOLIV) { Ability(ABILITY_EARLY_BIRD); UniqueAbility(ABILITY_OLIVE_GROVE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_OLIVE_GROVE);
        MESSAGE("Grass grew to cover the battlefield!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Olive Grove heals one quarter of Grass-type damage dealt in Grassy Terrain", s16 damage, u16 hp)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_RUN_AWAY; }
    PARAMETRIZE { uniqueAbility = ABILITY_OLIVE_GROVE; }

    GIVEN {
        PLAYER(SPECIES_SMOLIV) { HP(100); MaxHP(300); Ability(ABILITY_EARLY_BIRD); UniqueAbility(uniqueAbility); SpAttack(200); Moves(MOVE_GRASSY_TERRAIN, MOVE_ENERGY_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ENERGY_BALL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (uniqueAbility == ABILITY_OLIVE_GROVE)
            ABILITY_POPUP(player, ABILITY_OLIVE_GROVE);
        else
            NOT ABILITY_POPUP(player, ABILITY_OLIVE_GROVE);
    } THEN {
        results[i].hp = player->hp;
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[1].hp, results[0].hp + max(1, results[1].damage / 4));
    }
}

SINGLE_BATTLE_TEST("Olive Grove does not heal non-Grass moves in Grassy Terrain", u16 hp)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_RUN_AWAY; }
    PARAMETRIZE { uniqueAbility = ABILITY_OLIVE_GROVE; }

    GIVEN {
        PLAYER(SPECIES_SMOLIV) { HP(100); MaxHP(200); Ability(ABILITY_EARLY_BIRD); UniqueAbility(uniqueAbility); Moves(MOVE_GRASSY_TERRAIN, MOVE_SWIFT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_OLIVE_GROVE);
    } THEN {
        results[i].hp = player->hp;
    } FINALLY {
        EXPECT_EQ(results[0].hp, results[1].hp);
    }
}
