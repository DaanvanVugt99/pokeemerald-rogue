#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Blaze boosts Fire-type moves by more at half HP", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 100; }
    PARAMETRIZE { hp = 50; }
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        PLAYER(SPECIES_CHARMANDER) { Ability(ABILITY_BLAZE); MaxHP(100); HP(hp); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.25), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Blaze does not show an ability popup when dropping below half HP")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Ability(ABILITY_BLAZE); MaxHP(100); HP(60); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_BLAZE);
    }
}

SINGLE_BATTLE_TEST("Blaze shows an ability popup when boosting a Fire-type attack at half HP")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        PLAYER(SPECIES_CHARMANDER) { Ability(ABILITY_BLAZE); MaxHP(100); HP(50); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BLAZE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
    }
}
