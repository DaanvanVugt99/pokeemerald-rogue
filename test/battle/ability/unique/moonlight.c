#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
}

DOUBLE_BATTLE_TEST("Moonlight restores HP when an ally is healed")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Speed(50); HP(100); MaxHP(400); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_MOONLIGHT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(100); MaxHP(400); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerRight, MOVE_RECOVER); MOVE(playerLeft, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerRight);
        HP_BAR(playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_MOONLIGHT);
        MESSAGE("Clefable's Moonlight restored its HP a little!");
        HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 150);
    }
}

DOUBLE_BATTLE_TEST("Moonlight lets multiple users restore HP from the same heal")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Speed(50); HP(100); MaxHP(400); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_MOONLIGHT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CLEFAIRY) { Speed(100); HP(100); MaxHP(400); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_MOONLIGHT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(150); HP(100); MaxHP(400); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_RECOVER); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponentLeft);
        HP_BAR(opponentLeft);
        ABILITY_POPUP(playerRight, ABILITY_MOONLIGHT);
        MESSAGE("Clefairy's Moonlight restored its HP a little!");
        HP_BAR(playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_MOONLIGHT);
        MESSAGE("Clefable's Moonlight restored its HP a little!");
        HP_BAR(playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->hp, 150);
        EXPECT_EQ(playerRight->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Moonlight restores HP when a foe is healed")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { Speed(50); HP(100); MaxHP(400); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_MOONLIGHT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(100); MaxHP(400); Moves(MOVE_RECOVER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponent);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_MOONLIGHT);
        MESSAGE("Clefable's Moonlight restored its HP a little!");
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 150);
    }
}

SINGLE_BATTLE_TEST("Moonlight restores HP after end-turn healing")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { HP(100); MaxHP(400); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_MOONLIGHT); Moves(MOVE_GRASSY_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Clefable is healed by the grassy terrain!");
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_MOONLIGHT);
        MESSAGE("Clefable's Moonlight restored its HP a little!");
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 175);
    }
}

SINGLE_BATTLE_TEST("Moonlight does not recursively trigger from its own heal")
{
    GIVEN {
        PLAYER(SPECIES_CLEFABLE) { HP(1); MaxHP(400); Ability(ABILITY_MAGIC_GUARD); UniqueAbility(ABILITY_MOONLIGHT); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_MOONLIGHT);
        MESSAGE("Clefable's Moonlight restored its HP a little!");
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MOONLIGHT);
            MESSAGE("Clefable's Moonlight restored its HP a little!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 251);
    }
}
