#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
    ASSUME(!IS_MOVE_STATUS(MOVE_SWIFT));
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
}

SINGLE_BATTLE_TEST("Knightly raises Defense after the first contact move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ESCAVALIER) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_KNIGHTLY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_BLISSEY) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KNIGHTLY);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_KNIGHTLY);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Knightly is not consumed by non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_ESCAVALIER) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_KNIGHTLY); Moves(MOVE_SWIFT, MOVE_TACKLE); }
        OPPONENT(SPECIES_BLISSEY) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_KNIGHTLY);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_KNIGHTLY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Knightly still raises Defense if the first contact move knocks out the target")
{
    GIVEN {
        PLAYER(SPECIES_ESCAVALIER) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_KNIGHTLY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_ABRA) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_KNIGHTLY);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->hp, 0);
    }
}
