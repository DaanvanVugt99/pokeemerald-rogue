#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LICK].power > 0);
    ASSUME(gBattleMoves[MOVE_LICK].secondaryEffectChance == 30);
}

SINGLE_BATTLE_TEST("Taste Test uses Lick immediately on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_LICKITUNG) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_TASTE_TEST); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LICK, opponent);
    } THEN {
        EXPECT(player->hp < player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Taste Test coexists with primary switch-in abilities")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ARBOK) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_TASTE_TEST); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(player->hp < player->maxHP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Taste Test's Lick can paralyze on switch-in")
{
    PASSES_RANDOMLY(3, 10, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_LICKITUNG) { Ability(ABILITY_OWN_TEMPO); UniqueAbility(ABILITY_TASTE_TEST); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(player->status1 & STATUS1_PARALYSIS, 0);
    }
}
