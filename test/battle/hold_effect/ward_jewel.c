#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ward Jewel substitutes Special Defense for Special Attack", s16 damage)
{
    u16 item, spAttack, move;

    PARAMETRIZE { item = ITEM_NONE; spAttack = 40; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_NONE; spAttack = 200; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; spAttack = 40; move = MOVE_WATER_GUN; }
    PARAMETRIZE { item = ITEM_NONE; spAttack = 200; move = MOVE_PSYSHOCK; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; spAttack = 40; move = MOVE_PSYSHOCK; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); SpAttack(spAttack); SpDefense(200); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Defense(100); SpDefense(150); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[2].damage, results[0].damage);
        EXPECT_EQ(results[2].damage, results[1].damage);
        EXPECT_EQ(results[4].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Ward Jewel preserves physical and fixed-damage moves", s16 damage)
{
    u16 item, move;

    PARAMETRIZE { item = ITEM_NONE; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_BODY_PRESS; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; move = MOVE_BODY_PRESS; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_FOUL_PLAY; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; move = MOVE_FOUL_PLAY; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_JETSTREAM; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; move = MOVE_JETSTREAM; }
    PARAMETRIZE { item = ITEM_NONE; move = MOVE_DRAGON_RAGE; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; move = MOVE_DRAGON_RAGE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(item); Attack(40); Defense(80); SpDefense(200); Speed(120); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Attack(160); Defense(120); Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
        EXPECT_EQ(results[6].damage, results[7].damage);
        EXPECT_EQ(results[8].damage, results[9].damage);
    }
}

SINGLE_BATTLE_TEST("Ward Jewel uses Special Defense stages and respects Unaware", s16 damage)
{
    u16 setupMove, ability;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { setupMove = MOVE_AMNESIA; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { setupMove = MOVE_NASTY_PLOT; ability = ABILITY_SHADOW_TAG; }
    PARAMETRIZE { setupMove = MOVE_AMNESIA; ability = ABILITY_UNAWARE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WARD_JEWEL); SpAttack(40); SpDefense(120); Moves(setupMove, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ability); HP(1000); MaxHP(1000); SpDefense(120); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
        EXPECT_EQ(results[2].damage, results[0].damage);
        EXPECT_EQ(results[3].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Ward Jewel critical hits ignore Special Defense drops", s16 damage)
{
    u16 setupMove;
    bool32 critical;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; critical = FALSE; }
    PARAMETRIZE { setupMove = MOVE_FAKE_TEARS; critical = FALSE; }
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; critical = TRUE; }
    PARAMETRIZE { setupMove = MOVE_FAKE_TEARS; critical = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_WARD_JEWEL); SpAttack(40); SpDefense(120); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(120); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_CRITICAL_HIT, critical)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_LT(results[1].damage, results[0].damage);
        EXPECT_EQ(results[3].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Ward Jewel is suppressed by Klutz, Magic Room, and Embargo", s16 damage)
{
    u16 item, ability, setupMove;

    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_KLUTZ; setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; ability = ABILITY_KLUTZ; setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; setupMove = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; ability = ABILITY_SHADOW_TAG; setupMove = MOVE_MAGIC_ROOM; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; setupMove = MOVE_EMBARGO; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; ability = ABILITY_SHADOW_TAG; setupMove = MOVE_EMBARGO; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); SpAttack(40); SpDefense(200); Moves(MOVE_CELEBRATE, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(120); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[4].damage, results[5].damage);
    }
}

SINGLE_BATTLE_TEST("Ward Jewel avoids Special Attack penalties including Frostbite and Defeatist", s16 damage)
{
    u16 item, ability;
    u32 status;

    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_NONE; ability = ABILITY_SHADOW_TAG; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; ability = ABILITY_SHADOW_TAG; status = STATUS1_NONE; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; ability = ABILITY_SHADOW_TAG; status = STATUS1_FROSTBITE; }
    PARAMETRIZE { item = ITEM_WARD_JEWEL; ability = ABILITY_DEFEATIST; status = STATUS1_NONE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); Status1(status); HP(400); MaxHP(1000); SpAttack(120); SpDefense(200); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(120); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_EQ(results[2].damage, results[4].damage);
    }
}
