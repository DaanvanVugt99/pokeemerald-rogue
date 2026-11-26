#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Heartthrob passes swapped stat boosts and can activate again")
{
    GIVEN {
        PLAYER(SPECIES_LUVDISC) { Speed(80); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_HEARTTHROB); Moves(MOVE_HEART_SWAP, MOVE_CELEBRATE); }
        PLAYER(SPECIES_ODDISH) { Speed(40); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Ability(ABILITY_TELEPATHY); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_HEART_SWAP); SEND_OUT(player, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_HEART_SWAP); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEART_SWAP, player);
        ABILITY_POPUP(player, ABILITY_HEARTTHROB);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
        MESSAGE("Go! Oddish!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEART_SWAP, player);
        ABILITY_POPUP(player, ABILITY_HEARTTHROB);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
        MESSAGE("Go! Oddish!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_ODDISH);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 4);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Heartthrob does not activate after a blocked Heart Swap")
{
    GIVEN {
        PLAYER(SPECIES_LUVDISC) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_HEARTTHROB); Moves(MOVE_HEART_SWAP); }
        PLAYER(SPECIES_ODDISH);
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_HEART_SWAP, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEARTTHROB);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
        }
    }
}

SINGLE_BATTLE_TEST("Heartthrob leaves Heart Swap usable without a reserve ally")
{
    GIVEN {
        PLAYER(SPECIES_LUVDISC) { Speed(80); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_HEARTTHROB); Moves(MOVE_HEART_SWAP); }
        OPPONENT(SPECIES_WYNAUT) { Speed(100); Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_HEART_SWAP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEART_SWAP, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEARTTHROB);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}
