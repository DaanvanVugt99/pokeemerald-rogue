#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FOUL_MIXTURE].effect == EFFECT_FOUL_MIXTURE);
    ASSUME(gBattleMoves[MOVE_ACID_SPRAY].effect == EFFECT_SPECIAL_DEFENSE_DOWN_HIT_2);
    ASSUME(gBattleMoves[MOVE_SMOKESCREEN].effect == EFFECT_ACCURACY_DOWN);
    ASSUME(gBattleMoves[MOVE_VENOM_DRENCH].effect == EFFECT_VENOM_DRENCH);
}

SINGLE_BATTLE_TEST("Foul Mixture poisons the target, then uses a random chemical move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_FOUL_MIXTURE); }
        OPPONENT(SPECIES_WYNAUT) { HP(400); MaxHP(400); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_MIXTURE, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_ACID_SPRAY)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOUL_MIXTURE, player);
        STATUS_ICON(opponent, poison: TRUE);
        MESSAGE("Wobbuffet used Acid Spray!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID_SPRAY, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_LT(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Foul Mixture can poison the target before its random Venom Drench")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_FOUL_MIXTURE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_MIXTURE, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_VENOM_DRENCH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(opponent, poison: TRUE);
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_LT(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_LT(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_LT(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Foul Mixture does not use a chemical move if the target is already poisoned")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_FOUL_MIXTURE); }
        OPPONENT(SPECIES_WYNAUT) { Status1(STATUS1_POISON); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_MIXTURE, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_SMOKESCREEN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOKESCREEN, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Foul Mixture does not use a chemical move if the target cannot be poisoned by type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_FOUL_MIXTURE); }
        OPPONENT(SPECIES_KLINK) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_MIXTURE, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_SMOKESCREEN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SMOKESCREEN, player);
        }
    } THEN {
        EXPECT_EQ(opponent->status1, 0);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Foul Mixture does not crash if the random chemical move fails after poison succeeds")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_FOUL_MIXTURE); }
        OPPONENT(SPECIES_WYNAUT) { Ability(ABILITY_KEEN_EYE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOUL_MIXTURE, WITH_RNG(RNG_ROGUE_RANDOM_MOVE_POOL, MOVE_SMOKESCREEN)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(opponent, poison: TRUE);
        MESSAGE("Wobbuffet used Smokescreen!");
    } THEN {
        EXPECT(opponent->status1 & STATUS1_POISON);
        EXPECT_EQ(opponent->statStages[STAT_ACC], DEFAULT_STAT_STAGE);
    }
}
