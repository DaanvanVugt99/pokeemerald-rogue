#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Chatterbox copies the last status move on each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ODDISH) { Speed(40); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHATOT) { Speed(90); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_CHATTERBOX); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Ability(ABILITY_TELEPATHY); Moves(MOVE_LIGHT_SCREEN, MOVE_CALM_MIND, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_LIGHT_SCREEN); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CALM_MIND); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, opponent);
        ABILITY_POPUP(player, ABILITY_CHATTERBOX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COPYCAT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LIGHT_SCREEN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, opponent);
        ABILITY_POPUP(player, ABILITY_CHATTERBOX);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COPYCAT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, player);
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_LIGHTSCREEN);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Chatterbox skips damaging moves and status moves that Copycat cannot call")
{
    u16 move;
    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_PROTECT; }

    GIVEN {
        PLAYER(SPECIES_ODDISH) { HP(400); MaxHP(400); Speed(30); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CHATOT) { Speed(90); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_CHATTERBOX); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_TELEPATHY); Moves(move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, move); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Go! Chatot!");
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHATTERBOX);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_COPYCAT, player);
        }
    }
}

SINGLE_BATTLE_TEST("Chatterbox does not activate before any move has been used")
{
    GIVEN {
        PLAYER(SPECIES_CHATOT) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_CHATTERBOX); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHATTERBOX);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_COPYCAT, player);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}
