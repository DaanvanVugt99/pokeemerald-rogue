#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > gBattleMoves[MOVE_TACKLE].priority);
}

SINGLE_BATTLE_TEST("Pressure Valve gives +1 priority to the next move after a Fire-type hit")
{
    GIVEN {
        PLAYER(SPECIES_VOLCANION) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_PRESSURE_VALVE); Moves(MOVE_QUICK_ATTACK, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); SpAttack(100); Moves(MOVE_EMBER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_EMBER); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, opponent);
        ABILITY_POPUP(player, ABILITY_PRESSURE_VALVE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Pressure Valve boosts the next damaging move after a Water-type hit", s16 damage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_WATER_GUN; }

    GIVEN {
        PLAYER(SPECIES_VOLCANION) { HP(400); MaxHP(400); Speed(60); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_PRESSURE_VALVE); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); SpAttack(100); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Pressure Valve is consumed by a status move")
{
    GIVEN {
        PLAYER(SPECIES_VOLCANION) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_PRESSURE_VALVE); Moves(MOVE_QUICK_ATTACK, MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); SpAttack(100); Moves(MOVE_WATER_GUN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        ABILITY_POPUP(player, ABILITY_PRESSURE_VALVE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Pressure Valve does not trigger after other move types")
{
    GIVEN {
        PLAYER(SPECIES_VOLCANION) { HP(400); MaxHP(400); Speed(50); Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_PRESSURE_VALVE); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Attack(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PRESSURE_VALVE);
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}
