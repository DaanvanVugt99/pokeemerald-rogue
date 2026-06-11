#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power == 40);
    ASSUME(gBattleMoves[MOVE_HEADBUTT].power != 40);
    ASSUME(gBattleMoves[MOVE_LEER].effect == EFFECT_DEFENSE_DOWN);
    ASSUME(gBattleMoves[MOVE_SCREECH].effect == EFFECT_DEFENSE_DOWN_2);
    ASSUME(gBattleMoves[MOVE_ABSORB].priority == 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].priority == 0);
}

SINGLE_BATTLE_TEST("Brutal Charge gives 2x Speed on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Speed(50); Ability(ABILITY_QUICK_FEET); UniqueAbility(ABILITY_BRUTAL_CHARGE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_ABSORB); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_ABSORB); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Brutal Charge does not boost Attack on the first turn out")
{
    s16 turnOneDamage;
    s16 turnTwoDamage;

    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Ability(ABILITY_QUICK_FEET); UniqueAbility(ABILITY_BRUTAL_CHARGE); Moves(MOVE_TACKLE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &turnOneDamage);
        HP_BAR(opponent, captureDamage: &turnTwoDamage);
    } THEN {
        EXPECT_EQ(turnOneDamage, turnTwoDamage);
    }
}

SINGLE_BATTLE_TEST("Brutal Charge uses 40 BP Headbutt after lowering a foe's Defense")
{
    s16 damage;

    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Ability(ABILITY_QUICK_FEET); UniqueAbility(ABILITY_BRUTAL_CHARGE); Moves(MOVE_LEER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEER, player);
        ABILITY_POPUP(player, ABILITY_BRUTAL_CHARGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEADBUTT, player);
        HP_BAR(opponent, captureDamage: &damage);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(gBattleMovePower, 40);
        EXPECT_GT(damage, 0);
    }
}

DOUBLE_BATTLE_TEST("Brutal Charge triggers when an ally lowers an enemy's Defense")
{
    GIVEN {
        PLAYER(SPECIES_GRANBULL) { Speed(50); Ability(ABILITY_QUICK_FEET); UniqueAbility(ABILITY_BRUTAL_CHARGE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SCREECH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(20); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_SCREECH, target: opponentLeft); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCREECH, playerRight);
        ABILITY_POPUP(playerLeft, ABILITY_BRUTAL_CHARGE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEADBUTT, playerLeft);
        HP_BAR(opponentLeft);
    } THEN {
        EXPECT_EQ(opponentLeft->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 2);
        EXPECT_EQ(opponentRight->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
