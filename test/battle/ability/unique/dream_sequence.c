#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy > 0);
    ASSUME(gBattleMoves[MOVE_DREAM_EATER].power > 0);
    ASSUME(IS_MOVE_STATUS(MOVE_CALM_MIND));
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
}

SINGLE_BATTLE_TEST("Dream Sequence uses a random dream move after Hypnosis")
{
    s16 damage;
    PASSES_RANDOMLY(gBattleMoves[MOVE_HYPNOSIS].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_HYPNO) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAM_SEQUENCE); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ROGUE_DREAM_SEQUENCE, MOVE_DREAM_EATER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ABILITY_POPUP(player, ABILITY_DREAM_SEQUENCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DREAM_EATER, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_GT(damage, 0);
    }
}

SINGLE_BATTLE_TEST("Dream Sequence can choose a self-target dream move after Hypnosis")
{
    PASSES_RANDOMLY(gBattleMoves[MOVE_HYPNOSIS].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_HYPNO) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAM_SEQUENCE); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ROGUE_DREAM_SEQUENCE, MOVE_CALM_MIND)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        ABILITY_POPUP(player, ABILITY_DREAM_SEQUENCE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Dream Sequence does not trigger after other status moves")
{
    GIVEN {
        PLAYER(SPECIES_HYPNO) { Ability(ABILITY_INSOMNIA); UniqueAbility(ABILITY_DREAM_SEQUENCE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL, WITH_RNG(RNG_ROGUE_DREAM_SEQUENCE, MOVE_DREAM_EATER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DREAM_SEQUENCE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DREAM_EATER, player);
        }
    }
}
