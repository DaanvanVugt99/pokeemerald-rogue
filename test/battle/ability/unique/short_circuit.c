#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Short Circuit boosts Electric-type moves by 20 percent per fainted ally if the team shares a type", s16 damage)
{
    bool32 hasFaintedAlly;
    PARAMETRIZE { hasFaintedAlly = FALSE; }
    PARAMETRIZE { hasFaintedAlly = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ROTOM) { Speed(100); Ability(ABILITY_LEVITATE); Moves(MOVE_THUNDER_SHOCK); }
        if (hasFaintedAlly)
            PLAYER(SPECIES_PICHU) { HP(0); Speed(1); Ability(ABILITY_STATIC); }
        else
            PLAYER(SPECIES_PICHU) { Speed(1); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Short Circuit requires all living team members to share a type with the user", s16 damage)
{
    bool32 teamSharesType;
    PARAMETRIZE { teamSharesType = FALSE; }
    PARAMETRIZE { teamSharesType = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ROTOM) { Speed(100); Ability(ABILITY_LEVITATE); Moves(MOVE_THUNDER_SHOCK); }
        PLAYER(SPECIES_PICHU) { HP(0); Speed(1); Ability(ABILITY_STATIC); }
        if (teamSharesType)
            PLAYER(SPECIES_PIKACHU) { Speed(1); Ability(ABILITY_STATIC); }
        else
            PLAYER(SPECIES_GEODUDE) { Speed(1); Ability(ABILITY_ROCK_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.2), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Short Circuit does not boost non-Electric moves", s16 damage)
{
    bool32 hasFaintedAlly;
    PARAMETRIZE { hasFaintedAlly = FALSE; }
    PARAMETRIZE { hasFaintedAlly = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ROTOM) { Speed(100); Ability(ABILITY_LEVITATE); Moves(MOVE_TACKLE); }
        if (hasFaintedAlly)
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
