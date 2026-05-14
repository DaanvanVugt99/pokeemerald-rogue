#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPIKY_SHIELD].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_LEECH_SEED].effect == EFFECT_LEECH_SEED);
    ASSUME(gBattleMoves[MOVE_NEEDLE_ARM].effect == EFFECT_FLINCH_HIT);
    ASSUME(gBattleMoves[MOVE_PIN_MISSILE].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_BULLET_SEED].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_COTTON_SPORE].effect == EFFECT_SPEED_DOWN_2);
    ASSUME(gBattleMoves[MOVE_INGRAIN].effect == EFFECT_INGRAIN);
    ASSUME(gBattleMoves[MOVE_BODY_PRESS].effect == EFFECT_BODY_PRESS);
    ASSUME(gBattleMoves[MOVE_VINE_WHIP].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_ROLLOUT].effect == EFFECT_ROLLOUT);
}

SINGLE_BATTLE_TEST("Bramble Guard uses a bramble move after Spiky Shield")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_BRAMBLE_GUARD); Moves(MOVE_SPIKY_SHIELD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKY_SHIELD, WITH_RNG(RNG_ROGUE_BRAMBLE_GUARD, MOVE_PIN_MISSILE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKY_SHIELD, player);
        ABILITY_POPUP(player, ABILITY_BRAMBLE_GUARD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PIN_MISSILE, player);
    }
}

SINGLE_BATTLE_TEST("Bramble Guard can choose self-targeted bramble moves")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_BRAMBLE_GUARD); Moves(MOVE_SPIKY_SHIELD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKY_SHIELD, WITH_RNG(RNG_ROGUE_BRAMBLE_GUARD, MOVE_INGRAIN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPIKY_SHIELD, player);
        ABILITY_POPUP(player, ABILITY_BRAMBLE_GUARD);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INGRAIN, player);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_ROOTED);
    }
}

SINGLE_BATTLE_TEST("Bramble Guard does not trigger after other protection moves")
{
    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_BRAMBLE_GUARD); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT, WITH_RNG(RNG_ROGUE_BRAMBLE_GUARD, MOVE_PIN_MISSILE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        NOT ABILITY_POPUP(player, ABILITY_BRAMBLE_GUARD);
    }
}

SINGLE_BATTLE_TEST("Bramble Guard chooses from every bramble move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_SPIKES,
        MOVE_LEECH_SEED,
        MOVE_NEEDLE_ARM,
        MOVE_PIN_MISSILE,
        MOVE_BULLET_SEED,
        MOVE_COTTON_SPORE,
        MOVE_INGRAIN,
        MOVE_BODY_PRESS,
        MOVE_VINE_WHIP,
        MOVE_ROLLOUT,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_BRAMBLE_GUARD);

    GIVEN {
        PLAYER(SPECIES_CHESNAUGHT) { Speed(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_BRAMBLE_GUARD); Moves(MOVE_SPIKY_SHIELD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKY_SHIELD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BRAMBLE_GUARD);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
