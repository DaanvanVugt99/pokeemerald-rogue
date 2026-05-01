#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SANDSTORM].effect == EFFECT_SANDSTORM);
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Primal Storm uses Sandstorm after knocking out a target")
{
    GIVEN {
        PLAYER(SPECIES_TYRANTRUM) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_PRIMAL_STORM); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CATERPIE) { HP(1); MaxHP(400); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PRIMAL_STORM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SANDSTORM, player);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SANDSTORM);
    }
}

SINGLE_BATTLE_TEST("Primal Storm does not use Sandstorm without a knockout")
{
    GIVEN {
        PLAYER(SPECIES_TYRANTRUM) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_PRIMAL_STORM); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PRIMAL_STORM);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SANDSTORM));
    }
}

SINGLE_BATTLE_TEST("Primal Storm boosts biting move damage in Sandstorm", s16 damage)
{
    bool32 sandstorm;

    PARAMETRIZE { sandstorm = FALSE; }
    PARAMETRIZE { sandstorm = TRUE; }

    GIVEN {
        PLAYER(SPECIES_TYRANTRUM) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_PRIMAL_STORM); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SANDSTORM, MOVE_SPLASH); }
    } WHEN {
        if (sandstorm)
            TURN { MOVE(opponent, MOVE_SANDSTORM); MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); }
        else
            TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_SECONDARY_EFFECT, FALSE)); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Primal Storm does not boost non-biting move damage in Sandstorm", s16 damage)
{
    bool32 sandstorm;

    PARAMETRIZE { sandstorm = FALSE; }
    PARAMETRIZE { sandstorm = TRUE; }

    GIVEN {
        PLAYER(SPECIES_TYRANTRUM) { Ability(ABILITY_STENCH); UniqueAbility(ABILITY_PRIMAL_STORM); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Defense(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SANDSTORM, MOVE_SPLASH); }
    } WHEN {
        if (sandstorm)
            TURN { MOVE(opponent, MOVE_SANDSTORM); MOVE(player, MOVE_TACKLE); }
        else
            TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
