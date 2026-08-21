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

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
    ASSUME(gBattleMoves[MOVE_FOCUS_ENERGY].effect == EFFECT_FOCUS_ENERGY);
    ASSUME(gItems[ITEM_INFESTED_SEED].holdEffect == HOLD_EFFECT_SEEDS);
    ASSUME(gItems[ITEM_INFESTED_SEED].holdEffectParam == HOLD_EFFECT_PARAM_INFESTED_TERRAIN);
    ASSUME(gBattleMoves[MOVE_BATON_PASS].effect == EFFECT_BATON_PASS);
    ASSUME(gSpeciesInfo[SPECIES_CATERPIE].types[0] == TYPE_BUG || gSpeciesInfo[SPECIES_CATERPIE].types[1] == TYPE_BUG);
    ASSUME(gSpeciesInfo[SPECIES_SCYTHER].types[0] == TYPE_BUG || gSpeciesInfo[SPECIES_SCYTHER].types[1] == TYPE_BUG);
    ASSUME(gSpeciesInfo[SPECIES_RATICATE].types[0] != TYPE_BUG && gSpeciesInfo[SPECIES_RATICATE].types[1] != TYPE_BUG);
}

SINGLE_BATTLE_TEST("Infested Terrain activates Infested Seed and raises critical-hit ratio")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_INFESTED_SEED); Moves(MOVE_INFESTED_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Wobbuffet used Infested Seed to get pumped!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(player->status2 & STATUS2_FOCUS_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Infested Seed is not consumed if Focus Energy is already active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_INFESTED_SEED); Moves(MOVE_FOCUS_ENERGY, MOVE_INFESTED_TERRAIN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_FOCUS_ENERGY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            MESSAGE("Wobbuffet used Infested Seed to get pumped!");
        }
    } THEN {
        EXPECT_EQ(player->item, ITEM_INFESTED_SEED);
        EXPECT(player->status2 & STATUS2_FOCUS_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Infested Seed activates when its holder switches into Infested Terrain")
{
    GIVEN {
        PLAYER(SPECIES_CATERPIE) { Moves(MOVE_INFESTED_TERRAIN); }
        PLAYER(SPECIES_SCYTHER) { Item(ITEM_INFESTED_SEED); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_HELD_ITEM_EFFECT, player);
        MESSAGE("Scyther used Infested Seed to get pumped!");
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT(player->status2 & STATUS2_FOCUS_ENERGY);
    }
}

SINGLE_BATTLE_TEST("Infested Terrain damages switch ins based on Bug typing")
{
    u32 species, divisor;

    PARAMETRIZE { species = SPECIES_EXEGGUTOR; divisor = 2; }
    PARAMETRIZE { species = SPECIES_RATICATE; divisor = 8; }
    PARAMETRIZE { species = SPECIES_ALAKAZAM; divisor = 4; }
    PARAMETRIZE { species = SPECIES_LUCARIO; divisor = 32; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE);
        OPPONENT(species);
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        s32 maxHP = GetMonData(&OPPONENT_PARTY[1], MON_DATA_MAX_HP);
        if (species == SPECIES_EXEGGUTOR)
            MESSAGE("2 sent out Exeggutor!");
        else if (species == SPECIES_RATICATE)
            MESSAGE("2 sent out Raticate!");
        else if (species == SPECIES_ALAKAZAM)
            MESSAGE("2 sent out Alakazam!");
        else if (species == SPECIES_LUCARIO)
            MESSAGE("2 sent out Lucario!");
        HP_BAR(opponent, damage: maxHP / divisor);
        if (species == SPECIES_EXEGGUTOR)
            MESSAGE("Foe Exeggutor is swarmed\nby the infested terrain!");
        else if (species == SPECIES_RATICATE)
            MESSAGE("Foe Raticate is swarmed\nby the infested terrain!");
        else if (species == SPECIES_ALAKAZAM)
            MESSAGE("Foe Alakazam is swarmed\nby the infested terrain!");
        else if (species == SPECIES_LUCARIO)
            MESSAGE("Foe Lucario is swarmed\nby the infested terrain!");
    }
}

SINGLE_BATTLE_TEST("Infested Terrain only damages grounded switch ins")
{
    u32 item = ITEM_NONE;
    bool32 grounded;

    PARAMETRIZE { grounded = FALSE; }
    PARAMETRIZE { item = ITEM_IRON_BALL; grounded = TRUE; }

    GIVEN {
        ASSUME(gSpeciesInfo[SPECIES_ZUBAT].types[0] != TYPE_BUG && gSpeciesInfo[SPECIES_ZUBAT].types[1] != TYPE_BUG);
        ASSUME(gSpeciesInfo[SPECIES_ZUBAT].types[0] == TYPE_FLYING || gSpeciesInfo[SPECIES_ZUBAT].types[1] == TYPE_FLYING);
        ASSUME(gItems[ITEM_IRON_BALL].holdEffect == HOLD_EFFECT_IRON_BALL);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE);
        OPPONENT(SPECIES_ZUBAT) { Item(item); }
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        if (grounded)
            HP_BAR(opponent);
        else
            NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Infested Terrain does not damage Bug-type switch ins")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE);
        OPPONENT(SPECIES_SCYTHER);
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Infested Terrain uses dynamic typing for switch ins")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_CATERPIE);
        OPPONENT(SPECIES_RATICATE);

        CreateDynamicTypeMon(&OPPONENT_PARTY[1], SPECIES_RATICATE, TYPE_BUG, 0);
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        NOT HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->type1, TYPE_BUG);
    }
}

SINGLE_BATTLE_TEST("Infested Terrain damages subsequent switch ins based on Bug typing")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_RATICATE);
        OPPONENT(SPECIES_SCYTHER);
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } SCENE {
        s32 maxHP0 = GetMonData(&OPPONENT_PARTY[0], MON_DATA_MAX_HP);
        MESSAGE("2 sent out Scyther!");
        MESSAGE("2 sent out Raticate!");
        HP_BAR(opponent, damage: maxHP0 / 8);
        MESSAGE("Foe Raticate is swarmed\nby the infested terrain!");
    }
}

SINGLE_BATTLE_TEST("Infested Terrain does not damage Bug-type subsequent switch ins")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_SCYTHER);
        OPPONENT(SPECIES_CATERPIE);
    } WHEN {
        TURN { MOVE(player, MOVE_INFESTED_TERRAIN); }
        TURN { SWITCH(opponent, 1); }
        TURN { SWITCH(opponent, 0); }
    } SCENE {
        MESSAGE("2 sent out Caterpie!");
        MESSAGE("2 sent out Scyther!");
    } THEN {
        s32 maxHP = GetMonData(&OPPONENT_PARTY[0], MON_DATA_MAX_HP);
        s32 hp = GetMonData(&OPPONENT_PARTY[0], MON_DATA_HP);
        EXPECT_EQ(hp, maxHP);
    }
}
