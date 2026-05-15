#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RUINATION].effect == EFFECT_SUPER_FANG);
}

SINGLE_BATTLE_TEST("Cursed Tablet uses Ruination after the first super-effective hit each battle")
{
    GIVEN {
        PLAYER(SPECIES_RUNERIGUS) { HP(300); MaxHP(300); Ability(ABILITY_WANDERING_SPIRIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RUINATION, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP / 2);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Cursed Tablet works for Galarian Yamask")
{
    GIVEN {
        PLAYER(SPECIES_YAMASK_GALARIAN) { HP(300); MaxHP(300); Ability(ABILITY_WANDERING_SPIRIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RUINATION, player);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Cursed Tablet only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_RUNERIGUS) { HP(500); MaxHP(500); Ability(ABILITY_WANDERING_SPIRIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Ability(ABILITY_SHIELD_DUST); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RUINATION, player);
        NOT ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Cursed Tablet does not trigger after neutral damage")
{
    GIVEN {
        PLAYER(SPECIES_RUNERIGUS) { HP(300); MaxHP(300); Ability(ABILITY_WANDERING_SPIRIT); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHIELD_DUST); Moves(MOVE_PSYBEAM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYBEAM); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_CURSED_TABLET);
    } THEN {
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}
