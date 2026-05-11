#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LASER_FOCUS].effect == EFFECT_LASER_FOCUS);
    ASSUME(gBattleMoves[MOVE_SONIC_BOOM].effect == EFFECT_SONICBOOM);
    ASSUME(!IS_MOVE_STATUS(MOVE_SONIC_BOOM));
}

SINGLE_BATTLE_TEST("Instinct uses Laser Focus when its target drops below half HP")
{
    GIVEN {
        PLAYER(SPECIES_LYCANROC_MIDDAY) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_INSTINCT); Moves(MOVE_SONIC_BOOM, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SONIC_BOOM, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_INSTINCT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LASER_FOCUS, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Instinct does not trigger if the target was already below half HP")
{
    GIVEN {
        PLAYER(SPECIES_LYCANROC_MIDDAY) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_INSTINCT); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { MaxHP(100); HP(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SONIC_BOOM, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LASER_FOCUS, player);
        }
    } THEN {
        EXPECT_EQ(gStatuses3[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)] & STATUS3_LASER_FOCUS, 0);
    }
}
