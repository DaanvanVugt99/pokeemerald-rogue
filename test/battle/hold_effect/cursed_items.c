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
