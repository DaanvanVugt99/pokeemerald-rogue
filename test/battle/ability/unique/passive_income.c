#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_FURY_SWIPES].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_PAY_DAY].power > 0);
    ASSUME(gBattleMoves[MOVE_SWIFT].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_FAKE_OUT].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Passive Income applies Embargo before Pay Day and prevents Sitrus Berry from activating")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Level(50); Attack(80); Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(210); MaxHP(400); Defense(100); Item(ITEM_SITRUS_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        NOT MESSAGE("Foe Wobbuffet's Sitrus Berry restored health!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        NOT MESSAGE("Foe Wobbuffet's Sitrus Berry restored health!");
        ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        MESSAGE("Foe Wobbuffet can't use items anymore!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player);
        HP_BAR(opponent, captureDamage: &secondHit);
        MESSAGE("Coins scattered everywhere!");
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
        EXPECT_LT(opponent->hp, opponent->maxHP / 2);
        EXPECT_EQ(opponent->hp, 210 - firstHit - secondHit);
        EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_EMBARGO);
        EXPECT_EQ(opponent->item, ITEM_SITRUS_BERRY);
    }
}

SINGLE_BATTLE_TEST("Passive Income waits for damage and refreshes after switching")
{
    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Speed(100); Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Moves(MOVE_GROWL, MOVE_WATER_GUN, MOVE_SWIFT); }
        PLAYER(SPECIES_ODDISH) { Speed(20); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1000); MaxHP(1000); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SWIFT, hit: FALSE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NOT ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        MESSAGE("Foe Wobbuffet can't use items anymore!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        NOT ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        MESSAGE("Go! Oddish!");
        MESSAGE("Go! Meowth!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player);
    }
}

SINGLE_BATTLE_TEST("Passive Income adds Pay Day after Fury Swipes")
{
    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Moves(MOVE_FURY_SWIPES); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FURY_SWIPES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FURY_SWIPES, player);
        ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player);
    }
}

SINGLE_BATTLE_TEST("Passive Income popup is shown on the user after Fake Out")
{
    GIVEN {
        PLAYER(SPECIES_INTELEON) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Speed(100); Moves(MOVE_FAKE_OUT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FAKE_OUT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        HP_BAR(player);
        ABILITY_POPUP(opponent, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, opponent);
    }
}
