#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
}

SINGLE_BATTLE_TEST("Cud Chew eats a consumed Berry again on the next turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_CUD_CHEW); MaxHP(160); HP(80); Item(ITEM_SITRUS_BERRY); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        EXPECT_EQ(player->hp, 160);
    }
}

SINGLE_BATTLE_TEST("Unique Cud Chew eats a consumed Berry again on the next turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { MaxHP(160); HP(80); Item(ITEM_SITRUS_BERRY); UniqueAbility(ABILITY_CUD_CHEW); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
    } THEN {
        EXPECT_EQ(player->hp, 160);
    }
}
