#include "global.h"
#include "constants/rogue.h"
#include "pokemon.h"
#include "test/battle.h"

static u32 DynamicTypeCustomMonId(u32 type, u32 typeSlot)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | (1 << 28) // COMPRESSED_FORMAT_MON_TYPE
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

SINGLE_BATTLE_TEST("Dynamic custom typing is retained when a Pokemon switches in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_CRABOMINABLE) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }

        CreateDynamicTypeMon(&PLAYER_PARTY[1], SPECIES_CRABOMINABLE, TYPE_GROUND, 1);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->type1, TYPE_FIGHTING);
        EXPECT_EQ(player->type2, TYPE_GROUND);
    }
}

SINGLE_BATTLE_TEST("A transformed dynamic custom Pokemon restores the target species typing after Mimicry")
{
    u16 turns;

    GIVEN {
        ASSUME(gBattleMoves[MOVE_TRANSFORM].effect == EFFECT_TRANSFORM);
        ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);

        PLAYER(SPECIES_DITTO) { Speed(100); }
        OPPONENT(SPECIES_STUNFISK_GALARIAN) { Speed(50); Ability(ABILITY_MIMICRY); Moves(MOVE_PLAIN_TERRAIN, MOVE_CELEBRATE, MOVE_SPLASH); }

        CreateDynamicTypeMon(&PLAYER_PARTY[0], SPECIES_DITTO, TYPE_FIRE, 0);
    } WHEN {
        TURN { MOVE(player, MOVE_TRANSFORM); MOVE(opponent, MOVE_PLAIN_TERRAIN); }
        for (turns = 0; turns < 5; ++turns)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        for (; turns < TERRAIN_DURATION_TURNS - 1; ++turns)
            TURN { MOVE(player, MOVE_SPLASH); MOVE(opponent, MOVE_SPLASH); }
    } THEN {
        EXPECT(player->status2 & STATUS2_TRANSFORMED);
        EXPECT_EQ(player->species, SPECIES_STUNFISK_GALARIAN);
        EXPECT_EQ(player->type1, gSpeciesInfo[SPECIES_STUNFISK_GALARIAN].types[0]);
        EXPECT_EQ(player->type2, gSpeciesInfo[SPECIES_STUNFISK_GALARIAN].types[1]);
    }
}
