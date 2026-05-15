#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLING].effect == EFFECT_FLING);
    ASSUME(gBattleMoves[MOVE_PRESENT].effect == EFFECT_PRESENT);
}

SINGLE_BATTLE_TEST("Drop Off uses Fling on switch-in if holding an item")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BOMBIRDIER) { Ability(ABILITY_BIG_PECKS); Item(ITEM_RAZOR_CLAW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DROP_OFF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Drop Off does not use Fling on switch-in without an item")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_BOMBIRDIER) { Ability(ABILITY_BIG_PECKS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DROP_OFF);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLING, player);
        }
    }
}

SINGLE_BATTLE_TEST("Drop Off uses Present before fainting")
{
    GIVEN {
        PLAYER(SPECIES_BOMBIRDIER) { HP(1); MaxHP(100); Ability(ABILITY_BIG_PECKS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        HP_BAR(player, hp: 0);
        ABILITY_POPUP(player, ABILITY_DROP_OFF);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PRESENT, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
