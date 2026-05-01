#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Silver Lining blocks major status moves")
{
    u32 move;
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_SPORE; }
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SILVER_LINING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SILVER_LINING);
        if (move == MOVE_TOXIC) {
            MESSAGE("Shuckle's Silver Lining prevents poisoning!");
        } else if (move == MOVE_SPORE) {
            MESSAGE("It doesn't affect Shuckle…");
        } else {
            MESSAGE("Shuckle's Silver Lining prevents burns!");
        }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Silver Lining restores 1/16 HP after taking a hit")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { HP(100); MaxHP(160); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SILVER_LINING); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->hp, 70);
    }
}

SINGLE_BATTLE_TEST("Silver Lining does not redirect Rocky Helmet or recoil damage to the holder")
{
    s16 attackDamage;

    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].effect == EFFECT_RECOIL_25);
    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].makesContact == TRUE);
    ASSUME(gItems[ITEM_ROCKY_HELMET].holdEffect == HOLD_EFFECT_ROCKY_HELMET);

    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { HP(150); MaxHP(160); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SILVER_LINING); Item(ITEM_ROCKY_HELMET); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(160); MaxHP(160); Ability(ABILITY_NO_GUARD); Moves(MOVE_TAKE_DOWN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAKE_DOWN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAKE_DOWN, opponent);
        HP_BAR(player, captureDamage: &attackDamage);
        HP_BAR(opponent);
        MESSAGE("Foe Wobbuffet is hit with recoil!");
    } THEN {
        EXPECT_EQ(player->hp, 150 - attackDamage + 10);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
