#include "global.h"
#include "constants/rogue.h"
#include "pokemon.h"
#include "test/battle.h"

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | (1 << 21) // COMPRESSED_FORMAT_MON_TYPE
        | (typeSlot << 5)
        | type;
}

static void CreateDynamicTypeMon(struct Pokemon *mon, u16 species, u32 type, u32 typeSlot)
{
    CreateMon(mon, species, 100, 0, TRUE, 0, OT_ID_CUSTOM_MON, DynamicTypeCustomMonId(type, typeSlot));
}

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

SINGLE_BATTLE_TEST("Adaptive Origin counts dynamic custom typing when checking unique party types", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_ADAPTIVE_ORIGIN; }
    PARAMETRIZE { ability = ABILITY_PRESSURE; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);

        PLAYER(SPECIES_MEW)         { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ability); Moves(MOVE_THUNDERBOLT); }
        PLAYER(SPECIES_CHARIZARD)   { Ability(ABILITY_BLAZE); }
        PLAYER(SPECIES_FARFETCHD)   { Ability(ABILITY_KEEN_EYE); } // Static Flying overlaps with Charizard.
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); }

        CreateDynamicTypeMon(&PLAYER_PARTY[2], SPECIES_FARFETCHD, TYPE_GROUND, 1);
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(GetTypeBySpecies(SPECIES_FARFETCHD, 1, GetMonData(&PLAYER_PARTY[2], MON_DATA_OT_ID)), TYPE_GROUND);
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}
