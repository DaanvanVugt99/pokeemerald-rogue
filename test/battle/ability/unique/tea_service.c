#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TEATIME].effect == EFFECT_TEATIME);
    ASSUME(gBattleMoves[MOVE_NASTY_PLOT].target == MOVE_TARGET_USER);
    ASSUME(gItems[ITEM_LIECHI_BERRY].holdEffect == HOLD_EFFECT_ATTACK_UP);
}

SINGLE_BATTLE_TEST("Tea Service uses a random tea party move after this Pokemon uses Teatime")
{
    GIVEN {
        PLAYER(SPECIES_POLTEAGEIST) { Speed(100); Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_TEA_SERVICE); Moves(MOVE_TEATIME); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TEATIME, WITH_RNG(RNG_ROGUE_TEA_SERVICE, MOVE_NASTY_PLOT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TEATIME, player);
        ABILITY_POPUP(player, ABILITY_TEA_SERVICE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Tea Service chooses from every tea party move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_AROMATHERAPY,
        MOVE_SWEET_SCENT,
        MOVE_STRENGTH_SAP,
        MOVE_WILL_O_WISP,
        MOVE_CONFUSE_RAY,
        MOVE_GIGA_DRAIN,
        MOVE_SHADOW_BALL,
        MOVE_TRICK,
        MOVE_NASTY_PLOT,
        MOVE_RECOVER,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_TEA_SERVICE);

    GIVEN {
        PLAYER(SPECIES_POLTEAGEIST) { Speed(100); Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_TEA_SERVICE); Moves(MOVE_TEATIME); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Item(ITEM_LIECHI_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TEATIME); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TEA_SERVICE);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
