#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Bravery raises Attack after using a Flying-type move while below full HP")
{
    GIVEN {
        PLAYER(SPECIES_SWELLOW) { Speed(50); Ability(ABILITY_GUTS); Moves(MOVE_AERIAL_ACE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_BRAVERY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Bravery does not trigger at full HP")
{
    GIVEN {
        PLAYER(SPECIES_SWELLOW) { Speed(50); Ability(ABILITY_GUTS); Moves(MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BRAVERY);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Bravery does not trigger for non-Flying moves")
{
    GIVEN {
        PLAYER(SPECIES_SWELLOW) { Speed(50); Ability(ABILITY_GUTS); Moves(MOVE_TACKLE); HP(50); MaxHP(100); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BRAVERY);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
