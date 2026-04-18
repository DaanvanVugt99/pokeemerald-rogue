#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
}

SINGLE_BATTLE_TEST("Fortified Spin clears hazards on Protect and heals 1/8 max HP")
{
    GIVEN {
        PLAYER(SPECIES_PINECO) { HP(200); MaxHP(240); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_FORTIFIED_SPIN); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_STEALTH_ROCK); }
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        ABILITY_POPUP(player, ABILITY_FORTIFIED_SPIN);
    } THEN {
        EXPECT_EQ(player->hp, 230);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_STEALTH_ROCK));
    }
}

SINGLE_BATTLE_TEST("Fortified Spin still clears hazards on Protect at full HP")
{
    GIVEN {
        PLAYER(SPECIES_FORRETRESS) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_FORTIFIED_SPIN); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        ABILITY_POPUP(player, ABILITY_FORTIFIED_SPIN);
        NONE_OF {
            MESSAGE("Forretress's Fortified Spin restored its HP a little!");
            HP_BAR(player);
        }
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}

SINGLE_BATTLE_TEST("Fortified Spin still clears hazards while Heal Blocked")
{
    GIVEN {
        PLAYER(SPECIES_PINECO) { HP(200); MaxHP(240); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_FORTIFIED_SPIN); Moves(MOVE_CELEBRATE, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_HEAL_BLOCK, MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_HEAL_BLOCK); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_PROTECT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HEAL_BLOCK, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, player);
        ABILITY_POPUP(player, ABILITY_FORTIFIED_SPIN);
        NONE_OF {
            MESSAGE("Pineco's Fortified Spin restored its HP a little!");
            HP_BAR(player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 200);
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_SPIKES));
    }
}
