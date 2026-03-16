#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
}

SINGLE_BATTLE_TEST("Unique abilities can affect battle behavior")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { UniqueAbility(ABILITY_LEVITATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("2 sent out Wynaut!");
        NOT MESSAGE("Foe Wynaut is hurt by spikes!");
    }
}

SINGLE_BATTLE_TEST("Unique abilities are suppressed by Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT) { UniqueAbility(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_KOFFING) { Ability(ABILITY_NEUTRALIZING_GAS); Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        s32 maxHP = GetMonData(&PLAYER_PARTY[1], MON_DATA_MAX_HP);

        HP_BAR(player, damage: maxHP / 8);
        MESSAGE("Wynaut is hurt by spikes!");
    }
}
