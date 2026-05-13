#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEECH_SEED].effect == EFFECT_LEECH_SEED);
    ASSUME(gBattleMoves[MOVE_WORRY_SEED].effect == EFFECT_WORRY_SEED);
}

SINGLE_BATTLE_TEST("Messy Eater can use Leech Seed after eating a Berry")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_SPLIT_INSTINCT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GREEDENT) { Ability(ABILITY_CHEEK_POUCH); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM); UniqueAbility(ABILITY_MESSY_EATER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BELLY_DRUM); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MESSY_EATER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, opponent);
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Messy Eater can use Worry Seed after eating a Berry")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_SPLIT_INSTINCT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GREEDENT) { Ability(ABILITY_CHEEK_POUCH); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM); UniqueAbility(ABILITY_MESSY_EATER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BELLY_DRUM); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MESSY_EATER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WORRY_SEED, opponent);
    } THEN {
        EXPECT_EQ(player->ability, ABILITY_INSOMNIA);
    }
}
