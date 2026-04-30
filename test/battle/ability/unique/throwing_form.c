#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CIRCLE_THROW].effect == EFFECT_HIT_SWITCH_TARGET);
    ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
}

SINGLE_BATTLE_TEST("Throwing Form boosts forced-switch damage by 30 percent", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_THROWING_FORM; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_CIRCLE_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CIRCLE_THROW); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Throwing Form makes forced-switch moves ignore accuracy checks")
{
    u16 uniqueAbility;
    u32 expectedAccuracy = 0;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; expectedAccuracy = gBattleMoves[MOVE_CIRCLE_THROW].accuracy * 3 / 4; }
    PARAMETRIZE { uniqueAbility = ABILITY_THROWING_FORM; expectedAccuracy = 100; }

    PASSES_RANDOMLY(expectedAccuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_CIRCLE_THROW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DOUBLE_TEAM); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_CIRCLE_THROW); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CIRCLE_THROW, player);
    }
}

SINGLE_BATTLE_TEST("Throwing Form is Throh's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_THROH), ABILITY_THROWING_FORM);
    }
}
