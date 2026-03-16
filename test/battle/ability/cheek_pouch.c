#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
}

SINGLE_BATTLE_TEST("Cheek Pouch restores extra HP after eating a Berry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CHEEK_POUCH); MaxHP(160); HP(80); Item(ITEM_SITRUS_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHEEK_POUCH);
    } THEN {
        EXPECT_GT(player->hp, 80);
    }
}

SINGLE_BATTLE_TEST("Unique Cheek Pouch restores extra HP after eating a Berry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(80); Item(ITEM_SITRUS_BERRY); UniqueAbility(ABILITY_CHEEK_POUCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CHEEK_POUCH);
    } THEN {
        EXPECT_GT(player->hp, 80);
    }
}
