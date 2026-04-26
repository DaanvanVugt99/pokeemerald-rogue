#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(!IS_MOVE_STATUS(MOVE_HYPER_VOICE));
    ASSUME(IS_MOVE_STATUS(MOVE_MEMENTO));
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_METAL_SOUND].effect == EFFECT_SPECIAL_DEFENSE_DOWN_2);
    ASSUME(gBattleMoves[MOVE_METAL_SOUND].soundMove);
    ASSUME(gBattleMoves[MOVE_MEMENTO].effect == EFFECT_MEMENTO);
}

SINGLE_BATTLE_TEST("Regal Decree triggers only on the first status move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_EMPOLEON) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_REGAL_DECREE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_REGAL_DECREE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METAL_SOUND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_REGAL_DECREE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METAL_SOUND, player);
        }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Regal Decree does not consume its trigger on damaging moves")
{
    GIVEN {
        PLAYER(SPECIES_EMPOLEON) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_REGAL_DECREE); Moves(MOVE_HYPER_VOICE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        ABILITY_POPUP(player, ABILITY_REGAL_DECREE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METAL_SOUND, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 2);
    }
}

SINGLE_BATTLE_TEST("Regal Decree refreshes after the user switches out and back in")
{
    GIVEN {
        PLAYER(SPECIES_EMPOLEON) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_REGAL_DECREE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_REGAL_DECREE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METAL_SOUND, player);
        ABILITY_POPUP(player, ABILITY_REGAL_DECREE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_METAL_SOUND, player);
    }
}

SINGLE_BATTLE_TEST("Regal Decree makes sound-based moves Steel-type", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_STILL_WATER; }
    PARAMETRIZE { uniqueAbility = ABILITY_REGAL_DECREE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { SpAttack(200); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_CARBINK) { HP(1000); MaxHP(1000); SpDefense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Regal Decree does not call Metal Sound if the user faints before move end")
{
    GIVEN {
        PLAYER(SPECIES_EMPOLEON) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_REGAL_DECREE); Moves(MOVE_MEMENTO); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEMENTO); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEMENTO, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_REGAL_DECREE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_METAL_SOUND, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 0);
    }
}
