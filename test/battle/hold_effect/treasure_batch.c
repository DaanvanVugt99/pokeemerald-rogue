#include "global.h"
#include "test/battle.h"
#include "money.h"

ASSUMPTIONS
{
    ASSUME(ItemId_GetHoldEffect(ITEM_GLASS_SWORD) == HOLD_EFFECT_GLASS_SWORD);
    ASSUME(ItemId_GetHoldEffect(ITEM_WONDER_SHIELD) == HOLD_EFFECT_WONDER_SHIELD);
    ASSUME(ItemId_GetHoldEffect(ITEM_ECHO_SCEPTER) == HOLD_EFFECT_ECHO_SCEPTER);
    ASSUME(ItemId_GetHoldEffect(ITEM_GLYPH_CODEX) == HOLD_EFFECT_GLYPH_CODEX);
    ASSUME(ItemId_GetHoldEffect(ITEM_PRECISE_LENS) == HOLD_EFFECT_PRECISE_LENS);
    ASSUME(ItemId_GetHoldEffect(ITEM_FURY_MANTLE) == HOLD_EFFECT_FURY_MANTLE);
    ASSUME(ItemId_GetHoldEffect(ITEM_COMPOUND_GOGGLES) == HOLD_EFFECT_COMPOUND_GOGGLES);
    ASSUME(ItemId_GetHoldEffect(ITEM_GREEDY_GLOVES) == HOLD_EFFECT_GREEDY_GLOVES);
    ASSUME(ItemId_GetHoldEffect(ITEM_IMPACT_PLATING) == HOLD_EFFECT_IMPACT_PLATING);
    ASSUME(ItemId_GetHoldEffect(ITEM_CHIME_JEWEL) == HOLD_EFFECT_CHIME_JEWEL);
    ASSUME(ItemId_GetHoldEffect(ITEM_AMBUSH_TALON) == HOLD_EFFECT_AMBUSH_TALON);
    ASSUME(ItemId_GetHoldEffect(ITEM_PURITY_JEWEL) == HOLD_EFFECT_PURITY_JEWEL);
    ASSUME(ItemId_GetHoldEffect(ITEM_HEXING_WAND) == HOLD_EFFECT_HEXING_WAND);
    ASSUME(ItemId_GetHoldEffect(ITEM_FICKLE_HAT) == HOLD_EFFECT_FICKLE_HAT);
    ASSUME(ItemId_GetHoldEffect(ITEM_GOLDEN_EGG) == HOLD_EFFECT_GOLDEN_EGG);
    ASSUME(ItemId_GetHoldEffect(ITEM_BRIAR_BRACER) == HOLD_EFFECT_BRIAR_BRACER);
    ASSUME(ItemId_GetHoldEffect(ITEM_TRICKY_BOX) == HOLD_EFFECT_TRICKY_BOX);
    ASSUME(ItemId_GetHoldEffect(ITEM_ADAPTIVE_SPECS) == HOLD_EFFECT_ADAPTIVE_SPECS);
    ASSUME(ItemId_GetHoldEffect(ITEM_MONOTYPE_SERUM) == HOLD_EFFECT_MONOTYPE_SERUM);
    ASSUME(ItemId_GetHoldEffect(ITEM_DECOY_DOLL) == HOLD_EFFECT_DECOY_DOLL);
    ASSUME(ItemId_GetHoldEffect(ITEM_WOODEN_SWORD) == HOLD_EFFECT_WOODEN_SWORD);
    ASSUME(ItemId_GetHoldEffect(ITEM_HOURGLASS) == HOLD_EFFECT_HOURGLASS);
    ASSUME(ItemId_GetHoldEffect(ITEM_GOLDEN_IDOL) == HOLD_EFFECT_GOLDEN_IDOL);
    ASSUME(ItemId_GetHoldEffect(ITEM_DRAIN_BLADE) == HOLD_EFFECT_DRAIN_BLADE);
    ASSUME(ItemId_GetHoldEffect(ITEM_HEALING_LAMP) == HOLD_EFFECT_HEALING_LAMP);
    ASSUME(ItemId_GetHoldEffect(ITEM_CRYSTAL_WAND) == HOLD_EFFECT_CRYSTAL_WAND);
    ASSUME(ItemId_GetHoldEffect(ITEM_VICTORS_BAND) == HOLD_EFFECT_VICTORS_BAND);
    ASSUME(ItemId_GetHoldEffect(ITEM_WINGED_BOOTS) == HOLD_EFFECT_WINGED_BOOTS);
    ASSUME(ItemId_GetHoldEffect(ITEM_PINWHEEL) == HOLD_EFFECT_PINWHEEL);
    ASSUME(ItemId_GetHoldEffect(ITEM_ROYAL_JELLY) == HOLD_EFFECT_ROYAL_JELLY);
    ASSUME(gBattleMoves[MOVE_GUST].windMove);
    ASSUME(gBattleMoves[MOVE_TAILWIND].windMove);
    ASSUME(gBattleMoves[MOVE_ICY_WIND].windMove);
    ASSUME(!gBattleMoves[MOVE_CELEBRATE].windMove);
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_BITE].power > 0);
    ASSUME(gBattleMoves[MOVE_PIN_MISSILE].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_PIN_MISSILE].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_BUG_BITE].power > 0);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].power > 0);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].copycatBanned);
    ASSUME(IsMoveInherentlyMakingContact(MOVE_TACKLE));
    ASSUME(!IsMoveInherentlyMakingContact(MOVE_WATER_GUN));
    ASSUME(gSpeciesInfo[SPECIES_SCIZOR].types[0] == TYPE_BUG);
    ASSUME(gSpeciesInfo[SPECIES_SCIZOR].types[1] == TYPE_STEEL);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].types[0] == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_SNORLAX].types[1] == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_METAL_CLAW].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_FIRE_LASH].secondaryEffectChance == 100);
    ASSUME(gBattleMoves[MOVE_SONIC_BOOM].effect == EFFECT_SONICBOOM);
}

SINGLE_BATTLE_TEST("Treasure batch: Glass Sword raises damage dealt by 50 percent", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_GLASS_SWORD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Royal Jelly doubles Bug-type move damage", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_ROYAL_JELLY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_BUG_BITE); }
        OPPONENT(SPECIES_SNORLAX) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_BUG_BITE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Royal Jelly halves non-Bug move damage", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_ROYAL_JELLY; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_SNORLAX) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Glass Sword raises damage received by 50 percent", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_GLASS_SWORD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Glass Sword and Impact Plating do not modify confusion self-damage", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_GLASS_SWORD; }
    PARAMETRIZE { item = ITEM_IMPACT_PLATING; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CONFUSE_RAY, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CONFUSE_RAY); }
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_CONFUSION, TRUE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wonder Shield nullifies the first super effective damaging move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Item(ITEM_WONDER_SHIELD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's Wonder Shield nullified the damage!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wonder Shield waits for a super effective damaging move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Item(ITEM_WONDER_SHIELD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE, MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wonder Shield only activates once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Item(ITEM_WONDER_SHIELD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_BITE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, opponent);
        HP_BAR(player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wonder Shield nullifies every hit of its triggering move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Item(ITEM_WONDER_SHIELD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_PIN_MISSILE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PIN_MISSILE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Echo Scepter copies the first damaging move its holder survives")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Item(ITEM_ECHO_SCEPTER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's Echo Scepter echoed the attack!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Echo Scepter only activates once per battle")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Item(ITEM_ECHO_SCEPTER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        }
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Echo Scepter does not activate if its holder faints")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(500); Speed(50); Item(ITEM_ECHO_SCEPTER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        }
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Echo Scepter preserves its charge after U-turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Item(ITEM_ECHO_SCEPTER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_U_TURN, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_U_TURN); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 0); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponent);
        HP_BAR(player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's Echo Scepter echoed the attack!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Glyph Codex changes the holder to every move's type")
{
    GIVEN {
        PLAYER(SPECIES_KECLEON) { Item(ITEM_GLYPH_CODEX); Moves(MOVE_WATER_GUN, MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Kecleon transformed into the Water type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Kecleon transformed into the Fire type!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_FIRE);
        EXPECT_EQ(player->type2, TYPE_FIRE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Precise Lens guarantees a critical hit when moving after the target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Item(ITEM_PRECISE_LENS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Precise Lens does not guarantee a critical hit when moving first")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_PRECISE_LENS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_CRITICAL_HIT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Fury Mantle randomly raises Attack after a contact hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Item(ITEM_FURY_MANTLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_FURY_MANTLE, 0)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Fury Mantle randomly raises Speed after a contact hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Item(ITEM_FURY_MANTLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_FURY_MANTLE, 1)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Fury Mantle does not activate after a non-contact hit")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Item(ITEM_FURY_MANTLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Twin Goggles double damaging move accuracy")
{
    ASSUME(gBattleMoves[MOVE_THUNDER].accuracy == 70);
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_COMPOUND_GOGGLES); Moves(MOVE_THUNDER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER, player);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Twin Goggles prevent damaging move additional effects")
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].effect == EFFECT_PARALYZE_HIT);

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_COMPOUND_GOGGLES); Moves(MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_SECONDARY_EFFECT, TRUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        NOT STATUS_ICON(opponent, paralysis: TRUE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Greedy Gloves steal boosts before a contact move deals damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_GREEDY_GLOVES); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet stole the target's boosted stats!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Greedy Gloves do not steal boosts with non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_GREEDY_GLOVES); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SWORDS_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Impact Plating halves contact damage", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_IMPACT_PLATING; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Impact Plating raises non-contact damage by 50 percent", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_IMPACT_PLATING; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Chime Jewel raises Speed after a sound-based move")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_GROWL].soundMove);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CHIME_JEWEL); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Chime Jewel ignores non-sound moves")
{
    GIVEN {
        ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CHIME_JEWEL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Ambush Talon gives only the holder's first move priority")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(500); MaxHP(500); Item(ITEM_AMBUSH_TALON); Moves(MOVE_GROWL, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(500); MaxHP(500); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Purity Jewel boosts damage when every move has one type", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_PURITY_JEWEL; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        ASSUME(gBattleMoves[MOVE_GROWL].type == TYPE_NORMAL);
        ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].type == TYPE_NORMAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE, MOVE_GROWL, MOVE_SWORDS_DANCE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Purity Jewel counts status moves when checking types", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_PURITY_JEWEL; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
        ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].type == TYPE_ELECTRIC);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE, MOVE_THUNDER_WAVE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Hexing Wand boosts damage against statused targets", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HEXING_WAND; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Status1(STATUS1_BURN); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Hexing Wand does not boost damage against healthy targets", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HEXING_WAND; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Fickle Hat sharply raises and lowers different random stats")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_FICKLE_HAT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
    } THEN {
        u32 stat;
        u32 raised = 0;
        u32 lowered = 0;
        u32 unchanged = 0;

        for (stat = STAT_ATK; stat < NUM_STATS; stat++)
        {
            if (player->statStages[stat] == DEFAULT_STAT_STAGE + 2)
                raised++;
            else if (player->statStages[stat] == DEFAULT_STAT_STAGE - 2)
                lowered++;
            else if (player->statStages[stat] == DEFAULT_STAT_STAGE)
                unchanged++;
        }
        EXPECT_EQ(raised, 1);
        EXPECT_EQ(lowered, 1);
        EXPECT_EQ(unchanged, NUM_STATS - 3);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Golden Egg gives healing moves priority at half HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(100); MaxHP(200); Item(ITEM_GOLDEN_EGG); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Attack(1); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet can act faster, thanks to Golden Egg!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Golden Egg does not give priority above half HP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(101); MaxHP(200); Item(ITEM_GOLDEN_EGG); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Attack(1); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Golden Egg does not give Max Moves priority")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ABSORB].healBlockBanned);
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(100); MaxHP(200); Item(ITEM_GOLDEN_EGG); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Attack(1); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB, dynamax: TRUE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        MESSAGE("Wobbuffet used Max Overgrowth!");
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Briar Bracer afflicts contact attackers with Leech Seed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_BRIAR_BRACER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Foe Wobbuffet was seeded by Wobbuffet's Briar Bracer!");
    } THEN {
        EXPECT(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED);
        EXPECT_EQ(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED_BATTLER, B_POSITION_PLAYER_LEFT);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Briar Bracer ignores non-contact attacks")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_BRIAR_BRACER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Briar Bracer respects Grass immunity")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_BRIAR_BRACER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_BULBASAUR) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Briar Bracer is prevented by Protective Pads")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_BRIAR_BRACER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_PROTECTIVE_PADS); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_LEECHSEED));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Tricky Box answers an opposing status move with Copycat")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_TRICKY_BOX); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WILL_O_WISP); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WILL_O_WISP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet's Tricky Box answered the move!");
        MESSAGE("Wobbuffet used Copycat!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, player);
    } THEN {
        EXPECT(player->status1 & STATUS1_BURN);
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Tricky Box copies self-targeting status moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_TRICKY_BOX); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SWORDS_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SWORDS_DANCE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, opponent);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet used Copycat!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Tricky Box reacts when the opposing status move fails")
{
    GIVEN {
        PLAYER(SPECIES_SANDSHREW) { Item(ITEM_TRICKY_BOX); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_THUNDER_WAVE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_THUNDER_WAVE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Sandshrew used Copycat!");
    } THEN {
        EXPECT(!(player->status1 & STATUS1_PARALYSIS));
        EXPECT(opponent->status1 & STATUS1_PARALYSIS);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Tricky Box ignores damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_TRICKY_BOX); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        NOT ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    }
}

DOUBLE_BATTLE_TEST("Treasure batch: Tricky Box targets the status move's user in doubles")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_TRICKY_BOX); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(110); Moves(MOVE_WILL_O_WISP); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_WILL_O_WISP, target: playerLeft);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, opponentRight);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
        MESSAGE("Wobbuffet used Copycat!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WILL_O_WISP, playerLeft);
    } THEN {
        EXPECT(opponentRight->status1 & STATUS1_BURN);
        EXPECT(!(opponentLeft->status1 & STATUS1_BURN));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Adaptive Specs makes special moves physical when that deals more damage", s16 damage)
{
    u32 move;
    u16 item;

    PARAMETRIZE { move = MOVE_TACKLE; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_WATER_GUN; item = ITEM_ADAPTIVE_SPECS; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power == gBattleMoves[MOVE_WATER_GUN].power);
        PLAYER(SPECIES_WOBBUFFET) { Attack(240); SpAttack(20); Item(item); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Adaptive Specs makes physical moves special when that deals more damage", s16 damage)
{
    u32 move;
    u16 item;

    PARAMETRIZE { move = MOVE_WATER_GUN; item = ITEM_NONE; }
    PARAMETRIZE { move = MOVE_TACKLE; item = ITEM_ADAPTIVE_SPECS; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].power == gBattleMoves[MOVE_WATER_GUN].power);
        PLAYER(SPECIES_WOBBUFFET) { Attack(20); SpAttack(240); Item(item); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Adaptive Specs does not make non-contact moves contact")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(240); SpAttack(20); Item(ITEM_ADAPTIVE_SPECS); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_BRIAR_BRACER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_LEECHSEED));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Adaptive Specs preserves a move's inherent contact")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(20); SpAttack(240); Item(ITEM_ADAPTIVE_SPECS); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(ITEM_BRIAR_BRACER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gStatuses3[B_POSITION_PLAYER_LEFT] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Monotype Serum removes the holder's secondary defensive type", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_MONOTYPE_SERUM; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_SCIZOR) { SpDefense(120); HP(1000); MaxHP(1000); Item(item); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Monotype Serum preserves STAB for the removed secondary type", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_MONOTYPE_SERUM; }

    GIVEN {
        PLAYER(SPECIES_SCIZOR) { Attack(120); Item(item); Moves(MOVE_METAL_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_METAL_CLAW, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Monotype Serum removes secondary-type status immunities")
{
    GIVEN {
        PLAYER(SPECIES_SCIZOR) { Item(ITEM_MONOTYPE_SERUM); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC); }
    } THEN {
        EXPECT(player->status1 & STATUS1_TOXIC_POISON);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Decoy Doll pays one quarter max HP and creates a Substitute on entry")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(200); MaxHP(200); Item(ITEM_DECOY_DOLL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SUBSTITUTE_APPEAR, player);
        MESSAGE("Wobbuffet's Decoy Doll created a substitute!");
    } THEN {
        EXPECT_EQ(player->hp, 150);
        EXPECT(player->status2 & STATUS2_SUBSTITUTE);
        EXPECT_EQ(gDisableStructs[B_POSITION_PLAYER_LEFT].substituteHP, 50);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Decoy Doll does not activate when the holder cannot pay its HP cost")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(200); Item(ITEM_DECOY_DOLL); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
            MESSAGE("Wobbuffet's Decoy Doll created a substitute!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 50);
        EXPECT(!(player->status2 & STATUS2_SUBSTITUTE));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wooden Sword makes a single-hit attack strike three times")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(ITEM_WOODEN_SWORD); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        MESSAGE("Hit 3 time(s)!");
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wooden Sword deals 40 percent power per hit", s16 firstDamage, s16 secondDamage, s16 thirdDamage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_WOODEN_SWORD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].firstDamage);
        if (item == ITEM_WOODEN_SWORD) {
            HP_BAR(opponent, captureDamage: &results[i].secondDamage);
            HP_BAR(opponent, captureDamage: &results[i].thirdDamage);
        }
    } FINALLY {
        EXPECT_MUL_EQ(results[0].firstDamage, UQ_4_12(0.4), results[1].firstDamage);
        EXPECT_EQ(results[1].firstDamage, results[1].secondDamage);
        EXPECT_EQ(results[1].firstDamage, results[1].thirdDamage);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wooden Sword only allows the first hit's additional effect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(ITEM_WOODEN_SWORD); Moves(MOVE_FIRE_LASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FIRE_LASH, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Wooden Sword does not repeat fixed-damage moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WOODEN_SWORD); Moves(MOVE_SONIC_BOOM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SONIC_BOOM); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 80);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Hourglass halves Speed and ramps it by two stages each turn")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Item(ITEM_HOURGLASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 6);
        EXPECT_EQ(GetBattlerTotalSpeedStat(battler), 200);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Winged Boots doubles Speed at half HP")
{
    u32 battler;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(50); MaxHP(100); Item(ITEM_WINGED_BOOTS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerTotalSpeedStat(battler), 200);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Winged Boots is inactive above half HP")
{
    u32 battler;
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(51); MaxHP(100); Item(ITEM_WINGED_BOOTS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerTotalSpeedStat(battler), 100);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Crystal Wand raises Defense after super-effective damage once per turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CRYSTAL_WAND); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_CHARMANDER) { HP(10000); MaxHP(10000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Crystal Wand does not activate on neutral damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CRYSTAL_WAND); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(10000); MaxHP(10000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Crystal Wand does not activate when Wonder Shield nullifies damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CRYSTAL_WAND); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_CHARMANDER) { HP(10000); MaxHP(10000); Item(ITEM_WONDER_SHIELD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Crystal Wand activates once after a multi-hit move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_CRYSTAL_WAND); Moves(MOVE_TRIPLE_DIVE); }
        OPPONENT(SPECIES_CHARMANDER) { HP(10000); MaxHP(10000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TRIPLE_DIVE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Golden Idol scales damage with money and caps at 50 percent", s16 damage)
{
    u16 item;
    u32 money;

    PARAMETRIZE { item = ITEM_NONE; money = 0; }
    PARAMETRIZE { item = ITEM_GOLDEN_IDOL; money = 999; }
    PARAMETRIZE { item = ITEM_GOLDEN_IDOL; money = 20000; }
    PARAMETRIZE { item = ITEM_GOLDEN_IDOL; money = 50000; }
    PARAMETRIZE { item = ITEM_GOLDEN_IDOL; money = MAX_MONEY; }

    GIVEN {
        SetMoney(&gSaveBlock1Ptr->money, money);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[3].damage);
        EXPECT_EQ(results[3].damage, results[4].damage);
        SetMoney(&gSaveBlock1Ptr->money, 0);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Golden Idol shares the Amulet Coin prize-money effect")
{
    u16 item;
    u8 expectedMultiplier;

    PARAMETRIZE { item = ITEM_NONE; expectedMultiplier = 1; }
    PARAMETRIZE { item = ITEM_AMULET_COIN; expectedMultiplier = 2; }
    PARAMETRIZE { item = ITEM_GOLDEN_IDOL; expectedMultiplier = 2; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleStruct->moneyMultiplier == expectedMultiplier);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Drain Blade heals based on damage and target HP", s16 damage, s16 healing)
{
    u16 targetHp;

    PARAMETRIZE { targetHp = 1000; }
    PARAMETRIZE { targetHp = 500; }
    PARAMETRIZE { targetHp = 400; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); HP(500); MaxHP(1000); Item(ITEM_DRAIN_BLADE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(targetHp); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        HP_BAR(player, captureDamage: &results[i].healing);
    } FINALLY {
        EXPECT_EQ(results[0].healing, -max(1, results[0].damage / 8));
        EXPECT_EQ(results[1].healing, -max(1, results[1].damage / 8));
        EXPECT_EQ(results[2].healing, -max(1, results[2].damage / 4));
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Drain Blade combines multi-hit damage into one heal")
{
    s16 firstDamage;
    s16 secondDamage;
    s16 healing;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(300); HP(500); MaxHP(1000); Item(ITEM_DRAIN_BLADE); Moves(MOVE_DOUBLE_HIT); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(1); HP(10000); MaxHP(10000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_HIT, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_HIT, player);
        HP_BAR(opponent, captureDamage: &firstDamage);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_HIT, player);
        HP_BAR(opponent, captureDamage: &secondDamage);
        HP_BAR(player, captureDamage: &healing);
    } THEN {
        EXPECT_EQ(healing, -max(1, (firstDamage + secondDamage) / 8));
    }
}

DOUBLE_BATTLE_TEST("Treasure batch: Golden Idol does not stack with Amulet Coin")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_GOLDEN_IDOL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_AMULET_COIN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gBattleStruct->moneyMultiplier == 2);
        EXPECT(gBattleStruct->moneyMultiplierItem);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Healing Lamp restores one eighth max HP at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(96); Item(ITEM_HEALING_LAMP); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 13);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Healing Lamp makes the holder's moves consume one additional PP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_HEALING_LAMP); MovesWithPP({MOVE_CELEBRATE, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->pp[0], 8);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Healing Lamp's PP cost stacks with Pressure")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_HEALING_LAMP); MovesWithPP({MOVE_TACKLE, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_PRESSURE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->pp[0], 7);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Victor's Band restores 25 percent max HP after a knockout")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); HP(500); MaxHP(1000); Item(ITEM_VICTORS_BAND); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } THEN {
        EXPECT_EQ(player->hp, 750);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Klutz suppresses both Healing Lamp effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); HP(1); MaxHP(96); Item(ITEM_HEALING_LAMP); MovesWithPP({MOVE_CELEBRATE, 10}); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
        EXPECT_EQ(player->pp[0], 9);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Pinwheel switches the holder out after a successful wind attack")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Item(ITEM_PINWHEEL); Moves(MOVE_GUST); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUST); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Pinwheel switches the holder out after a successful wind status move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_PINWHEEL); Moves(MOVE_TAILWIND); }
        PLAYER(SPECIES_MAGIKARP);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAILWIND); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MAGIKARP);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Pinwheel does not switch the holder out when a wind move fails")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Item(ITEM_PINWHEEL); Moves(MOVE_GUST); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_GUST); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_PIDGEOT);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Pinwheel does not activate without a reserve Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Item(ITEM_PINWHEEL); Moves(MOVE_GUST); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUST); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_PIDGEOT);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Pinwheel ignores moves without the wind flag")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Item(ITEM_PINWHEEL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_PIDGEOT);
    }
}

SINGLE_BATTLE_TEST("Treasure batch: Klutz suppresses Pinwheel")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Ability(ABILITY_KLUTZ); Item(ITEM_PINWHEEL); Moves(MOVE_GUST); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GUST); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GUST, player);
        NONE_OF {
            ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_PIDGEOT);
    }
}

DOUBLE_BATTLE_TEST("Treasure batch: Pinwheel waits for a spread wind move to hit every target")
{
    GIVEN {
        PLAYER(SPECIES_PIDGEOT) { Speed(100); Item(ITEM_PINWHEEL); Moves(MOVE_ICY_WIND); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        PLAYER(SPECIES_MAGIKARP) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ICY_WIND); SEND_OUT(playerLeft, 2); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ICY_WIND, playerLeft);
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, playerLeft);
    } THEN {
        EXPECT_EQ(playerLeft->species, SPECIES_MAGIKARP);
        EXPECT_LT(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
    }
}
