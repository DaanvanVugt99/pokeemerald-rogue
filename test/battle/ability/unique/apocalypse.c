#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Apocalypse uses Nasty Plot after knocking out a target on a shared-type team")
{
    GIVEN {
        PLAYER(SPECIES_HYDREIGON) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_APOCALYPSE); Moves(MOVE_DRAGON_BREATH); }
        PLAYER(SPECIES_DEINO);
        PLAYER(SPECIES_SCRAFTY);
        OPPONENT(SPECIES_SHEDINJA) { Ability(ABILITY_WONDER_GUARD); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_APOCALYPSE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_NASTY_PLOT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Apocalypse does not trigger if the party does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_HYDREIGON) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_APOCALYPSE); Moves(MOVE_DRAGON_BREATH); }
        PLAYER(SPECIES_DEINO);
        PLAYER(SPECIES_PIKACHU);
        OPPONENT(SPECIES_SHEDINJA) { Ability(ABILITY_WONDER_GUARD); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_APOCALYPSE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Apocalypse does not trigger if the target is not knocked out")
{
    GIVEN {
        PLAYER(SPECIES_HYDREIGON) { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_APOCALYPSE); Moves(MOVE_DRAGON_BREATH); }
        PLAYER(SPECIES_DEINO);
        PLAYER(SPECIES_SCRAFTY);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_APOCALYPSE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
