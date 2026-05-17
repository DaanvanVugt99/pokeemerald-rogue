#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROAR].effect == EFFECT_ROAR);
    ASSUME(gBattleMoves[MOVE_DRAGON_TAIL].effect == EFFECT_HIT_SWITCH_TARGET);
}

SINGLE_BATTLE_TEST("Strange Guest blocks Roar for Ogerpon and its forms")
{
    u16 species;
    PARAMETRIZE { species = SPECIES_OGERPON_TEAL_MASK; }
    PARAMETRIZE { species = SPECIES_OGERPON_WELLSPRING_MASK; }
    PARAMETRIZE { species = SPECIES_OGERPON_HEARTHFLAME_MASK; }
    PARAMETRIZE { species = SPECIES_OGERPON_CORNERSTONE_MASK; }
    PARAMETRIZE { species = SPECIES_OGERPON_TEAL_MASK_TERA; }
    PARAMETRIZE { species = SPECIES_OGERPON_WELLSPRING_MASK_TERA; }
    PARAMETRIZE { species = SPECIES_OGERPON_HEARTHFLAME_MASK_TERA; }
    PARAMETRIZE { species = SPECIES_OGERPON_CORNERSTONE_MASK_TERA; }

    GIVEN {
        PLAYER(species);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ROAR); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ROAR); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Roar!");
        ABILITY_POPUP(player, ABILITY_STRANGE_GUEST);
        MESSAGE("Ogerpon anchors itself with Strange Guest!");
    } THEN {
        EXPECT_EQ(player->species, species);
    }
}

SINGLE_BATTLE_TEST("Strange Guest blocks Dragon Tail forced switches")
{
    GIVEN {
        PLAYER(SPECIES_OGERPON_TEAL_MASK);
        PLAYER(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_TAIL); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_TAIL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_TAIL, opponent);
        ABILITY_POPUP(player, ABILITY_STRANGE_GUEST);
        MESSAGE("Ogerpon anchors itself with Strange Guest!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_OGERPON_TEAL_MASK);
    }
}

SINGLE_BATTLE_TEST("Strange Guest makes Red Card fail after activation")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_RED_CARD); }
        OPPONENT(SPECIES_OGERPON_TEAL_MASK) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet held up its Red Card against Foe Ogerpon!");
        MESSAGE("Foe Ogerpon anchors itself with Strange Guest!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->species, SPECIES_OGERPON_TEAL_MASK);
    }
}
