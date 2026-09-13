#include "global.h"
#include "test/battle.h"
#include "battle_util.h"

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor reduces physical and special damage", s16 damage)
{
    u16 item, move;
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_CLEAR_ARMOR; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_CLEAR_ARMOR; move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(1000); MaxHP(1000); Defense(120); SpDefense(120); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.8), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(0.8), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor blocks self boosts and self drops")
{
    u16 move, ability;
    PARAMETRIZE { move = MOVE_SWORDS_DANCE; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { move = MOVE_AMNESIA; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { move = MOVE_DOUBLE_TEAM; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { move = MOVE_CLOSE_COMBAT; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { move = MOVE_OVERHEAT; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { move = MOVE_OVERHEAT; ability = ABILITY_CONTRARY; }
    PARAMETRIZE { move = MOVE_CURSE; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { move = MOVE_BELLY_DRUM; ability = ABILITY_SHADOW_TAG; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_ARMOR); Ability(ability); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        for (u32 stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor blocks opposing drops and ability boosts")
{
    u16 move;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_SCARY_FACE; }
    PARAMETRIZE { move = MOVE_SAND_ATTACK; }
    PARAMETRIZE { move = MOVE_MUD_SLAP; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_ARMOR); Ability(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_INTIMIDATE); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        for (u32 stat = STAT_ATK; stat < NUM_BATTLE_STATS; stat++)
            EXPECT_EQ(player->statStages[stat], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor preserves stages against resets swaps and theft")
{
    u16 move;
    PARAMETRIZE { move = MOVE_HAZE; }
    PARAMETRIZE { move = MOVE_CLEAR_SMOG; }
    PARAMETRIZE { move = MOVE_TOPSY_TURVY; }
    PARAMETRIZE { move = MOVE_POWER_SWAP; }
    PARAMETRIZE { move = MOVE_HEART_SWAP; }
    PARAMETRIZE { move = MOVE_SPECTRAL_THIEF; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_ARMOR); Moves(MOVE_TRICK, move, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TRICK); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_CLEAR_ARMOR);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor blocks Psych Up and Transform stat copying")
{
    u16 move;
    PARAMETRIZE { move = MOVE_PSYCH_UP; }
    PARAMETRIZE { move = MOVE_TRANSFORM; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_ARMOR); Moves(MOVE_CELEBRATE, move); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor blocks Baton Pass stat transfers")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE, MOVE_BATON_PASS); }
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CLEAR_ARMOR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_CLEAR_ARMOR);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Clear Armor suppression restores stat changes and normal damage", s16 damage)
{
    u16 item, ability, setup;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_KLUTZ; setup = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_CLEAR_ARMOR; ability = ABILITY_KLUTZ; setup = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; setup = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { item = ITEM_CLEAR_ARMOR; ability = ABILITY_SHADOW_TAG; setup = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; setup = MOVE_EMBARGO; }
    PARAMETRIZE { item = ITEM_CLEAR_ARMOR; ability = ABILITY_SHADOW_TAG; setup = MOVE_EMBARGO; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Ability(ability); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE, MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(setup, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setup); }
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Sleeping Bag halves attacks only while asleep including Comatose", s16 damage)
{
    u16 item, ability, move;
    u32 status;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; status = STATUS1_SLEEP_TURN(3); move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_SHADOW_TAG; status = STATUS1_SLEEP_TURN(3); move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_SHADOW_TAG; status = STATUS1_NONE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_COMATOSE; status = STATUS1_NONE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; status = STATUS1_SLEEP_TURN(3); move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_SHADOW_TAG; status = STATUS1_SLEEP_TURN(3); move = MOVE_WATER_GUN; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Ability(ability); Status1(status); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
        EXPECT_EQ(results[1].damage, results[3].damage);
        EXPECT_MUL_EQ(results[4].damage, UQ_4_12(0.5), results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Sleeping Bag protection ends on waking", s16 damage)
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Status1(STATUS1_SLEEP_TURN(1)); Speed(200); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        MESSAGE("Wobbuffet woke up!");
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Sleeping Bag respects item suppression", s16 damage)
{
    u16 item, ability, setup;
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_KLUTZ; setup = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_KLUTZ; setup = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; setup = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_SHADOW_TAG; setup = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; setup = MOVE_EMBARGO; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; ability = ABILITY_SHADOW_TAG; setup = MOVE_EMBARGO; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Ability(ability); Status1(STATUS1_SLEEP_TURN(4)); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(setup, MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setup); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: defensive treasures leave poison and fixed damage unchanged")
{
    u16 item;
    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_CLEAR_ARMOR; }
    PARAMETRIZE { item = ITEM_SLEEPING_BAG; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); Status1(STATUS1_POISON); HP(800); MaxHP(800); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, damage: 40);
        HP_BAR(opponent, damage: 100);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Pixie Dust doubles burn chance and stacks with Serene Grace")
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_SERENE_GRACE; }
    PASSES_RANDOMLY(ability == ABILITY_SERENE_GRACE ? 40 : 20, 100, RNG_SECONDARY_EFFECT);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].secondaryEffectChance == 10);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PIXIE_DUST); Ability(ability); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        STATUS_ICON(opponent, burn: TRUE);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Pixie Dust caps secondary chances without wrapping")
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { ability = ABILITY_SERENE_GRACE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PIXIE_DUST); Ability(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(CalcSecondaryEffectChance(0, 60, EFFECT_DEFENSE_DOWN_HIT), 100);
        gSideStatuses[GetBattlerSide(0)] |= SIDE_STATUS_RAINBOW;
        EXPECT_EQ(CalcSecondaryEffectChance(0, 70, EFFECT_DEFENSE_DOWN_HIT), 100);
        EXPECT_EQ(CalcSecondaryEffectChance(0, 30, EFFECT_FLINCH_HIT), 100);
        gSideStatuses[GetBattlerSide(0)] &= ~SIDE_STATUS_RAINBOW;
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Pixie Dust respects suppression")
{
    u16 ability, setup;
    PARAMETRIZE { ability = ABILITY_KLUTZ; setup = MOVE_CELEBRATE; }
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; setup = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; setup = MOVE_EMBARGO; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PIXIE_DUST); Ability(ability); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(setup); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setup); }
    } THEN {
        EXPECT_EQ(CalcSecondaryEffectChance(0, 10, EFFECT_BURN_HIT), 10);
    }
}

SINGLE_BATTLE_TEST("Treasure additions: Pixie Dust does not bypass Covert Cloak or Sheer Force")
{
    u16 ability, item;
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; item = ITEM_COVERT_CLOAK; }
    PARAMETRIZE { ability = ABILITY_SHEER_FORCE; item = ITEM_NONE; }
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PIXIE_DUST); Ability(ability); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(item); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT STATUS_ICON(opponent, burn: TRUE);
    }
}
