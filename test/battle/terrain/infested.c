#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
    ASSUME(gBattleMoves[MOVE_BATON_PASS].effect == EFFECT_BATON_PASS);
    ASSUME(gSpeciesInfo[SPECIES_CATERPIE].types[0] == TYPE_BUG || gSpeciesInfo[SPECIES_CATERPIE].types[1] == TYPE_BUG);
    ASSUME(gSpeciesInfo[SPECIES_SCYTHER].types[0] == TYPE_BUG || gSpeciesInfo[SPECIES_SCYTHER].types[1] == TYPE_BUG);
    ASSUME(gSpeciesInfo[SPECIES_RATICATE].types[0] != TYPE_BUG && gSpeciesInfo[SPECIES_RATICATE].types[1] != TYPE_BUG);
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
