#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Shield Dust prevents Spikes damage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUSTOX) { Ability(ABILITY_SHIELD_DUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("2 sent out Dustox!");
        NOT MESSAGE("Foe Dustox is hurt by spikes!");
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Shield Dust prevents Stealth Rock damage")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUSTOX) { Ability(ABILITY_SHIELD_DUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("2 sent out Dustox!");
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Shield Dust does not prevent Sticky Web Speed drop")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_STICKY_WEB].effect == EFFECT_STICKY_WEB);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_DUSTOX) { Ability(ABILITY_SHIELD_DUST); }
    } WHEN {
        TURN { MOVE(player, MOVE_STICKY_WEB); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("Foe Dustox was caught in a Sticky Web!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
