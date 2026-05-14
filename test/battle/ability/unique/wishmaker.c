#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WISH].effect == EFFECT_WISH);
    ASSUME(gBattleMoves[MOVE_SWIFT].power > 0);
    ASSUME(gBattleMoves[MOVE_COSMIC_POWER].effect == EFFECT_COSMIC_POWER);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Wishmaker uses a random wish move after Wish")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_WISHMAKER); Moves(MOVE_WISH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WISH, WITH_RNG(RNG_ROGUE_WISHMAKER, MOVE_SWIFT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        ABILITY_POPUP(player, ABILITY_WISHMAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Wishmaker can choose a self-target wish move after Wish")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_WISHMAKER); Moves(MOVE_WISH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WISH, WITH_RNG(RNG_ROGUE_WISHMAKER, MOVE_COSMIC_POWER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        ABILITY_POPUP(player, ABILITY_WISHMAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COSMIC_POWER, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Wishmaker does not trigger after other moves")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_WISHMAKER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_WISHMAKER, MOVE_SWIFT)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WISHMAKER);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        }
    }
}
