#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].priority == 0);
    ASSUME(gSpeciesInfo[SPECIES_PIDGEY].types[0] == TYPE_FLYING || gSpeciesInfo[SPECIES_PIDGEY].types[1] == TYPE_FLYING);
    ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] != TYPE_FLYING && gSpeciesInfo[SPECIES_WOBBUFFET].types[1] != TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Skyfire gives +1 priority against non-grounded targets")
{
    GIVEN {
        PLAYER(SPECIES_COALOSSAL) { Speed(50); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_SKYFIRE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_PIDGEY) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    }
}

SINGLE_BATTLE_TEST("Skyfire does not change priority against grounded targets")
{
    GIVEN {
        PLAYER(SPECIES_COALOSSAL) { Speed(50); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_SKYFIRE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}
