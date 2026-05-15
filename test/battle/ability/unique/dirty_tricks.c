#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SAND_ATTACK].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_METRONOME].effect == EFFECT_METRONOME);
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
}

SINGLE_BATTLE_TEST("Dirty Tricks uses Sand Attack on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WIGLETT) { Ability(ABILITY_GOOEY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DIRTY_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_ATTACK, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Dirty Tricks uses Metronome before fainting")
{
    GIVEN {
        PLAYER(SPECIES_WUGTRIO) { HP(1); MaxHP(100); Ability(ABILITY_GOOEY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Ability(ABILITY_MOXIE); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AERIAL_ACE, WITH_RNG(RNG_METRONOME, MOVE_SCRATCH)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DIRTY_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SAND_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        HP_BAR(player, hp: 0);
        ABILITY_POPUP(player, ABILITY_DIRTY_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Dirty Tricks preserves faint flow if Metronome calls a move that saves target")
{
    GIVEN {
        PLAYER(SPECIES_WUGTRIO) { HP(1); MaxHP(100); Ability(ABILITY_GOOEY); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_MOXIE); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AERIAL_ACE, WITH_RNG(RNG_METRONOME, MOVE_TAILWIND)); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        HP_BAR(player, hp: 0);
        ABILITY_POPUP(player, ABILITY_DIRTY_TRICKS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METRONOME, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        MESSAGE("The tailwind blew from\nbehind your team!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}
