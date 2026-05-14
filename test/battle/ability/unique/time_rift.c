#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROAR_OF_TIME].effect == EFFECT_RECHARGE);
    ASSUME(gBattleMoves[MOVE_MAGIC_ROOM].effect == EFFECT_MAGIC_ROOM);
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].power > 0);
}

SINGLE_BATTLE_TEST("Time Rift uses a random time move after Roar of Time")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_TIME_RIFT); Moves(MOVE_ROAR_OF_TIME); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR_OF_TIME, WITH_RNG(RNG_ROGUE_TIME_RIFT, MOVE_MAGIC_ROOM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR_OF_TIME, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_TIME_RIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_ROOM, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MAGIC_ROOM);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Time Rift can choose an attacking time move after Roar of Time")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA_ORIGIN) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_TIME_RIFT); Moves(MOVE_ROAR_OF_TIME); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR_OF_TIME, WITH_RNG(RNG_ROGUE_TIME_RIFT, MOVE_ANCIENT_POWER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR_OF_TIME, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_TIME_RIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ANCIENT_POWER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Time Rift does not trigger after other moves")
{
    GIVEN {
        PLAYER(SPECIES_DIALGA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_TIME_RIFT); Moves(MOVE_DRAGON_CLAW); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_CLAW, WITH_RNG(RNG_ROGUE_TIME_RIFT, MOVE_MAGIC_ROOM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_CLAW, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TIME_RIFT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_ROOM, player);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

SINGLE_BATTLE_TEST("Time Rift no longer triggers on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DRATINI) { Ability(ABILITY_SHED_SKIN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_DIALGA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_TIME_RIFT); Moves(MOVE_ROAR_OF_TIME); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(opponent, ABILITY_TIME_RIFT);
    } THEN {
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_OPPONENT] & gBitTable[1], 0);
    }
}
