#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAINING_KISS].effect == EFFECT_ABSORB);
    ASSUME(gBattleMoves[MOVE_TACKLE].power == 40);
    ASSUME(gBattleMoves[MOVE_SWIFT].power == 60);
    ASSUME(gBattleMoves[MOVE_SLASH].power > 60);
}

SINGLE_BATTLE_TEST("Toxic Vanity uses Draining Kiss after the first move with 60 or less base power each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TOXIC_VANITY); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_WYNAUT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        ABILITY_POPUP(player, ABILITY_TOXIC_VANITY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAINING_KISS, player);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Toxic Vanity does not trigger after moves above 60 base power")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TOXIC_VANITY); Moves(MOVE_SLASH); }
        OPPONENT(SPECIES_WYNAUT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SLASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SLASH, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TOXIC_VANITY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAINING_KISS, player);
        }
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Toxic Vanity only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TOXIC_VANITY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_VANITY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAINING_KISS, player);
        NOT ABILITY_POPUP(player, ABILITY_TOXIC_VANITY);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Toxic Vanity Draining Kiss heals and has double power against poisoned targets", u16 hp, u16 userHp)
{
    u32 status;
    PARAMETRIZE { status = STATUS1_NONE; }
    PARAMETRIZE { status = STATUS1_POISON; }

    GIVEN {
        PLAYER(SPECIES_SENTRET) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TOXIC_VANITY); HP(100); MaxHP(200); SpAttack(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); SpDefense(100); Status1(status); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TOXIC_VANITY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAINING_KISS, player);
    } THEN {
        results[i].hp = opponent->hp;
        results[i].userHp = player->hp;
    } FINALLY {
        EXPECT(results[1].hp < results[0].hp);
        EXPECT(results[0].userHp > 100);
        EXPECT(results[1].userHp > results[0].userHp);
    }
}
