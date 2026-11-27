#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WISH].effect == EFFECT_WISH);
    ASSUME(gBattleMoves[MOVE_SWIFT].power > 0);
    ASSUME(gBattleMoves[MOVE_COSMIC_POWER].effect == EFFECT_COSMIC_POWER);
}

SINGLE_BATTLE_TEST("Wishmaker uses a random wish move when Wish resolves at full HP")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_WISHMAKER); Moves(MOVE_WISH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WISH, WITH_RNG(RNG_ROGUE_WISHMAKER, MOVE_SWIFT)); }
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        MESSAGE("Jirachi's WISH came true!");
        ABILITY_POPUP(player, ABILITY_WISHMAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Wishmaker lets the healed switch-in use the follow-up move")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_WISHMAKER); Moves(MOVE_WISH); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WISH, WITH_RNG(RNG_ROGUE_WISHMAKER, MOVE_COSMIC_POWER)); }
        TURN { SWITCH(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_COSMIC_POWER, player);
        MESSAGE("Go! Wobbuffet!");
        MESSAGE("Jirachi's WISH came true!");
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_WISHMAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COSMIC_POWER, player);
    } THEN {
        EXPECT_GT(player->hp, 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
