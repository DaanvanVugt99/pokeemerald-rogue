#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Hot Pursuit hits a switching foe with Flame Charge and burns it")
{
    GIVEN {
        PLAYER(SPECIES_RAPIDASH) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_HOT_PURSUIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HOT_PURSUIT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        STATUS_ICON(opponent, burn: TRUE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Hot Pursuit does not trigger when the opponent does not switch")
{
    GIVEN {
        PLAYER(SPECIES_RAPIDASH) { Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_HOT_PURSUIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HOT_PURSUIT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        }
    }
}
