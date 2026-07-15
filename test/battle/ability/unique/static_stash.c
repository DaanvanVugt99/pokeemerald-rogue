#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CHARGE].effect == EFFECT_CHARGE);
    ASSUME(gBattleMoves[MOVE_BUG_BITE].effect == EFFECT_BUG_BITE);
    ASSUME(gBattleMoves[MOVE_RECYCLE].effect == EFFECT_RECYCLE);
    ASSUME(gItems[ITEM_SITRUS_BERRY].holdEffect == HOLD_EFFECT_RESTORE_PCT_HP);
    ASSUME(gItems[ITEM_ELECTRIC_SEED].holdEffect == HOLD_EFFECT_SEEDS);
    ASSUME(gItems[ITEM_ELECTRIC_SEED].holdEffectParam == HOLD_EFFECT_PARAM_ELECTRIC_TERRAIN);
    ASSUME(gItems[ITEM_INFESTED_SEED].holdEffect == HOLD_EFFECT_SEEDS);
    ASSUME(gItems[ITEM_INFESTED_SEED].holdEffectParam == HOLD_EFFECT_PARAM_INFESTED_TERRAIN);
}

SINGLE_BATTLE_TEST("Static Stash uses Charge after Emolga consumes a Berry with Bug Bite")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { HP(75); MaxHP(100); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_STASH); Moves(MOVE_BUG_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_SITRUS_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, player);
        MESSAGE("Emolga's Sitrus Berry restored health!");
        ABILITY_POPUP(player, ABILITY_STATIC_STASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
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

SINGLE_BATTLE_TEST("Static Stash uses Charge after Emolga consumes an Infested Seed")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_STASH); Item(ITEM_INFESTED_SEED); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_INFESTED_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_INFESTED_TERRAIN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_INFESTED_TERRAIN, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Emolga used Infested Seed to get pumped!");
        ABILITY_POPUP(player, ABILITY_STATIC_STASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(player->status2 & STATUS2_FOCUS_ENERGY);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_CHARGED_UP);
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

SINGLE_BATTLE_TEST("Static Stash does not activate when a recycled Berry is removed")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { HP(100); MaxHP(100); Speed(50); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_STASH); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM, MOVE_RECYCLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE, MOVE_KNOCK_OFF); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECYCLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_KNOCK_OFF); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STATIC_STASH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECYCLE, player);
        MESSAGE("Emolga found one Sitrus Berry!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_STATIC_STASH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}
