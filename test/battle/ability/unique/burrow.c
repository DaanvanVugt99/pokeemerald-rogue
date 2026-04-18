#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Unique Burrow goes underground on first switch-in, ignoring moves and hazards")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUNSPARCE) { Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_BURROW); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_STEALTH_ROCK, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BURROW);
        MESSAGE("Foe Wobbuffet's attack missed!");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Unique Burrow ends after using a normal move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUNSPARCE) { Speed(100); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_BURROW); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Unique Burrow makes Dig skip its charging turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DUDUNSPARCE) { Speed(100); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_BURROW); Moves(MOVE_DIG); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DIG); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DIG, player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}
