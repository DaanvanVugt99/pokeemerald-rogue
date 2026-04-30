#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ICE_SHARD].type == TYPE_ICE);
    ASSUME(!IS_MOVE_STATUS(MOVE_ICE_SHARD));
}

SINGLE_BATTLE_TEST("Frostbite gives Ice-type moves a 10 percent freeze chance")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_FROSTBITE); Moves(MOVE_ICE_SHARD); }
        OPPONENT(SPECIES_POOCHYENA) { HP(999); MaxHP(999); Defense(255); Ability(ABILITY_RUN_AWAY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_SHARD, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FROSTBITE);
    } THEN {
        EXPECT_LT(opponent->hp, 999);
        EXPECT_NE(opponent->status1 & (STATUS1_FREEZE | STATUS1_FROSTBITE), 0);
    }
}

SINGLE_BATTLE_TEST("Frostbite boosts Ice-type move damage by 1.3x in Snow", s16 damage)
{
    bool32 hasSnow;

    PARAMETRIZE { hasSnow = FALSE; }
    PARAMETRIZE { hasSnow = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_FROSTBITE); Moves(MOVE_ICE_SHARD); }
        if (hasSnow)
            OPPONENT(SPECIES_NINETALES_ALOLAN) { Ability(ABILITY_SNOW_WARNING); }
        else
            OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICE_SHARD, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}
