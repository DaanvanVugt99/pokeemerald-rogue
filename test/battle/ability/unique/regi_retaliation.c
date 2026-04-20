#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TAUNT].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Stone Spikes chips the attacker when hit by a physical move")
{
    GIVEN {
        PLAYER(SPECIES_REGIROCK) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_STONE_SPIKES); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(160); MaxHP(160); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STONE_SPIKES);
    } THEN {
        EXPECT_EQ(opponent->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Icy Mirror chips the attacker when hit by a special move")
{
    GIVEN {
        PLAYER(SPECIES_REGICE) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_ICY_MIRROR); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(160); MaxHP(160); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ICY_MIRROR);
    } THEN {
        EXPECT_EQ(opponent->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Iron Maiden chips the attacker when hit by a status move")
{
    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_IRON_MAIDEN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(160); MaxHP(160); Moves(MOVE_TAUNT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TAUNT); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IRON_MAIDEN);
    } THEN {
        EXPECT_EQ(opponent->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Iron Maiden does not chip the attacker when the status move fails")
{
    GIVEN {
        PLAYER(SPECIES_REGISTEEL) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_IRON_MAIDEN); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(160); MaxHP(160); Moves(MOVE_TAUNT); }
    } WHEN {
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_TAUNT); }
    } THEN {
        EXPECT_EQ(opponent->hp, 160);
    }
}
