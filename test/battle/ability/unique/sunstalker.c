#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].priority == 0);
}

SINGLE_BATTLE_TEST("Sunstalker gives +1 priority only to the first slicing move after switch-in outside sun")
{
    GIVEN {
        PLAYER(SPECIES_SCEPTILE) { Speed(50); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_SUNSTALKER); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}

SINGLE_BATTLE_TEST("Sunstalker is not consumed by non-slicing moves")
{
    GIVEN {
        PLAYER(SPECIES_SCEPTILE) { Speed(50); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_SUNSTALKER); Moves(MOVE_TACKLE, MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Sunstalker gives slicing moves +1 priority every turn in sun")
{
    GIVEN {
        PLAYER(SPECIES_SCEPTILE) { Speed(50); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_SUNSTALKER); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Ability(ABILITY_DROUGHT); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}
