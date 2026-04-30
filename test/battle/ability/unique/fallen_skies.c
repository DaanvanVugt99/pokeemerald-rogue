#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WING_ATTACK].priority == 0);
    ASSUME(gBattleMoves[MOVE_WING_ATTACK].type == TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Fallen Skies gives +1 priority to Flying-type moves below half HP")
{
    GIVEN {
        PLAYER(SPECIES_ARCHEOPS) { HP(50); MaxHP(100); Speed(50); Ability(ABILITY_DEFEATIST); UniqueAbility(ABILITY_FALLEN_SKIES); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Fallen Skies does not give priority above half HP")
{
    GIVEN {
        PLAYER(SPECIES_ARCHEOPS) { HP(51); MaxHP(100); Speed(50); Ability(ABILITY_DEFEATIST); UniqueAbility(ABILITY_FALLEN_SKIES); Moves(MOVE_WING_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WING_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WING_ATTACK, player);
    }
}
