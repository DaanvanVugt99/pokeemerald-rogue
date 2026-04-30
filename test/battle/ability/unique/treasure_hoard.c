#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_BREATH].type == TYPE_FIRE);
    ASSUME(gItems[ITEM_NORMAL_GEM].holdEffect == HOLD_EFFECT_GEMS);
    ASSUME(gItems[ITEM_AMULET_COIN].holdEffect == HOLD_EFFECT_DOUBLE_PRIZE);
}

SINGLE_BATTLE_TEST("Treasure Hoard doubles Dragon Breath damage while holding a Gem or Amulet Coin", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_NORMAL_GEM; }
    PARAMETRIZE { item = ITEM_AMULET_COIN; }

    GIVEN {
        PLAYER(SPECIES_DRUDDIGON) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_TREASURE_HOARD); Item(item); Moves(MOVE_DRAGON_BREATH); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_BREATH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure Hoard does not boost other moves", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_NORMAL_GEM; }
    PARAMETRIZE { item = ITEM_AMULET_COIN; }

    GIVEN {
        PLAYER(SPECIES_DRUDDIGON) { Ability(ABILITY_ROUGH_SKIN); UniqueAbility(ABILITY_TREASURE_HOARD); Item(item); Moves(MOVE_FLAMETHROWER); SpAttack(100); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); SpDefense(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_FLAMETHROWER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}
