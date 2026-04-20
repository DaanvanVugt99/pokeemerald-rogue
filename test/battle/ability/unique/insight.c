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
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_INSIGHT); Moves(MOVE_PSYCHIC); }
        OPPONENT(SPECIES_UMBREON);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC); }
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Insight ignores defensive stat boosts for Psychic moves in Psychic Terrain", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_INSIGHT; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PSYCHIC_TERRAIN, MOVE_AMNESIA, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYCHIC_TERRAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Insight does not ignore defensive stat boosts for Psychic moves outside Psychic Terrain", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_INSIGHT; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE, MOVE_PSYCHIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_AMNESIA, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
