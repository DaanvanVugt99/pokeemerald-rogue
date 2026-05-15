#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SOLAR_BEAM].effect == EFFECT_SOLAR_BEAM);
    ASSUME(gBattleMoves[MOVE_WEATHER_BALL].effect == EFFECT_WEATHER_BALL);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_SPIKY_SHIELD].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_BANEFUL_BUNKER].effect == EFFECT_PROTECT);
    ASSUME(gItems[ITEM_UTILITY_UMBRELLA].holdEffect == HOLD_EFFECT_UTILITY_UMBRELLA);
}

SINGLE_BATTLE_TEST("Mega Sol lets Solar Beam skip its charge turn and rain damage penalty", s16 damage)
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_RAIN_DANCE; }

    GIVEN {
        PLAYER(SPECIES_MEGANIUM_MEGA) { Ability(ABILITY_MEGA_SOL); Speed(1); Moves(MOVE_SOLAR_BEAM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE, MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { MOVE(opponent, setupMove); MOVE(player, MOVE_SOLAR_BEAM); }
    } SCENE {
        NOT MESSAGE("Meganium took in sunlight!");
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } THEN {
        if (setupMove == MOVE_CELEBRATE)
            EXPECT_EQ(gBattleWeather, B_WEATHER_NONE);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mega Sol makes Weather Ball Fire-type even through rain and Utility Umbrella", s16 damage)
{
    u32 ability;
    u32 item;
    u32 setupMove;
    PARAMETRIZE { ability = ABILITY_NONE;     item = ITEM_NONE;             setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_UTILITY_UMBRELLA; setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_NONE;             setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_NONE;             setupMove = MOVE_RAIN_DANCE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); Moves(MOVE_WEATHER_BALL, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEGANIUM) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE, MOVE_RAIN_DANCE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_WEATHER_BALL); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[2].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(6.0), results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Mega Sol boosts Fire moves and weakens Water moves", s16 damage)
{
    u32 ability;
    u32 item;
    u32 targetItem;
    u32 move;
    PARAMETRIZE { ability = ABILITY_NONE;     item = ITEM_NONE;             targetItem = ITEM_NONE;             move = MOVE_EMBER; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_UTILITY_UMBRELLA; targetItem = ITEM_NONE;             move = MOVE_EMBER; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_NONE;             targetItem = ITEM_NONE;             move = MOVE_EMBER; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_NONE;             targetItem = ITEM_UTILITY_UMBRELLA; move = MOVE_EMBER; }
    PARAMETRIZE { ability = ABILITY_NONE;     item = ITEM_NONE;             targetItem = ITEM_NONE;             move = MOVE_WATER_GUN; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_UTILITY_UMBRELLA; targetItem = ITEM_NONE;             move = MOVE_WATER_GUN; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_NONE;             targetItem = ITEM_NONE;             move = MOVE_WATER_GUN; }
    PARAMETRIZE { ability = ABILITY_MEGA_SOL; item = ITEM_NONE;             targetItem = ITEM_UTILITY_UMBRELLA; move = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Item(item); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Item(targetItem); HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[1].damage, results[2].damage);
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[2].damage);
        EXPECT_EQ(results[0].damage, results[3].damage);
        EXPECT_EQ(results[5].damage, results[6].damage);
        EXPECT_MUL_EQ(results[4].damage, Q_4_12(0.5), results[6].damage);
        EXPECT_EQ(results[4].damage, results[7].damage);
    }
}

SINGLE_BATTLE_TEST("Dragonize turns Normal damaging moves into Dragon-type moves and boosts them", s16 damage)
{
    u32 ability;
    PARAMETRIZE { ability = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_DRAGONIZE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Dragonize converted moves respect Fairy immunity")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_DRAGONIZE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WIGGLYTUFF) { HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Piercing Drill contact moves hit through Protect for quarter damage", s16 damage)
{
    u32 protectMove;
    PARAMETRIZE { protectMove = MOVE_PROTECT; }
    PARAMETRIZE { protectMove = MOVE_CELEBRATE; }
    PARAMETRIZE { protectMove = MOVE_QUICK_GUARD; }
    PARAMETRIZE { protectMove = MOVE_WIDE_GUARD; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PIERCING_DRILL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_PROTECT, MOVE_CELEBRATE, MOVE_QUICK_GUARD, MOVE_WIDE_GUARD); }
    } WHEN {
        TURN { MOVE(opponent, protectMove); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(4.0), results[1].damage);
        EXPECT_EQ(results[1].damage, results[2].damage);
        EXPECT_EQ(results[1].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Piercing Drill still triggers Spiky Shield side effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PIERCING_DRILL); HP(80); MaxHP(80); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_SPIKY_SHIELD); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKY_SHIELD); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent);
        HP_BAR(player);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Piercing Drill still triggers Baneful Bunker side effects")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PIERCING_DRILL); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_BANEFUL_BUNKER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_BANEFUL_BUNKER); MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT(player->status1 & STATUS1_POISON);
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Piercing Drill non-contact moves do not bypass Protect")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_PIERCING_DRILL); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray burns attackers after direct move damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SPICY_SPRAY); HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN); }
    } THEN {
        EXPECT(player->status1 & STATUS1_BURN);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray respects burn immunity")
{
    GIVEN {
        PLAYER(SPECIES_CHARMANDER) { Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SPICY_SPRAY); HP(500); MaxHP(500); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Spicy Spray does not trigger from indirect damage")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_TOXIC, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SPICY_SPRAY); HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
