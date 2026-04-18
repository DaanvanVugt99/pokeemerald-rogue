#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Honey Rage makes the next damaging move heal 1/4 of damage dealt after a Berry is consumed")
{
    GIVEN {
        PLAYER(SPECIES_URSARING) { HP(80); MaxHP(160); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HONEY_RAGE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 90);
    }
}

SINGLE_BATTLE_TEST("Honey Rage is consumed after one damaging move")
{
    GIVEN {
        PLAYER(SPECIES_URSARING) { HP(80); MaxHP(160); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HONEY_RAGE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 90);
    }
}

SINGLE_BATTLE_TEST("Honey Rage is not consumed by a damaging move that misses")
{
    GIVEN {
        PLAYER(SPECIES_URSARING) { HP(80); MaxHP(160); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HONEY_RAGE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_DRAGON_RAGE, MOVE_GUNK_SHOT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUNK_SHOT, hit: FALSE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_GUNK_SHOT, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 90);
    }
}

SINGLE_BATTLE_TEST("Honey Rage works with Counter")
{
    GIVEN {
        PLAYER(SPECIES_URSARING) { HP(80); MaxHP(160); Ability(ABILITY_GUTS); UniqueAbility(ABILITY_HONEY_RAGE); Item(ITEM_SITRUS_BERRY); Moves(MOVE_COUNTER, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_COUNTER); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_GT(player->hp, 70);
    }
}
