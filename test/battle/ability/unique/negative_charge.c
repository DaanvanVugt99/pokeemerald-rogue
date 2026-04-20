#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PSYCHIC].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_GASTRO_ACID].effect == EFFECT_GASTRO_ACID);
}

SINGLE_BATTLE_TEST("Negative Charge reduces damage if the party contains a Plus ability Pokemon", s16 damage)
{
    u16 teammateSpecies;
    u16 teammateAbility;

    PARAMETRIZE { teammateSpecies = SPECIES_PLUSLE; teammateAbility = ABILITY_PLUS; }
    PARAMETRIZE { teammateSpecies = SPECIES_PIKACHU; teammateAbility = ABILITY_STATIC; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_NEGATIVE_CHARGE); Moves(MOVE_CELEBRATE); }
        PLAYER(teammateSpecies) { Ability(teammateAbility); }
        OPPONENT(SPECIES_ALAKAZAM) { Moves(MOVE_PSYCHIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(0.8), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Negative Charge restores 1/16 max HP each turn if the party contains a Plus ability Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(150); MaxHP(160); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_NEGATIVE_CHARGE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PLUSLE) { Ability(ABILITY_PLUS); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NEGATIVE_CHARGE);
        MESSAGE("Wobbuffet's Negative Charge restored its HP a little!");
        HP_BAR(player, damage: -10);
    }
}

DOUBLE_BATTLE_TEST("Negative Charge uses the active teammate's live ability, not its stored party ability", s16 damage)
{
    bool32 suppressPlus;

    PARAMETRIZE { suppressPlus = FALSE; }
    PARAMETRIZE { suppressPlus = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); HP(400); MaxHP(400); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_NEGATIVE_CHARGE); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_PLUSLE) { Speed(40); Ability(ABILITY_PLUS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ALAKAZAM) { Speed(60); Moves(suppressPlus ? MOVE_GASTRO_ACID : MOVE_CELEBRATE, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, suppressPlus ? MOVE_GASTRO_ACID : MOVE_CELEBRATE, target: playerRight); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_PSYCHIC, target: playerLeft); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(playerLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(0.8), results[0].damage);
    }
}
