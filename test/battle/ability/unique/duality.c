#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_PHYSICAL(MOVE_TACKLE));
    ASSUME(IS_MOVE_SPECIAL(MOVE_WATER_GUN));
}

SINGLE_BATTLE_TEST("Duality swaps Attack and Sp. Atk every other turn for physical moves")
{
    s16 firstHit;
    s16 secondHit;
    s16 thirdHit;

    GIVEN {
        PLAYER(SPECIES_ESPATHRA) { Level(100); Attack(200); SpAttack(50); Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(1000); MaxHP(1000); Defense(100); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &firstHit);
        HP_BAR(opponent, captureDamage: &secondHit);
        HP_BAR(opponent, captureDamage: &thirdHit);
    } THEN {
        EXPECT_GT(firstHit, secondHit);
        EXPECT_EQ(firstHit, thirdHit);
    }
}

SINGLE_BATTLE_TEST("Duality swaps Attack and Sp. Atk every other turn for special moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_ESPATHRA) { Level(100); Attack(200); SpAttack(50); Speed(100); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(1000); MaxHP(1000); Defense(100); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &firstHit);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_LT(firstHit, secondHit);
    }
}
