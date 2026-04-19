#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Iron Shards sets Stealth Rock when the user takes 25 percent or less damage from a move")
{
    GIVEN {
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IRON_SHARDS); MaxHP(200); HP(200); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK);
    }
}

SINGLE_BATTLE_TEST("Iron Shards does not set Stealth Rock when the user takes more than 25 percent damage from a move")
{
    GIVEN {
        PLAYER(SPECIES_AGGRON) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_IRON_SHARDS); MaxHP(100); HP(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_STEALTH_ROCK, player);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_OPPONENT] & SIDE_STATUS_STEALTH_ROCK));
    }
}
