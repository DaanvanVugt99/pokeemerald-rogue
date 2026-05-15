#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
    ASSUME(gBattleMoves[MOVE_ACID].power > 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_AMNESIA].target == MOVE_TARGET_USER);
}

SINGLE_BATTLE_TEST("Acid Reflux uses a random stomach move after Stockpile")
{
    GIVEN {
        PLAYER(SPECIES_SWALOT) { Ability(ABILITY_LIQUID_OOZE); UniqueAbility(ABILITY_ACID_REFLUX); Moves(MOVE_STOCKPILE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STOCKPILE, WITH_RNG(RNG_ROGUE_ACID_REFLUX, MOVE_ACID)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        ABILITY_POPUP(player, ABILITY_ACID_REFLUX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Acid Reflux can choose a self-target stomach move after Stockpile")
{
    GIVEN {
        PLAYER(SPECIES_SWALOT) { Ability(ABILITY_LIQUID_OOZE); UniqueAbility(ABILITY_ACID_REFLUX); Moves(MOVE_STOCKPILE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_STOCKPILE, WITH_RNG(RNG_ROGUE_ACID_REFLUX, MOVE_AMNESIA)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STOCKPILE, player);
        ABILITY_POPUP(player, ABILITY_ACID_REFLUX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AMNESIA, player);
    }
}
