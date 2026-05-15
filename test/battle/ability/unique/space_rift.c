#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPACIAL_REND].power > 0);
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
    ASSUME(gBattleMoves[MOVE_ANCIENT_POWER].power > 0);
}

SINGLE_BATTLE_TEST("Space Rift uses a random space move after Spacial Rend")
{
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SPACE_RIFT); Moves(MOVE_SPACIAL_REND); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPACIAL_REND, WITH_RNG(RNG_ROGUE_SPACE_RIFT, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPACIAL_REND, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SPACE_RIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Space Rift can choose an attacking space move after Spacial Rend")
{
    GIVEN {
        PLAYER(SPECIES_PALKIA_ORIGIN) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SPACE_RIFT); Moves(MOVE_SPACIAL_REND); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPACIAL_REND, WITH_RNG(RNG_ROGUE_SPACE_RIFT, MOVE_ANCIENT_POWER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPACIAL_REND, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SPACE_RIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ANCIENT_POWER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Space Rift can choose Teleport after Spacial Rend")
{
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SPACE_RIFT); Moves(MOVE_SPACIAL_REND); }
        PLAYER(SPECIES_DIALGA) { Speed(50); Ability(ABILITY_PRESSURE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(25); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPACIAL_REND, WITH_RNG(RNG_ROGUE_SPACE_RIFT, MOVE_TELEPORT)); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPACIAL_REND, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_SPACE_RIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TELEPORT, player);
        MESSAGE("Do it! Dialga!");
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_TELEPORT);
        EXPECT_EQ(gBattlerPartyIndexes[B_POSITION_PLAYER_LEFT], 1);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Space Rift only triggers once each battle")
{
    GIVEN {
        PLAYER(SPECIES_PALKIA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SPACE_RIFT); Moves(MOVE_SPACIAL_REND); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPACIAL_REND, WITH_RNG(RNG_ROGUE_SPACE_RIFT, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SPACIAL_REND, WITH_RNG(RNG_ROGUE_SPACE_RIFT, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPACE_RIFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        NOT ABILITY_POPUP(player, ABILITY_SPACE_RIFT);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
