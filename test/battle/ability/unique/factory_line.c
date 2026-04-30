#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHIFT_GEAR].effect == EFFECT_SHIFT_GEAR);
}

SINGLE_BATTLE_TEST("Factory Line uses Shift Gear on switch-in if the party has 2 other Steel-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KLINKLANG) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FACTORY_LINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGNEMITE) { Speed(40); Ability(ABILITY_MAGNET_PULL); }
        OPPONENT(SPECIES_BRONZOR) { Speed(30); Ability(ABILITY_LEVITATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_FACTORY_LINE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHIFT_GEAR, opponent);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Factory Line does not use Shift Gear on switch-in without 2 other Steel-type Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KLINKLANG) { Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_FACTORY_LINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MAGIKARP) { Speed(40); Ability(ABILITY_SWIFT_SWIM); }
        OPPONENT(SPECIES_PIKACHU) { Speed(30); Ability(ABILITY_STATIC); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_FACTORY_LINE);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}
