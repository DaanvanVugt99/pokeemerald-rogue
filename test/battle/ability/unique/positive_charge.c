#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_PULSE].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_GASTRO_ACID].effect == EFFECT_GASTRO_ACID);
}

SINGLE_BATTLE_TEST("Positive Charge boosts special damage if the party contains a Minus ability Pokemon", s16 damage)
{
    u16 teammateSpecies;
    u16 teammateAbility;

    PARAMETRIZE { teammateSpecies = SPECIES_MINUN; teammateAbility = ABILITY_MINUS; }
    PARAMETRIZE { teammateSpecies = SPECIES_PIKACHU; teammateAbility = ABILITY_STATIC; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(10); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_POSITIVE_CHARGE); Moves(MOVE_WATER_PULSE); }
        PLAYER(teammateSpecies) { Speed(1); Ability(teammateAbility); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(5); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.2), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Positive Charge boosts Speed only if the party contains a Minus ability Pokemon")
{
    u16 teammateSpecies;
    u16 teammateAbility;

    PARAMETRIZE { teammateSpecies = SPECIES_MINUN; teammateAbility = ABILITY_MINUS; }
    PARAMETRIZE { teammateSpecies = SPECIES_PIKACHU; teammateAbility = ABILITY_STATIC; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(80); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_POSITIVE_CHARGE); Moves(MOVE_TACKLE); }
        PLAYER(teammateSpecies) { Speed(1); Ability(teammateAbility); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(90); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (teammateAbility == ABILITY_MINUS)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        }
    }
}

DOUBLE_BATTLE_TEST("Positive Charge uses the active teammate's live ability, not its stored party ability", s16 damage)
{
    bool32 suppressMinus;

    PARAMETRIZE { suppressMinus = FALSE; }
    PARAMETRIZE { suppressMinus = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_POSITIVE_CHARGE); Moves(MOVE_CELEBRATE, MOVE_WATER_PULSE); }
        PLAYER(SPECIES_MINUN) { Speed(40); Ability(ABILITY_MINUS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(suppressMinus ? MOVE_GASTRO_ACID : MOVE_CELEBRATE, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, suppressMinus ? MOVE_GASTRO_ACID : MOVE_CELEBRATE, target: playerRight); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_WATER_PULSE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(1.2), results[0].damage);
    }
}
