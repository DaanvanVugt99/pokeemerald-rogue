#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(ItemId_GetHoldEffect(ITEM_CURSED_LENS) == HOLD_EFFECT_CURSED_LENS);
    ASSUME(ItemId_GetHoldEffect(ITEM_VOW_OF_SILENCE) == HOLD_EFFECT_VOW_OF_SILENCE);
    ASSUME(ItemId_GetHoldEffect(ITEM_BLOOD_OATH) == HOLD_EFFECT_BLOOD_OATH);
    ASSUME(ItemId_GetHoldEffect(ITEM_HOLLOW_SUN) == HOLD_EFFECT_HOLLOW_SUN);
    ASSUME(ItemId_GetHoldEffect(ITEM_MALICE_ORB) == HOLD_EFFECT_MALICE_ORB);
    ASSUME(ItemId_GetHoldEffect(ITEM_GRAVEGLASS) == HOLD_EFFECT_GRAVEGLASS);
    ASSUME(ItemId_GetHoldEffect(ITEM_ASHEN_CROWN) == HOLD_EFFECT_ASHEN_CROWN);
    ASSUME(ItemId_GetHoldEffect(ITEM_WITCHS_THREAD) == HOLD_EFFECT_WITCHS_THREAD);
    ASSUME(ItemId_GetHoldEffect(ITEM_PETRIFIED_HEART) == HOLD_EFFECT_PETRIFIED_HEART);
    ASSUME(ItemId_GetHoldEffect(ITEM_FALSE_IDOL) == HOLD_EFFECT_FALSE_IDOL);
    ASSUME(ItemId_GetHoldEffect(ITEM_RUSTED_ANCHOR) == HOLD_EFFECT_RUSTED_ANCHOR);
    ASSUME(ItemId_GetHoldEffect(ITEM_GAMBLERS_CLAW) == HOLD_EFFECT_GAMBLERS_CLAW);
}

SINGLE_BATTLE_TEST("Cursed Lens applies once from final type effectiveness", s16 damage)
{
    u16 item;
    u16 targetSpecies;

    PARAMETRIZE { item = ITEM_NONE;        targetSpecies = SPECIES_WOBBUFFET; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; targetSpecies = SPECIES_WOBBUFFET; }
    PARAMETRIZE { item = ITEM_NONE;        targetSpecies = SPECIES_SCEPTILE_MEGA; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; targetSpecies = SPECIES_SCEPTILE_MEGA; }
    PARAMETRIZE { item = ITEM_NONE;        targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_NONE;        targetSpecies = SPECIES_PARAS; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; targetSpecies = SPECIES_PARAS; }
    PARAMETRIZE { item = ITEM_NONE;        targetSpecies = SPECIES_SQUIRTLE; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; targetSpecies = SPECIES_SQUIRTLE; }
    PARAMETRIZE { item = ITEM_NONE;        targetSpecies = SPECIES_OMANYTE; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; targetSpecies = SPECIES_OMANYTE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Item(item); Moves(MOVE_EMBER); }
        OPPONENT(targetSpecies) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_MUL_EQ(results[4].damage, UQ_4_12(1.3), results[5].damage);
        EXPECT_MUL_EQ(results[6].damage, UQ_4_12(1.3), results[7].damage);
        EXPECT_MUL_EQ(results[8].damage, UQ_4_12(0.8), results[9].damage);
        EXPECT_MUL_EQ(results[10].damage, UQ_4_12(0.8), results[11].damage);
    }
}

SINGLE_BATTLE_TEST("Cursed Lens respects runtime type changes", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SOAK].effect == EFFECT_SOAK);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Item(item); Moves(MOVE_SOAK, MOVE_EMBER); }
        OPPONENT(SPECIES_CHARMANDER) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOAK); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.8), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Cursed Lens leaves immunities and fixed damage unchanged", s16 damage)
{
    u16 item;
    u16 move;

    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_DRAGON_RAGE; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; move = MOVE_DRAGON_RAGE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Moves(move); }
        OPPONENT(SPECIES_GASTLY) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        if (move == MOVE_TACKLE)
            NOT HP_BAR(opponent);
        else
            HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[2].damage, 40);
        EXPECT_EQ(results[3].damage, 40);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Cursed Lens", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_CURSED_LENS; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); SpAttack(120); Item(item); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_TANGELA) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Hollow Sun reverses non-neutral incoming matchups and preserves neutral and immunity", s16 damage)
{
    u16 item;
    u16 move;
    u16 targetSpecies;

    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_EMBER;    targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; move = MOVE_EMBER;    targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_WATER_GUN; targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; move = MOVE_WATER_GUN; targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_TACKLE;   targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; move = MOVE_TACKLE;   targetSpecies = SPECIES_TANGELA; }
    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_TACKLE;   targetSpecies = SPECIES_GASTLY; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; move = MOVE_TACKLE;   targetSpecies = SPECIES_GASTLY; }
    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_EMBER;    targetSpecies = SPECIES_FORRETRESS; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; move = MOVE_EMBER;    targetSpecies = SPECIES_FORRETRESS; }
    PARAMETRIZE { item = ITEM_NONE;        move = MOVE_EMBER;    targetSpecies = SPECIES_RELICANTH; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; move = MOVE_EMBER;    targetSpecies = SPECIES_RELICANTH; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Moves(move); }
        OPPONENT(targetSpecies) { Defense(120); SpDefense(120); Item(item); HP(10000); MaxHP(10000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        if (targetSpecies == SPECIES_GASTLY)
            NOT HP_BAR(opponent);
        else
            HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.25), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(4.0), results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
        EXPECT_MUL_EQ(results[8].damage, UQ_4_12(0.0625), results[9].damage);
        EXPECT_MUL_EQ(results[11].damage, UQ_4_12(0.0625), results[10].damage);
    }
}

SINGLE_BATTLE_TEST("Hollow Sun respects runtime typing changes", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_SOAK].effect == EFFECT_SOAK);
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_SOAK, MOVE_EMBER); }
        OPPONENT(SPECIES_CHARMANDER) { SpDefense(120); Item(item); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_SOAK); }
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(4.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Hollow Sun", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_HOLLOW_SUN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_TANGELA) { Ability(ABILITY_KLUTZ); SpDefense(120); Item(item); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Vow of Silence boosts physical and special direct damage", s16 damage)
{
    u16 item;
    u16 move;

    PARAMETRIZE { item = ITEM_NONE;           move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_VOW_OF_SILENCE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE;           move = MOVE_PSYBEAM; }
    PARAMETRIZE { item = ITEM_VOW_OF_SILENCE; move = MOVE_PSYBEAM; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Item(item); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.25), results[1].damage);
        EXPECT_MUL_EQ(results[2].damage, UQ_4_12(1.25), results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Vow of Silence blocks status moves and preserves Struggle fallback")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_VOW_OF_SILENCE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, allowed: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, player);
    }
}

SINGLE_BATTLE_TEST("Vow of Silence blocks Me First")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_VOW_OF_SILENCE); Moves(MOVE_ME_FIRST, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ME_FIRST, allowed: FALSE); MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    }
}

SINGLE_BATTLE_TEST("Vow of Silence blocks a called status move at execution")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_RUINATION].effect == EFFECT_SUPER_FANG);
        ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
        PLAYER(SPECIES_CHI_YU) { Speed(1); Item(ITEM_VOW_OF_SILENCE); Moves(MOVE_RUINATION); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ECLIPSE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); MOVE(player, MOVE_RUINATION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RUINATION, player);
        ABILITY_POPUP(player, ABILITY_CINDER_EDICT);
        MESSAGE("Vow of Silence's effects prevent status moves from being used!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, player);
    }
}

SINGLE_BATTLE_TEST("Vow of Silence leaves fixed damage unchanged", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_VOW_OF_SILENCE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, 40);
        EXPECT_EQ(results[1].damage, 40);
    }
}

SINGLE_BATTLE_TEST("Vow of Silence does not boost confusion self-damage", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_VOW_OF_SILENCE; }

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
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Vow of Silence", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_VOW_OF_SILENCE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Blood Oath saves its holder below full HP and is consumed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(20); MaxHP(100); Item(ITEM_BLOOD_OATH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player, hp: 1);
    } THEN {
        EXPECT_EQ(player->hp, 1);
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Blood Oath can stop an OHKO move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(50); MaxHP(100); Item(ITEM_BLOOD_OATH); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Moves(MOVE_FISSURE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FISSURE); }
    } SCENE {
        HP_BAR(player, hp: 1);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Blood Oath is consumed before a later multi-hit strike")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ARM_THRUST].effect == EFFECT_MULTI_HIT);
        PLAYER(SPECIES_WOBBUFFET) { HP(20); MaxHP(100); Defense(1); Item(ITEM_BLOOD_OATH); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(500); Moves(MOVE_ARM_THRUST); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ARM_THRUST); }
    } SCENE {
        HP_BAR(player, hp: 1);
        HP_BAR(player, hp: 0);
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
    }
}

SINGLE_BATTLE_TEST("Blood Oath does not prevent indirect damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); Status1(STATUS1_POISON); Item(ITEM_BLOOD_OATH); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet fainted!");
    } THEN {
        EXPECT_EQ(GetMonData(&PLAYER_PARTY[0], MON_DATA_HELD_ITEM), ITEM_BLOOD_OATH);
    }
}

SINGLE_BATTLE_TEST("Sturdy and Endure take precedence over Blood Oath")
{
    u16 move;
    u16 ability;

    PARAMETRIZE { move = MOVE_CELEBRATE; ability = ABILITY_STURDY; }
    PARAMETRIZE { move = MOVE_ENDURE;    ability = ABILITY_NONE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_ENDURE].effect == EFFECT_ENDURE);
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); Defense(1); Speed(100); Ability(ability); Item(ITEM_BLOOD_OATH); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(500); Speed(1); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, hp: 1);
    } THEN {
        EXPECT_EQ(player->item, ITEM_BLOOD_OATH);
    }
}

SINGLE_BATTLE_TEST("False Swipe does not consume Blood Oath")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(20); MaxHP(100); Defense(1); Item(ITEM_BLOOD_OATH); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(500); Moves(MOVE_FALSE_SWIPE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FALSE_SWIPE); }
    } SCENE {
        HP_BAR(player, hp: 1);
    } THEN {
        EXPECT_EQ(player->item, ITEM_BLOOD_OATH);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Blood Oath")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(20); MaxHP(100); Ability(ABILITY_KLUTZ); Item(ITEM_BLOOD_OATH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        HP_BAR(player, hp: 0);
    }
}

SINGLE_BATTLE_TEST("Petrified Heart boosts physical and special defense", s16 damage)
{
    u16 item;
    u32 move;

    PARAMETRIZE { item = ITEM_NONE;             move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_PETRIFIED_HEART; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE;             move = MOVE_PSYBEAM; }
    PARAMETRIZE { item = ITEM_PETRIFIED_HEART; move = MOVE_PSYBEAM; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
        ASSUME(gBattleMoves[MOVE_PSYBEAM].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Moves(move); }
    } WHEN {
        TURN { MOVE(opponent, move); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.5), results[0].damage);
        EXPECT_MUL_EQ(results[3].damage, UQ_4_12(1.5), results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Petrified Heart halves Speed and blocks recovery")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); HP(1); MaxHP(100); Item(ITEM_PETRIFIED_HEART); Moves(MOVE_RECOVER, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER, allowed: FALSE); MOVE(player, MOVE_TACKLE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->item, ITEM_PETRIFIED_HEART);
        EXPECT_EQ(GetBattlerHoldEffect(battler, TRUE), HOLD_EFFECT_PETRIFIED_HEART);
        EXPECT_EQ(GetBattlerTotalSpeedStat(battler), 50);
        EXPECT_EQ(IsBattlerHealBlocked(battler), TRUE);
        EXPECT_EQ(player->hp, 1);
    }
}

DOUBLE_BATTLE_TEST("Petrified Heart blocks healing from an ally's Pollen Puff")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_POLLEN_PUFF].effect == EFFECT_HIT_ENEMY_HEAL_ALLY);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); Item(ITEM_PETRIFIED_HEART); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_POLLEN_PUFF); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { MOVE(playerRight, MOVE_POLLEN_PUFF, target: playerLeft); }
    } THEN {
        EXPECT_EQ(playerLeft->hp, 1);
    }
}

SINGLE_BATTLE_TEST("Petrified Heart blocks Leech Seed recovery")
{
    PASSES_RANDOMLY(90, 100, RNG_ACCURACY);
    GIVEN {
        ASSUME(gBattleMoves[MOVE_LEECH_SEED].effect == EFFECT_LEECH_SEED);
        PLAYER(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); Item(ITEM_PETRIFIED_HEART); Moves(MOVE_LEECH_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(100); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_LEECH_SEED); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
        EXPECT_LT(opponent->hp, 100);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Petrified Heart's Speed and healing block")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Speed(100); HP(1); MaxHP(100); Item(ITEM_PETRIFIED_HEART); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerTotalSpeedStat(battler), 100);
        EXPECT_EQ(IsBattlerHealBlocked(battler), FALSE);
        EXPECT_EQ(player->hp, 51);
    }
}

SINGLE_BATTLE_TEST("False Idol suppresses the holder's Ability and boosts direct damage", s16 damage)
{
    u16 item;
    u32 battler;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_FALSE_IDOL; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_STURDY); Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(GetBattlerAbility(battler), item == ITEM_FALSE_IDOL ? ABILITY_NONE : ABILITY_STURDY);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses False Idol", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_FALSE_IDOL; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Attack(120); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        EXPECT_EQ(GetBattlerAbility(GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)), ABILITY_KLUTZ);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Rusted Anchor makes physical moves use Defense", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_RUSTED_ANCHOR; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(40); Defense(200); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Rusted Anchor does not change special moves", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_RUSTED_ANCHOR; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(120); Defense(200); Item(item); Moves(MOVE_PSYBEAM); }
        OPPONENT(SPECIES_WOBBUFFET) { SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYBEAM); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Rusted Anchor", s16 damage)
{
    u16 item;

    PARAMETRIZE { item = ITEM_NONE; }
    PARAMETRIZE { item = ITEM_RUSTED_ANCHOR; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Attack(40); Defense(200); Item(item); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Malice Orb trades the matching defensive stage for an offensive stage")
{
    u16 move;

    PARAMETRIZE { move = MOVE_TACKLE; }
    PARAMETRIZE { move = MOVE_PSYBEAM; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
        ASSUME(gBattleMoves[MOVE_PSYBEAM].split == SPLIT_SPECIAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); SpAttack(120); Item(ITEM_MALICE_ORB); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); SpDefense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        if (move == MOVE_TACKLE)
        {
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
            EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        }
        else
        {
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
            EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
        }
    }
}

SINGLE_BATTLE_TEST("Malice Orb activates once after a multi-hit move")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_ARM_THRUST].split == SPLIT_PHYSICAL);
        PLAYER(SPECIES_WOBBUFFET) { Attack(120); Item(ITEM_MALICE_ORB); Moves(MOVE_ARM_THRUST); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_ARM_THRUST); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Malice Orb does not activate for status moves")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_CELEBRATE].split == SPLIT_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_MALICE_ORB); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Malice Orb")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Attack(120); Item(ITEM_MALICE_ORB); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(120); HP(1000); MaxHP(1000); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Graveglass adds Ghost to monotypes and dual-types")
{
    u16 species;
    u32 battler;

    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    PARAMETRIZE { species = SPECIES_PELIPPER; }

    GIVEN {
        PLAYER(species) { Item(ITEM_GRAVEGLASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        if (species == SPECIES_WOBBUFFET)
        {
            EXPECT_EQ(GetBattlerType(battler, 0, FALSE), TYPE_PSYCHIC);
            EXPECT_EQ(GetBattlerType(battler, 1, FALSE), TYPE_GHOST);
            EXPECT_EQ(GetBattlerType(battler, 2, FALSE), TYPE_MYSTERY);
        }
        else
        {
            EXPECT_EQ(GetBattlerType(battler, 0, FALSE), TYPE_WATER);
            EXPECT_EQ(GetBattlerType(battler, 1, FALSE), TYPE_FLYING);
            EXPECT_EQ(GetBattlerType(battler, 2, FALSE), TYPE_GHOST);
        }
    }
}

SINGLE_BATTLE_TEST("Graveglass grants Ghost escape behavior")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_GRAVEGLASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_MEAN_LOOK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_MEAN_LOOK); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(CanBattlerEscape(battler), TRUE);
        EXPECT_EQ(IsRunningFromBattleImpossible(battler), BATTLE_RUN_SUCCESS);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Graveglass typing and escape behavior")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Item(ITEM_GRAVEGLASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_NE(GetBattlerType(battler, 1, FALSE), TYPE_GHOST);
        EXPECT_EQ(IsAbilityPreventingEscape(battler), GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT) + 1);
    }
}

SINGLE_BATTLE_TEST("Witch's Thread gives status moves +1 priority")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_TAIL_WHIP].split == SPLIT_STATUS);
        PLAYER(SPECIES_WOBBUFFET) { Speed(5); Item(ITEM_WITCHS_THREAD); Moves(MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Witch's Thread reflects Synchronize-compatible status effects to its holder")
{
    u32 move;
    u32 status;

    PARAMETRIZE { move = MOVE_WILL_O_WISP; status = STATUS1_BURN; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; status = STATUS1_PARALYSIS; }
    PARAMETRIZE { move = MOVE_TOXIC; status = STATUS1_TOXIC_POISON; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
        ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].effect == EFFECT_PARALYZE);
        ASSUME(gBattleMoves[MOVE_TOXIC].effect == EFFECT_TOXIC);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WITCHS_THREAD); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT(player->status1 & status);
        EXPECT(opponent->status1 & status);
    }
}

SINGLE_BATTLE_TEST("Witch's Thread respects status immunities when reflecting status")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
        PLAYER(SPECIES_CHARIZARD) { Item(ITEM_WITCHS_THREAD); Moves(MOVE_WILL_O_WISP); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT(player->status1 != STATUS1_BURN);
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Witch's Thread priority and status reflection")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Item(ITEM_WITCHS_THREAD); Moves(MOVE_WILL_O_WISP); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_WILL_O_WISP); }
    } THEN {
        EXPECT_EQ(GetMovePriority(B_POSITION_PLAYER_LEFT, MOVE_WILL_O_WISP), 0);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Gambler's Claw gives first-turn priority and later-turn penalty")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(5); Item(ITEM_GAMBLERS_CLAW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Wobbuffet used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Gambler's Claw priority changes")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Speed(5); Item(ITEM_GAMBLERS_CLAW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Celebrate!");
        MESSAGE("Wobbuffet used Celebrate!");
    }
}

SINGLE_BATTLE_TEST("Ashen Crown raises all stats and marks the holder to perish after a KO")
{
    u32 battler;
    u32 perishSongTimer;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ASHEN_CROWN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(gStatuses3[battler] & STATUS3_PERISH_SONG);
        perishSongTimer = gDisableStructs[battler].perishSongTimer;
        EXPECT_GT(perishSongTimer, 0);
    }
}

SINGLE_BATTLE_TEST("Ashen Crown does not restart or reannounce an active perish countdown")
{
    u32 battler;
    u32 perishSongTimer;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_ASHEN_CROWN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 2); }
    } SCENE {
        MESSAGE("Wobbuffet will perish in three turns!");
        MESSAGE("Wobbuffet's PERISH count fell to 3!");
        NOT MESSAGE("Wobbuffet will perish in three turns!");
        MESSAGE("Wobbuffet's PERISH count fell to 2!");
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 2);
        EXPECT(gStatuses3[battler] & STATUS3_PERISH_SONG);
        perishSongTimer = gDisableStructs[battler].perishSongTimer;
        EXPECT_EQ(perishSongTimer, 1);
    }
}

SINGLE_BATTLE_TEST("Klutz suppresses Ashen Crown")
{
    u32 battler;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_KLUTZ); Item(ITEM_ASHEN_CROWN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } THEN {
        battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        EXPECT_EQ(gStatuses3[battler] & STATUS3_PERISH_SONG, 0);
    }
}
