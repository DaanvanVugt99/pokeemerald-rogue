#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_POISON_GAS].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
    ASSUME(gBattleMoves[MOVE_KNOCK_OFF].power > 30);
}

SINGLE_BATTLE_TEST("Kombo uses 30 BP Knock Off the first time the user uses a Poison move")
{
    GIVEN {
        PLAYER(SPECIES_DRAPION) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_KOMBO); Moves(MOVE_POISON_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_ORAN_BERRY); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_POISON_GAS, player);
        ABILITY_POPUP(player, ABILITY_KOMBO);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_ITEM_KNOCKOFF);
        MESSAGE("Drapion knocked off Foe Wobbuffet's Oran Berry!");
    } THEN {
        EXPECT_EQ(opponent->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Kombo does not consume its trigger on non-Poison moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAPION) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_KOMBO); Moves(MOVE_TACKLE, MOVE_POISON_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_KOMBO);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
    }
}

SINGLE_BATTLE_TEST("Kombo only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_DRAPION) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_KOMBO); Moves(MOVE_POISON_GAS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KOMBO);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_KOMBO);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        }
    }
}

SINGLE_BATTLE_TEST("Kombo refreshes after switching out")
{
    GIVEN {
        PLAYER(SPECIES_DRAPION) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_KOMBO); Moves(MOVE_POISON_GAS); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_REFRESH); }
    } WHEN {
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_REFRESH); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_REFRESH); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_REFRESH); }
        TURN { MOVE(player, MOVE_POISON_GAS); MOVE(opponent, MOVE_REFRESH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KOMBO);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
        ABILITY_POPUP(player, ABILITY_KOMBO);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, player);
    }
}

SINGLE_BATTLE_TEST("Kombo's called Knock Off uses reduced base power", s16 damage)
{
    u16 move;
    u16 uniqueAbility;

    PARAMETRIZE { move = MOVE_KNOCK_OFF; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_POISON_GAS; uniqueAbility = ABILITY_KOMBO; }

    GIVEN {
        PLAYER(SPECIES_DRAPION) { Attack(100); Ability(ABILITY_NO_GUARD); UniqueAbility(uniqueAbility); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
        EXPECT_GT(results[1].damage, 0);
    }
}
