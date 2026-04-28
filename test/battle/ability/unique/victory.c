#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CELEBRATE].effect == EFFECT_CELEBRATE);
}

SINGLE_BATTLE_TEST("Victory uses Celebrate after the user knocks out a target on a shared-type team")
{
    GIVEN {
        PLAYER(SPECIES_VICTINI) { Speed(100); HP(200); MaxHP(400); Ability(ABILITY_VICTORY_STAR); UniqueAbility(ABILITY_VICTORY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_CHARMANDER) { Speed(50); }
        PLAYER(SPECIES_VULPIX) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_VICTORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Victory requires every teammate to share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_VICTINI) { Speed(100); HP(200); MaxHP(400); Ability(ABILITY_VICTORY_STAR); UniqueAbility(ABILITY_VICTORY); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_CHARMANDER) { Speed(50); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
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
