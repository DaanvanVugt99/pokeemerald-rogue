#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Swarm boosts Bug-type moves by more at half HP", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 100; }
    PARAMETRIZE { hp = 50; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
        ASSUME(gBattleMoves[MOVE_BUG_BITE].power == 60);
        ASSUME(gSpeciesInfo[SPECIES_LEDYBA].types[0] == TYPE_BUG);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[0] == TYPE_PSYCHIC);
        ASSUME(gSpeciesInfo[SPECIES_WOBBUFFET].types[1] == TYPE_PSYCHIC);
        PLAYER(SPECIES_LEDYBA) { Ability(ABILITY_SWARM); MaxHP(100); HP(hp); Attack(45); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(121); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        // Swarm applies as an attack stat modifier, so final damage does not scale
        // exactly by 1.25 here due to stat and damage rounding.
        EXPECT_EQ(results[0].damage, 60);
        EXPECT_EQ(results[1].damage, 72);
    }
}

SINGLE_BATTLE_TEST("Swarm does not show an ability popup when dropping below half HP")
{
    GIVEN {
        PLAYER(SPECIES_LEDYBA) { Ability(ABILITY_SWARM); MaxHP(100); HP(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_SWARM);
    }
}

SINGLE_BATTLE_TEST("Swarm shows an ability popup when boosting a Bug-type attack at half HP")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
        PLAYER(SPECIES_LEDYBA) { Ability(ABILITY_SWARM); MaxHP(100); HP(50); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SWARM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BUG_BITE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Swarm shows one ability popup for a boosted multi-hit attack at half HP")
{
    PASSES_RANDOMLY(35, 100, RNG_HITS);

    GIVEN {
        ASSUME(B_MULTI_HIT_CHANCE >= GEN_5);
        ASSUME(gBattleMoves[MOVE_PIN_MISSILE].effect == EFFECT_MULTI_HIT);
        ASSUME(gBattleMoves[MOVE_PIN_MISSILE].type == TYPE_BUG);
        PLAYER(SPECIES_LEDYBA) { Ability(ABILITY_SWARM); MaxHP(100); HP(50); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PIN_MISSILE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SWARM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PIN_MISSILE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_SWARM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PIN_MISSILE, player);
        HP_BAR(opponent);
        MESSAGE("Hit 2 time(s)!");
    }
}
