#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
}

SINGLE_BATTLE_TEST("Short Circuit boosts Electric-type moves by 20 percent per fainted Electric ally", s16 damage)
{
    u16 faintedSpecies;
    PARAMETRIZE { faintedSpecies = SPECIES_NONE; }
    PARAMETRIZE { faintedSpecies = SPECIES_PICHU; }
    PARAMETRIZE { faintedSpecies = SPECIES_GASTLY; }

    GIVEN {
        PLAYER(SPECIES_ROTOM) { Speed(100); Ability(ABILITY_STATIC); Moves(MOVE_THUNDER_SHOCK); }
        if (faintedSpecies != SPECIES_NONE)
            PLAYER(faintedSpecies) { HP(0); Speed(1); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
        EXPECT_EQ(results[0].damage, results[2].damage);
    }
}

SINGLE_BATTLE_TEST("Short Circuit does not boost non-Electric moves", s16 damage)
{
    bool32 hasFaintedElectricAlly;
    PARAMETRIZE { hasFaintedElectricAlly = FALSE; }
    PARAMETRIZE { hasFaintedElectricAlly = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ROTOM) { Speed(100); Ability(ABILITY_STATIC); Moves(MOVE_TACKLE); }
        if (hasFaintedElectricAlly)
            PLAYER(SPECIES_PICHU) { HP(0); Speed(1); Ability(ABILITY_STATIC); }
        else
            PLAYER(SPECIES_PICHU) { Speed(1); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Short Circuit makes the user immune to Ground-type moves without Levitate")
{
    GIVEN {
        PLAYER(SPECIES_ROTOM) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_SHORT_CIRCUIT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHORT_CIRCUIT);
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
            HP_BAR(player);
        }
    }
}

SINGLE_BATTLE_TEST("Short Circuit's Ground immunity is bypassed by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_ROTOM) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_SHORT_CIRCUIT); }
        OPPONENT(SPECIES_PINSIR) { Ability(ABILITY_MOLD_BREAKER); Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, opponent);
        HP_BAR(player);
    }
}

SINGLE_BATTLE_TEST("Short Circuit makes the user ungrounded without Levitate")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ROTOM) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_SHORT_CIRCUIT); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { MOVE(player, MOVE_CELEBRATE); SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("2 sent out Rotom!");
        NOT MESSAGE("Foe Rotom is hurt by spikes!");
    }
}
