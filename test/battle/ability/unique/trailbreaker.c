#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Trailbreaker sets Plain Terrain after a Ground-type move")
{
    GIVEN {
        PLAYER(SPECIES_MUDBRAY) { Ability(ABILITY_STAMINA); UniqueAbility(ABILITY_TRAILBREAKER); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_TRAILBREAKER);
        MESSAGE("Plain terrain spread\nacross the battlefield!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Trailbreaker does not set Plain Terrain after non-Ground moves")
{
    GIVEN {
        PLAYER(SPECIES_MUDBRAY) { Ability(ABILITY_STAMINA); UniqueAbility(ABILITY_TRAILBREAKER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TRAILBREAKER);
            MESSAGE("Plain terrain spread\nacross the battlefield!");
        }
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN, 0);
    }
}
