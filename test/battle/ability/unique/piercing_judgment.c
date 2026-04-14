#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HORN_DRILL].effect == EFFECT_OHKO);
}

SINGLE_BATTLE_TEST("Piercing Judgment makes Horn Drill ignore accuracy checks")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_HORN_DRILL].accuracy == 30);
        PLAYER(SPECIES_SEAKING) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_PIERCING_JUDGMENT); Moves(MOVE_HORN_DRILL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_HORN_DRILL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HORN_DRILL, player);
        HP_BAR(opponent, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Piercing Judgment fails when target is above half HP")
{
    GIVEN {
        PLAYER(SPECIES_SEAKING) { Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_PIERCING_JUDGMENT); Moves(MOVE_HORN_DRILL); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(101); MaxHP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_HORN_DRILL); }
    } THEN {
        EXPECT_EQ(opponent->hp, 101);
    }
}

SINGLE_BATTLE_TEST("Piercing Judgment still obeys OHKO level clause")
{
    GIVEN {
        PLAYER(SPECIES_SEAKING) { Level(40); Ability(ABILITY_SWIFT_SWIM); UniqueAbility(ABILITY_PIERCING_JUDGMENT); Moves(MOVE_HORN_DRILL); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(50); HP(50); MaxHP(200); }
    } WHEN {
        TURN { MOVE(player, MOVE_HORN_DRILL); }
    } THEN {
        EXPECT_EQ(opponent->hp, 50);
    }
}
