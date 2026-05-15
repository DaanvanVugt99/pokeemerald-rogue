#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CONVERSION].effect == EFFECT_CONVERSION);
    ASSUME(gBattleMoves[MOVE_CONVERSION_2].effect == EFFECT_CONVERSION_2);
    ASSUME(gBattleMoves[MOVE_PSYBEAM].power > 0);
    ASSUME(gBattleMoves[MOVE_SHARPEN].effect == EFFECT_ATTACK_UP);
}

SINGLE_BATTLE_TEST("Debug uses a random glitch move after Conversion")
{
    GIVEN {
        PLAYER(SPECIES_PORYGON) { Speed(100); Ability(ABILITY_ANALYTIC); UniqueAbility(ABILITY_DEBUG); Moves(MOVE_THUNDERBOLT, MOVE_CONVERSION); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONVERSION, WITH_RNG(RNG_ROGUE_DEBUG, MOVE_SHARPEN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONVERSION, player);
        ABILITY_POPUP(player, ABILITY_DEBUG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHARPEN, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Debug uses a random glitch move after Conversion 2")
{
    GIVEN {
        PLAYER(SPECIES_PORYGON2) { Speed(50); Ability(ABILITY_ANALYTIC); UniqueAbility(ABILITY_DEBUG); Moves(MOVE_CONVERSION_2); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CONVERSION_2, WITH_RNG(RNG_ROGUE_DEBUG, MOVE_PSYBEAM)); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CONVERSION_2, player);
        ABILITY_POPUP(player, ABILITY_DEBUG);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYBEAM, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
