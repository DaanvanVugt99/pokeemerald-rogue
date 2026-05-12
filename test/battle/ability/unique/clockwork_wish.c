#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(gBattleMoves[MOVE_HEAL_BELL].effect == EFFECT_HEAL_BELL);
}

SINGLE_BATTLE_TEST("Clockwork Wish uses Heal Bell after the first status move each battle")
{
    GIVEN {
        PLAYER(SPECIES_MAGEARNA) { Status1(STATUS1_BURN); Ability(ABILITY_SOUL_HEART); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_CLOCKWORK_WISH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        NOT ABILITY_POPUP(player, ABILITY_CLOCKWORK_WISH);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Clockwork Wish does not trigger after non-status moves")
{
    GIVEN {
        PLAYER(SPECIES_MAGEARNA) { Status1(STATUS1_BURN); Ability(ABILITY_SOUL_HEART); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CLOCKWORK_WISH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, player);
        }
    } THEN {
        EXPECT_NE(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Clockwork Wish does not consume its trigger on a failed status move")
{
    GIVEN {
        PLAYER(SPECIES_MAGEARNA) { Speed(50); Status1(STATUS1_BURN); Ability(ABILITY_SOUL_HEART); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_GROWL); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NOT ABILITY_POPUP(player, ABILITY_CLOCKWORK_WISH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ABILITY_POPUP(player, ABILITY_CLOCKWORK_WISH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BELL, player);
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
