#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sugar Rush raises Speed by 2 after consuming a Berry")
{
    GIVEN {
        PLAYER(SPECIES_SLURPUFF) { HP(100); MaxHP(100); Ability(ABILITY_SWEET_VEIL); UniqueAbility(ABILITY_SUGAR_RUSH); Item(ITEM_SITRUS_BERRY); Moves(MOVE_BELLY_DRUM); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BELLY_DRUM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Sugar Rush does not raise Speed when no Berry is consumed")
{
    GIVEN {
        PLAYER(SPECIES_SLURPUFF) { Ability(ABILITY_SWEET_VEIL); UniqueAbility(ABILITY_SUGAR_RUSH); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
