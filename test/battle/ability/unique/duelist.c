#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].priority == 0);
    ASSUME(gBattleMoves[MOVE_LEAF_BLADE].slicingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].slicingMove);
}

SINGLE_BATTLE_TEST("Duelist gives +1 priority to slicing moves at or below half HP")
{
    GIVEN {
        PLAYER(SPECIES_FARFETCHD) { HP(50); MaxHP(100); Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_DUELIST); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Duelist does not give priority above half HP")
{
    GIVEN {
        PLAYER(SPECIES_FARFETCHD) { HP(51); MaxHP(100); Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_DUELIST); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
    }
}

SINGLE_BATTLE_TEST("Duelist heals 1/8 max HP after using a slicing move at or below half HP")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_FARFETCHD) { HP(40); MaxHP(160); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_DUELIST); Moves(MOVE_LEAF_BLADE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_LEAF_BLADE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAF_BLADE, player);
        ABILITY_POPUP(player, ABILITY_DUELIST);
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -20);
    }
}
