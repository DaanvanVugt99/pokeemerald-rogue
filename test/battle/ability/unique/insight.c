#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PSYCHIC].type == TYPE_PSYCHIC);
    ASSUME(gBattleMoves[MOVE_PSYCHIC].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Psychic does not affect Dark-types without Insight")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_NONE); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_UMBREON);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Insight makes Psychic hit Dark-types")
{
    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Ability(ABILITY_TELEPATHY); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_UMBREON);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); }
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Insight ignores defensive stat boosts for Psychic moves in Psychic Terrain", s16 damage)
{
    bool32 raiseSpDef;

    PARAMETRIZE { raiseSpDef = FALSE; }
    PARAMETRIZE { raiseSpDef = TRUE; }

    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PSYCHIC_TERRAIN, MOVE_CELEBRATE, MOVE_AMNESIA); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, raiseSpDef ? MOVE_AMNESIA : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Insight does not ignore defensive stat boosts for Psychic moves outside Psychic Terrain", s16 damage)
{
    bool32 raiseSpDef;

    PARAMETRIZE { raiseSpDef = FALSE; }
    PARAMETRIZE { raiseSpDef = TRUE; }

    GIVEN {
        PLAYER(SPECIES_BEHEEYEM) { Ability(ABILITY_TELEPATHY); Moves(MOVE_CELEBRATE, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_AMNESIA, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, raiseSpDef ? MOVE_AMNESIA : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
