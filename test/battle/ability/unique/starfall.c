#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WISH].effect == EFFECT_WISH);
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
}

SINGLE_BATTLE_TEST("Starfall uses Wish after the first status move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { HP(50); MaxHP(100); Ability(ABILITY_SERENE_GRACE); Moves(MOVE_GROWL, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_STARFALL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    } THEN {
        EXPECT_GT(player->hp, 50);
    }
}

SINGLE_BATTLE_TEST("Starfall only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_JIRACHI) { HP(50); MaxHP(100); Ability(ABILITY_SERENE_GRACE); Moves(MOVE_GROWL, MOVE_TAIL_WHIP, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_STARFALL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WISH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        NOT ABILITY_POPUP(player, ABILITY_STARFALL);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
