#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AGILITY].type == TYPE_PSYCHIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_PSYCHIC);
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > 0);
}

SINGLE_BATTLE_TEST("One Mind makes Psychic-type moves act first over higher-priority moves")
{
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Speed(1); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ONE_MIND); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_RATTATA) { Speed(100); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        MESSAGE("Mewtwo used Agility!");
        MESSAGE("Foe Rattata used Quick Attack!");
    }
}

SINGLE_BATTLE_TEST("One Mind Psychic-type moves still act first in Trick Room")
{
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ONE_MIND); Moves(MOVE_TRICK_ROOM, MOVE_AGILITY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRICK_ROOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Mewtwo used Trick Room!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Mewtwo used Agility!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("One Mind does not affect non-Psychic moves")
{
    GIVEN {
        PLAYER(SPECIES_MEWTWO) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ONE_MIND); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}
