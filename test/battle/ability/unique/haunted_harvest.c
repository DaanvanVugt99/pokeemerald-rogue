#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TRICK_OR_TREAT].effect == EFFECT_THIRD_TYPE);
    ASSUME(gBattleMoves[MOVE_TRICK_OR_TREAT].argument == TYPE_GHOST);
    ASSUME(gSpeciesInfo[SPECIES_GASTLY].types[0] == TYPE_GHOST || gSpeciesInfo[SPECIES_GASTLY].types[1] == TYPE_GHOST);
}

SINGLE_BATTLE_TEST("Haunted Harvest uses Trick-or-Treat on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GOURGEIST) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_HAUNTED_HARVEST); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_HAUNTED_HARVEST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_OR_TREAT, opponent);
    } THEN {
        EXPECT_EQ(player->type3, TYPE_GHOST);
    }
}

SINGLE_BATTLE_TEST("Haunted Harvest uses Trick-or-Treat at battle start")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PUMPKABOO_SMALL) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_HAUNTED_HARVEST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_HAUNTED_HARVEST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_OR_TREAT, opponent);
    } THEN {
        EXPECT_EQ(player->type3, TYPE_GHOST);
    }
}

SINGLE_BATTLE_TEST("Haunted Harvest does not trigger if the target is already Ghost-type")
{
    GIVEN {
        PLAYER(SPECIES_GASTLY) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PUMPKABOO_SMALL) { Ability(ABILITY_FRISK); UniqueAbility(ABILITY_HAUNTED_HARVEST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_HAUNTED_HARVEST);
    }
}
