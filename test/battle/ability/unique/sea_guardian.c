#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_SURF].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_ICE_BEAM].type == TYPE_ICE);
    ASSUME(gBattleMoves[MOVE_FLASH_CANNON].type == TYPE_STEEL);
}

SINGLE_BATTLE_TEST("Sea Guardian shows the normal not very effective message when it grants Water's Fire resistance")
{
    bool32 validParty;

    PARAMETRIZE { validParty = FALSE; }
    PARAMETRIZE { validParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_LUGIA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SEA_GUARDIAN); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(validParty ? SPECIES_SLOWBRO : SPECIES_PIKACHU) { Ability(validParty ? ABILITY_OWN_TEMPO : ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FLAMETHROWER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAMETHROWER, opponent);
        HP_BAR(player);
        if (validParty)
            MESSAGE("It's not very effective…");
        else {
            NONE_OF {
                MESSAGE("It's super effective!");
                MESSAGE("It's not very effective…");
            }
        }
    }
}

SINGLE_BATTLE_TEST("Sea Guardian grants Water's Fire resistance when Lugia's party shares a type", s16 damage)
{
    bool32 validParty;

    PARAMETRIZE { validParty = FALSE; }
    PARAMETRIZE { validParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_LUGIA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SEA_GUARDIAN); Moves(MOVE_CELEBRATE); HP(500); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(validParty ? SPECIES_SLOWBRO : SPECIES_PIKACHU) { Ability(validParty ? ABILITY_OWN_TEMPO : ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FLAMETHROWER); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FLAMETHROWER); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Sea Guardian grants Water's Water resistance when Lugia's party shares a type", s16 damage)
{
    bool32 validParty;

    PARAMETRIZE { validParty = FALSE; }
    PARAMETRIZE { validParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_LUGIA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SEA_GUARDIAN); Moves(MOVE_CELEBRATE); HP(500); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(validParty ? SPECIES_SLOWBRO : SPECIES_PIKACHU) { Ability(validParty ? ABILITY_OWN_TEMPO : ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SURF); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SURF); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Sea Guardian grants Water's Ice resistance when Lugia's party shares a type", s16 damage)
{
    bool32 validParty;

    PARAMETRIZE { validParty = FALSE; }
    PARAMETRIZE { validParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_LUGIA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SEA_GUARDIAN); Moves(MOVE_CELEBRATE); HP(500); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(validParty ? SPECIES_SLOWBRO : SPECIES_PIKACHU) { Ability(validParty ? ABILITY_OWN_TEMPO : ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_ICE_BEAM); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ICE_BEAM); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Sea Guardian grants Water's Steel resistance when Lugia's party shares a type", s16 damage)
{
    bool32 validParty;

    PARAMETRIZE { validParty = FALSE; }
    PARAMETRIZE { validParty = TRUE; }

    GIVEN {
        PLAYER(SPECIES_LUGIA) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_SEA_GUARDIAN); Moves(MOVE_CELEBRATE); HP(500); }
        PLAYER(SPECIES_XATU) { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(validParty ? SPECIES_SLOWBRO : SPECIES_PIKACHU) { Ability(validParty ? ABILITY_OWN_TEMPO : ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FLASH_CANNON); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FLASH_CANNON); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(2.0), results[0].damage);
    }
}
