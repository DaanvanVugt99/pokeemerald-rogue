#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Restorative Aura heals all active battlers by 1/8 max HP at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) { Ability(ABILITY_HEALER); UniqueAbility(ABILITY_RESTORATIVE_AURA); HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RESTORATIVE_AURA);
        HP_BAR(player, damage: -12);
        HP_BAR(opponent, damage: -12);
    } THEN {
        EXPECT_EQ(player->hp, 62);
        EXPECT_EQ(opponent->hp, 62);
    }
}

SINGLE_BATTLE_TEST("Restorative Aura heals after move damage at end of turn")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_AUDINO) { Ability(ABILITY_HEALER); UniqueAbility(ABILITY_RESTORATIVE_AURA); HP(100); MaxHP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STRENGTH); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STRENGTH); }
    } SCENE {
        HP_BAR(player, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_RESTORATIVE_AURA);
        HP_BAR(player, damage: -25);
    } THEN {
        EXPECT_EQ(player->hp, 100 - damage + 25);
    }
}

DOUBLE_BATTLE_TEST("Restorative Aura heals all four battlers at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_AUDINO) { Ability(ABILITY_HEALER); UniqueAbility(ABILITY_RESTORATIVE_AURA); HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_RESTORATIVE_AURA);
        HP_BAR(playerLeft, damage: -12);
        HP_BAR(playerRight, damage: -12);
        HP_BAR(opponentLeft, damage: -12);
        HP_BAR(opponentRight, damage: -12);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 62);
        EXPECT_EQ(playerRight->hp, 62);
        EXPECT_EQ(opponentLeft->hp, 62);
        EXPECT_EQ(opponentRight->hp, 62);
    }
}
