#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Klutz removes the target's held item when hitting it", s16 damage)
{
    u32 ability;

    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_KLUTZ; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power != 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_POTION); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (ability == ABILITY_KLUTZ) {
            ABILITY_POPUP(player, ABILITY_KLUTZ);
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
            MESSAGE("Wobbuffet knocked off Foe Wobbuffet's Potion!");
        }
    } THEN {
        if (ability == ABILITY_KLUTZ)
            EXPECT_EQ(opponent->item, ITEM_NONE);
        else
            EXPECT_EQ(opponent->item, ITEM_POTION);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Klutz does not remove items if the move fails to damage the target")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_CELEBRATE].power == 0);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_POTION); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
            MESSAGE("Wobbuffet knocked off Foe Wobbuffet's Potion!");
        }
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_POTION);
    }
}

SINGLE_BATTLE_TEST("Klutz removes the target's held item after another secondary effect")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SMACK_DOWN].effect == EFFECT_SMACK_DOWN);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Moves(MOVE_SMACK_DOWN); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_POTION); }
    } WHEN {
        TURN { MOVE(player, MOVE_SMACK_DOWN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, player);
        ABILITY_POPUP(player, ABILITY_KLUTZ);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
        MESSAGE("Wobbuffet knocked off Foe Wobbuffet's Potion!");
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}
