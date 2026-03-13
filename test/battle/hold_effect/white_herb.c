#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_WHITE_HERB].holdEffect == HOLD_EFFECT_RESTORE_STATS);
}

SINGLE_BATTLE_TEST("White Herb restores stats when they're lowered")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_LEER].effect == EFFECT_DEFENSE_DOWN);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_LEER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's White Herb restored its status!");
    } THEN {
        EXPECT(player->item == ITEM_NONE);
        EXPECT(player->statStages[STAT_DEF] = DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("White Herb restores stats after Attack was lowered by Intimidate in singles")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { ; }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's White Herb restored its status!");
    } THEN {
        EXPECT(player->item == ITEM_NONE);
        EXPECT(player->statStages[STAT_DEF] = DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("White Herb restores stats after Attack was lowered by Intimidate in doubles")
{
    GIVEN {
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_WYNAUT) { Item(ITEM_WHITE_HERB); }
        PLAYER(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { ; }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_INTIMIDATE);

        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentLeft);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponentRight);

        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponentLeft);
        MESSAGE("Foe Wobbuffet's White Herb restored its status!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, opponentRight);
        MESSAGE("Foe Wynaut's White Herb restored its status!");
    } THEN {
        EXPECT(opponentLeft->item == ITEM_NONE);
        EXPECT(opponentLeft->statStages[STAT_DEF] = DEFAULT_STAT_STAGE);
        EXPECT(opponentRight->item == ITEM_NONE);
        EXPECT(opponentRight->statStages[STAT_DEF] = DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("White Herb restores stats after Attack was lowered by Intimidate while switching in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); }
        TURN { MOVE(player, MOVE_CLOSE_COMBAT); }
    } SCENE {
        // Turn 1: White Herb should be consumed by Intimidate.
        ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's White Herb restored its status!");

        // Turn 2: White Herb was already consumed, so it should not trigger again.
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CLOSE_COMBAT, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            MESSAGE("Wobbuffet's White Herb restored its status!");
        }
    } THEN {
        EXPECT(player->item == ITEM_NONE);
        EXPECT(player->statStages[STAT_DEF] = DEFAULT_STAT_STAGE - 1);
        EXPECT(player->statStages[STAT_SPDEF] = DEFAULT_STAT_STAGE - 1);
    }
}


SINGLE_BATTLE_TEST("White Herb restores stats after all hits of a multi hit move happened")
{
    u16 species;
    u16 ability;

    PARAMETRIZE { species = SPECIES_SLIGGOO_HISUIAN; ability = ABILITY_GOOEY; }
    PARAMETRIZE { species = SPECIES_DUGTRIO_ALOLAN; ability = ABILITY_TANGLING_HAIR; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_DUAL_WINGBEAT].strikeCount == 2);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(species) { Ability(ability); }
    } WHEN {
        TURN { MOVE(player, MOVE_DUAL_WINGBEAT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DUAL_WINGBEAT, player);
        ABILITY_POPUP(opponent, ability);
        MESSAGE("Wobbuffet's Speed fell!");
        ABILITY_POPUP(opponent, ability);
        MESSAGE("Wobbuffet's Speed fell!");
        MESSAGE("Wobbuffet's White Herb restored its status!");
    } THEN {
        EXPECT(player->item == ITEM_NONE);
        EXPECT(player->statStages[STAT_SPEED] = DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("White Herb does not activate if Knock Off removes it")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
        PLAYER(SPECIES_SLUGMA) {  Ability(ABILITY_WEAK_ARMOR); Item(ITEM_WHITE_HERB); MaxHP(400); HP(400); Defense(200); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_KNOCK_OFF); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_KNOCK_OFF, opponent);
        MESSAGE("Foe Wobbuffet knocked off Slugma's White Herb!");
        NONE_OF { MESSAGE("Slugma's White Herb restored its status!"); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("White Herb wont have time to activate if Magician steals it")
{
    GIVEN {
        PLAYER(SPECIES_SLUGMA) {  Ability(ABILITY_WEAK_ARMOR); Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_FENNEKIN) { Ability(ABILITY_MAGICIAN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            MESSAGE("Slugma's White Herb restored its status!");
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_WHITE_HERB);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("White Herb wont have time to activate if Pickpocket steals it")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SUPERPOWER].effect == EFFECT_SUPERPOWER);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_SNEASEL) { Ability(ABILITY_PICKPOCKET); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUPERPOWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUPERPOWER, player);
        ABILITY_POPUP(opponent, ABILITY_PICKPOCKET);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            MESSAGE("Wobbuffet's White Herb restored its status!");
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_WHITE_HERB);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("White Herb restores Attack after Competitive was triggered")
{
    GIVEN {
        PLAYER(SPECIES_IGGLYBUFF) { Ability(ABILITY_COMPETITIVE); Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { ; }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
        ABILITY_POPUP(player, ABILITY_COMPETITIVE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("White Herb does not activate after Defiant was triggered")
{
    GIVEN {
        PLAYER(SPECIES_MANKEY) { Ability(ABILITY_DEFIANT); Item(ITEM_WHITE_HERB); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); }
    } WHEN {
        TURN { ; }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_INTIMIDATE);
        ABILITY_POPUP(player, ABILITY_DEFIANT);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            MESSAGE("Mankey's White Herb restored its status!");
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_WHITE_HERB);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
