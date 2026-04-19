#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gItems[ITEM_NORMAL_GEM].holdEffect == HOLD_EFFECT_GEMS);
    ASSUME(gItems[ITEM_NORMAL_GEM].secondaryId == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Gemstash raises Defense when a Gem is used")
{
    GIVEN {
        PLAYER(SPECIES_SABLEYE) { Ability(ABILITY_KEEN_EYE); Item(ITEM_NORMAL_GEM); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_GEMSTASH);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Gemstash can generate a Gem matching one of the user's move types")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_GEMSTASH_PROC);
    GIVEN {
        PLAYER(SPECIES_SABLEYE) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NORMAL_GEM);
    }
}

SINGLE_BATTLE_TEST("Gemstash chooses a Gem from one of the user's move types")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_GEMSTASH_PROC);
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_GEMSTASH_TYPE);
    GIVEN {
        PLAYER(SPECIES_SABLEYE) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_SCRATCH, MOVE_SHADOW_SNEAK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->item == ITEM_NORMAL_GEM || player->item == ITEM_GHOST_GEM);
    }
}
