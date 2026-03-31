#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Tantrum gives +1 priority to the next move after a miss")
{
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Speed(50); MaxHP(500); Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_TANTRUM); Moves(MOVE_AERIAL_ACE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); MaxHP(500); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_PROTECT); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
    }
}

SINGLE_BATTLE_TEST("Tantrum does not boost priority if the previous move did not miss")
{
    GIVEN {
        PLAYER(SPECIES_PRIMEAPE) { Speed(50); MaxHP(500); Ability(ABILITY_VITAL_SPIRIT); UniqueAbility(ABILITY_TANTRUM); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); MaxHP(500); Moves(MOVE_AERIAL_ACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_AERIAL_ACE); }
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_AERIAL_ACE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
    }
}

SINGLE_BATTLE_TEST("Tantrum activates persistent state after a miss")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_TANTRUM); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_PROTECT); }
    } THEN {
        EXPECT(gDisableStructs[0].uniquePersistentStateActive);
    }
}
