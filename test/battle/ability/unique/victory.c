#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CELEBRATE].effect == EFFECT_DO_NOTHING);
}

SINGLE_BATTLE_TEST("Victory heals and uses Celebrate after the user knocks out a target")
{
    GIVEN {
        PLAYER(SPECIES_VICTINI) { Speed(100); HP(200); MaxHP(400); Ability(ABILITY_VICTORY_STAR); UniqueAbility(ABILITY_VICTORY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VICTORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Victory raises Speed if the user's stats were lowered earlier that turn")
{
    GIVEN {
        PLAYER(SPECIES_VICTINI) { Speed(100); HP(200); MaxHP(400); Ability(ABILITY_VICTORY_STAR); UniqueAbility(ABILITY_VICTORY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(150); HP(1); MaxHP(400); Moves(MOVE_CHARM); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CHARM); MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VICTORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Victory respects Heal Block but still raises Speed after a stat drop")
{
    GIVEN {
        PLAYER(SPECIES_VICTINI) { Speed(100); HP(200); MaxHP(400); Ability(ABILITY_VICTORY_STAR); UniqueAbility(ABILITY_VICTORY); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(150); HP(1); MaxHP(400); Moves(MOVE_HEAL_BLOCK, MOVE_CHARM); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_HEAL_BLOCK); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CHARM); MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VICTORY);
        NONE_OF { HP_BAR(player, damage: -100); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Victory does not trigger if the user does not knock out a target")
{
    GIVEN {
        PLAYER(SPECIES_VICTINI) { Speed(100); HP(200); MaxHP(400); Ability(ABILITY_VICTORY_STAR); UniqueAbility(ABILITY_VICTORY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(400); MaxHP(400); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_VICTORY);
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Victory is assigned to Victini")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_VICTINI), ABILITY_VICTORY);
    }
}
