#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Adaptive Origin gives STAB to off-type moves if all party Pokemon have unique types", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_ADAPTIVE_ORIGIN; }
    PARAMETRIZE { ability = ABILITY_PRESSURE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);

        PLAYER(SPECIES_MEW)        { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ability); Moves(MOVE_THUNDERBOLT); }
        PLAYER(SPECIES_CHARIZARD)  { Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_PIKACHU)    { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Adaptive Origin does not give STAB to off-type moves if party types are not unique", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_ADAPTIVE_ORIGIN; }
    PARAMETRIZE { ability = ABILITY_PRESSURE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);

        PLAYER(SPECIES_MEW)         { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ability); Moves(MOVE_THUNDERBOLT); }
        PLAYER(SPECIES_CHARIZARD)   { Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_FARFETCHD)   { Ability(ABILITY_KEEN_EYE); } // Shares Flying with Charizard
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
