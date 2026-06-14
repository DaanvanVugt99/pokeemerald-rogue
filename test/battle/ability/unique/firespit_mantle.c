#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_EARTH_POWER].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_EARTH_POWER].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_SURF].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_SURF].target == MOVE_TARGET_FOES_AND_ALLY);
}

SINGLE_BATTLE_TEST("Firespit Mantle halves the first Water or Ground hit each switch-in and burns the attacker", s16 firstHit, s16 secondHit)
{
    u32 move;

    PARAMETRIZE { move = MOVE_WATER_GUN; }
    PARAMETRIZE { move = MOVE_EARTH_POWER; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpAttack(100); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(move); }
        OPPONENT(SPECIES_ARCANINE_HISUIAN) { HP(2000); MaxHP(2000); SpDefense(100); Speed(1); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FIRESPIT_MANTLE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, move, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].firstHit);
        ABILITY_POPUP(opponent, ABILITY_FIRESPIT_MANTLE);
        STATUS_ICON(player, burn: TRUE);
        HP_BAR(opponent, captureDamage: &results[i].secondHit);
    } THEN {
        EXPECT_MUL_EQ(results[i].firstHit, UQ_4_12(2.0), results[i].secondHit);
        EXPECT(player->status1 & STATUS1_BURN);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

DOUBLE_BATTLE_TEST("Firespit Mantle burns after being hit by a spread move before the final target")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpAttack(100); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SURF); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARCANINE_HISUIAN) { HP(2000); MaxHP(2000); SpDefense(100); Speed(1); Ability(ABILITY_BATTLE_ARMOR); UniqueAbility(ABILITY_FIRESPIT_MANTLE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); SpDefense(100); Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_SURF, WITH_RNG(RNG_DAMAGE_MODIFIER, 100));
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SURF, playerLeft);
        ABILITY_POPUP(opponentLeft, ABILITY_FIRESPIT_MANTLE);
        STATUS_ICON(playerLeft, burn: TRUE);
    } THEN {
        EXPECT(playerLeft->status1 & STATUS1_BURN);
    }
}
