#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
    ASSUME(gBattleMoves[MOVE_MIST].effect == EFFECT_MIST);
}

SINGLE_BATTLE_TEST("Freshly Baked restores HP and sets Mist on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_DACHSBUN) { HP(60); MaxHP(90); Ability(ABILITY_WELL_BAKED_BODY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE, MOVE_GROWL); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FRESHLY_BAKED);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MIST, player);
        MESSAGE("Dachsbun is protected by MIST!");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_MIST);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
