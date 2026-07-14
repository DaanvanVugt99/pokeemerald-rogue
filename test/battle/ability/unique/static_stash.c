#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CHARGE].effect == EFFECT_CHARGE);
    ASSUME(gItems[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
    ASSUME(gItems[ITEM_ELECTRIC_SEED].holdEffect == HOLD_EFFECT_SEEDS);
    ASSUME(gItems[ITEM_ELECTRIC_SEED].holdEffectParam == HOLD_EFFECT_PARAM_ELECTRIC_TERRAIN);
}

SINGLE_BATTLE_TEST("Static Stash uses Charge after Emolga consumes a Berry")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { HP(100); MaxHP(100); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_STASH); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BELLY_DRUM, player);
        ABILITY_POPUP(player, ABILITY_STATIC_STASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        MESSAGE("Emolga began charging power!");
    } THEN {
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Static Stash uses Charge after Emolga consumes a terrain seed")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_STASH); Item(ITEM_ELECTRIC_SEED); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ELECTRIC_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ELECTRIC_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Using Electric Seed, the Defense of Emolga rose!");
        ABILITY_POPUP(player, ABILITY_STATIC_STASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Static Stash does not activate when Emolga loses a seed without consuming it")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_STASH); Item(ITEM_ELECTRIC_SEED); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_KNOCK_OFF); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STATIC_STASH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP));
    }
}
