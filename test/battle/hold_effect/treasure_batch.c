#include "global.h"
#include "test/battle.h"

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
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_BITE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_BITE].power > 0);
    ASSUME(gBattleMoves[MOVE_PIN_MISSILE].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_PIN_MISSILE].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].power > 0);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].copycatBanned);
    ASSUME(IsMoveInherentlyMakingContact(MOVE_TACKLE));
    ASSUME(!IsMoveInherentlyMakingContact(MOVE_WATER_GUN));
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
