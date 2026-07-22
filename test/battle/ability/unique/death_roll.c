#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
}

SINGLE_BATTLE_TEST("Death Roll does not trigger on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE)   { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEATH_ROLL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Death Roll uses Scary Face after turn 1 when using biting moves")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE)   { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DEATH_ROLL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Death Roll does not trigger on non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_TOTODILE)   { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DEATH_ROLL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SCARY_FACE, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Death Roll targets the foe targeted by the biting move")
{
    GIVEN {
        PLAYER(SPECIES_FERALIGATR)  { Speed(50); Ability(ABILITY_STRONG_JAW); UniqueAbility(ABILITY_DEATH_ROLL); Moves(MOVE_CELEBRATE, MOVE_BITE); }
        PLAYER(SPECIES_WOBBUFFET)   { Speed(40); Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_BITE, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponentRight->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 2);
    }
}
