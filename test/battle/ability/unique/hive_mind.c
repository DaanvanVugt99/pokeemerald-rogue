#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ZAP_CANNON].accuracy == 50);
    ASSUME(gBattleMoves[MOVE_PSYCHIC].type == TYPE_PSYCHIC);
    ASSUME(gBattleMoves[MOVE_PSYCHIC].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Hive Mind makes moves always hit if all Pokémon on the team share a type with the user")
{
    PASSES_RANDOMLY(100, 100, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_HIVE_MIND); Moves(MOVE_ZAP_CANNON); }
        PLAYER(SPECIES_KADABRA)   { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_MAGNETON)  { Ability(ABILITY_STURDY); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DOUBLE_TEAM); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DOUBLE_TEAM); MOVE(player, MOVE_ZAP_CANNON); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_TEAM, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ZAP_CANNON, player);
        NONE_OF {
            MESSAGE("Metagross's attack missed!");
        }
    }
}

SINGLE_BATTLE_TEST("Hive Mind only ignores defensive stat boosts if all Pokémon on the team share a type", s16 damage)
{
    bool32 sharedParty;

    PARAMETRIZE { sharedParty = FALSE; }
    PARAMETRIZE { sharedParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_METAGROSS) { Ability(ABILITY_CLEAR_BODY); UniqueAbility(ABILITY_HIVE_MIND); Moves(MOVE_CELEBRATE, MOVE_PSYCHIC); }
        PLAYER(SPECIES_KADABRA)   { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(sharedParty ? SPECIES_MAGNETON : SPECIES_PIKACHU) { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_AMNESIA, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AMNESIA); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage);
    }
}

