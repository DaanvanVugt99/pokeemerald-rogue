#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].effect == EFFECT_RECOIL_25);
    ASSUME(gBattleMoves[MOVE_BRAVE_BIRD].effect == EFFECT_RECOIL_33);
}

SINGLE_BATTLE_TEST("Warpath recoil moves heal the user for 1/4 of damage dealt", s16 selfDamage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_WARPATH; }

    GIVEN {
        PLAYER(SPECIES_EEVEE) { HP(200); MaxHP(200); Ability(ABILITY_RUN_AWAY); UniqueAbility(uniqueAbility); Moves(MOVE_BRAVE_BIRD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BRAVE_BIRD); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        results[i].selfDamage = player->maxHP - player->hp;
    } FINALLY {
        EXPECT_LT(results[1].selfDamage, results[0].selfDamage);
    }
}

SINGLE_BATTLE_TEST("Warpath on the opposing battler doubles recoil damage taken", s16 selfDamage)
{
    u16 targetUniqueAbility;

    PARAMETRIZE { targetUniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { targetUniqueAbility = ABILITY_WARPATH; }

    GIVEN {
        PLAYER(SPECIES_EEVEE) { HP(200); MaxHP(200); Ability(ABILITY_RUN_AWAY); UniqueAbility(ABILITY_NONE); Moves(MOVE_TAKE_DOWN); }
        OPPONENT(SPECIES_EEVEE) { HP(500); MaxHP(500); Ability(ABILITY_RUN_AWAY); UniqueAbility(targetUniqueAbility); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_DOWN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        results[i].selfDamage = player->maxHP - player->hp;
    } FINALLY {
        EXPECT_MUL_EQ(results[0].selfDamage, Q_4_12(2.0), results[1].selfDamage);
    }
}

SINGLE_BATTLE_TEST("Warpath doubles recoil even if the recoil hit KOs the target", s16 selfDamage)
{
    u16 targetUniqueAbility;

    PARAMETRIZE { targetUniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { targetUniqueAbility = ABILITY_WARPATH; }

    GIVEN {
        PLAYER(SPECIES_EEVEE) { HP(300); MaxHP(300); Ability(ABILITY_RUN_AWAY); Moves(MOVE_TAKE_DOWN); Attack(200); }
        OPPONENT(SPECIES_SHEDINJA) { HP(1); MaxHP(1); Ability(ABILITY_WONDER_GUARD); UniqueAbility(targetUniqueAbility); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_DOWN); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        results[i].selfDamage = player->maxHP - player->hp;
    } FINALLY {
        EXPECT_MUL_EQ(results[0].selfDamage, Q_4_12(2.0), results[1].selfDamage);
    }
}
