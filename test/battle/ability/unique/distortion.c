#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_UPROAR].effect == EFFECT_UPROAR);
    ASSUME(gBattleMoves[MOVE_UPROAR].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].power > 0);
    ASSUME(gBattleMoves[MOVE_SCREECH].soundMove);
    ASSUME(IS_MOVE_STATUS(MOVE_SCREECH));
}

SINGLE_BATTLE_TEST("Distortion uses a random loud move after Uproar")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_UPROAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_UPROAR, WITH_RNG(RNG_ROGUE_DISTORTION, MOVE_HYPER_VOICE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_UPROAR, player);
        ABILITY_POPUP(player, ABILITY_DISTORTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp < gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].maxHP);
    }
}

SINGLE_BATTLE_TEST("Distortion can choose a status loud move after Uproar")
{
    PASSES_RANDOMLY(gBattleMoves[MOVE_SCREECH].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_UPROAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_UPROAR, WITH_RNG(RNG_ROGUE_DISTORTION, MOVE_SCREECH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_UPROAR, player);
        ABILITY_POPUP(player, ABILITY_DISTORTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCREECH, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Distortion does not trigger after other sound moves")
{
    GIVEN {
        PLAYER(SPECIES_EXPLOUD) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_DISTORTION); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE, WITH_RNG(RNG_ROGUE_DISTORTION, MOVE_SCREECH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DISTORTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCREECH, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
