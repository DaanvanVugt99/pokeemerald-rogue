#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TEETER_DANCE].effect == EFFECT_TEETER_DANCE);
}

SINGLE_BATTLE_TEST("Carnival uses Teeter Dance on switch-in during rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_LUDICOLO) { Ability(ABILITY_SWIFT_SWIM); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAIN_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RAIN_DANCE, player);
        ABILITY_POPUP(opponent, ABILITY_CARNIVAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TEETER_DANCE, opponent);
    } THEN {
        EXPECT(player->status2 & STATUS2_CONFUSION);
    }
}

SINGLE_BATTLE_TEST("Carnival does not use Teeter Dance on switch-in outside rain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_LUDICOLO) { Ability(ABILITY_SWIFT_SWIM); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_CARNIVAL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TEETER_DANCE, opponent);
        }
    } THEN {
        EXPECT(!(player->status2 & STATUS2_CONFUSION));
    }
}
