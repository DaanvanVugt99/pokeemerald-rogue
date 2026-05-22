#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ruminate restores 1/8 max HP at end of turn when below half HP")
{
    GIVEN {
        PLAYER(SPECIES_GOGOAT) { Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_RUMINATE); HP(79); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RUMINATE);
        MESSAGE("Gogoat's Ruminate restored its HP a little!");
        HP_BAR(player, damage: -20);
    }
}

SINGLE_BATTLE_TEST("Ruminate does not restore HP at exactly half HP")
{
    GIVEN {
        PLAYER(SPECIES_GOGOAT) { Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_RUMINATE); HP(80); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RUMINATE);
    } THEN {
        EXPECT_EQ(player->hp, 80);
    }
}

SINGLE_BATTLE_TEST("Ruminate is blocked by Heal Block")
{
    GIVEN {
        PLAYER(SPECIES_GOGOAT) { Ability(ABILITY_SAP_SIPPER); UniqueAbility(ABILITY_RUMINATE); HP(79); MaxHP(160); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HEAL_BLOCK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_HEAL_BLOCK); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RUMINATE);
    } THEN {
        EXPECT_EQ(player->hp, 79);
    }
}
