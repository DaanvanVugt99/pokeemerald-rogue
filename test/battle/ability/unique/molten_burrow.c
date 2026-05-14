#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FIRE_SPIN].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FIRE_SPIN].power > 0);
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
}

SINGLE_BATTLE_TEST("Molten Burrow burns on successful Fire-type hits during Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_CENTISKORCH) { Speed(200); Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_MOLTEN_BURROW); Moves(MOVE_FIRE_SPIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_INFESTED_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_INFESTED_TERRAIN); }
        TURN { MOVE(player, MOVE_FIRE_SPIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOLTEN_BURROW);
        STATUS_ICON(opponent, burn: TRUE);
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_BURN, STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Molten Burrow does not burn outside Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_CENTISKORCH) { Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_MOLTEN_BURROW); Moves(MOVE_FIRE_SPIN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_SPIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MOLTEN_BURROW);
        }
    } THEN {
        EXPECT_EQ(opponent->status1 & STATUS1_BURN, 0);
    }
}

SINGLE_BATTLE_TEST("Molten Burrow boosts Fire-type damage by 1.2x in Infested Terrain", s16 damage)
{
    bool32 hasInfestedTerrain;

    PARAMETRIZE { hasInfestedTerrain = FALSE; }
    PARAMETRIZE { hasInfestedTerrain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_CENTISKORCH) { Speed(200); Ability(ABILITY_FLASH_FIRE); UniqueAbility(ABILITY_MOLTEN_BURROW); Moves(MOVE_FIRE_SPIN, MOVE_CELEBRATE); }
        if (hasInfestedTerrain)
            OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_INFESTED_TERRAIN, MOVE_CELEBRATE); }
        else
            OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (hasInfestedTerrain)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_INFESTED_TERRAIN); }
        TURN { MOVE(player, MOVE_FIRE_SPIN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}
