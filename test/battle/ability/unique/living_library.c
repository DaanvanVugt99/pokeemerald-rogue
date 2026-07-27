#include "global.h"
#include "test/battle.h"

static bool32 HandContainsMove(u32 battler, u16 move)
{
    u32 i;

    for (i = 0; i < MAX_MON_MOVES; i++)
    {
        if (gBattleMons[battler].moves[i] == move)
            return TRUE;
    }

    return FALSE;
}

SINGLE_BATTLE_TEST("Living Library draws four distinct party moves and excludes technical moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT, MOVE_SKETCH); }
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_LIVING_LIBRARY);
            Moves(MOVE_PROTECT, MOVE_TACKLE);
        }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_PROTECT, MOVE_EMBER, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVING_LIBRARY);
        ABILITY_POPUP(player, ABILITY_LIVING_LIBRARY);
    } THEN {
        u32 i, j;

        EXPECT(HandContainsMove(B_POSITION_PLAYER_LEFT, MOVE_PROTECT));
        EXPECT(HandContainsMove(B_POSITION_PLAYER_LEFT, MOVE_TACKLE));
        EXPECT(HandContainsMove(B_POSITION_PLAYER_LEFT, MOVE_EMBER));
        EXPECT(HandContainsMove(B_POSITION_PLAYER_LEFT, MOVE_WATER_GUN));
        EXPECT(!HandContainsMove(B_POSITION_PLAYER_LEFT, MOVE_SKETCH));
        for (i = 0; i < MAX_MON_MOVES; i++)
        {
            EXPECT_NE(gBattleMons[B_POSITION_PLAYER_LEFT].moves[i], MOVE_NONE);
            for (j = i + 1; j < MAX_MON_MOVES; j++)
                EXPECT_NE(gBattleMons[B_POSITION_PLAYER_LEFT].moves[i], gBattleMons[B_POSITION_PLAYER_LEFT].moves[j]);
        }
    }
}

SINGLE_BATTLE_TEST("Living Library can use borrowed moves without changing the party moveset or PP")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SKETCH); }
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_LIVING_LIBRARY);
            Moves(MOVE_SKETCH);
        }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, moveSlot: 0); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Mew used Ember!");
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE1), MOVE_SKETCH);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE2), MOVE_NONE);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE3), MOVE_NONE);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE4), MOVE_NONE);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_PP1), gBattleMoves[MOVE_SKETCH].pp);
    }
}

SINGLE_BATTLE_TEST("Living Library leaves empty slots when the party knows fewer than four legal moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_SKETCH); }
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_LIVING_LIBRARY);
            Moves(MOVE_SPLASH);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[0], MOVE_SPLASH);
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[1], MOVE_NONE);
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[2], MOVE_NONE);
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[3], MOVE_NONE);
    }
}

SINGLE_BATTLE_TEST("Living Library restores the original moves while suppressed")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_LIVING_LIBRARY);
            Moves(MOVE_SPLASH);
        }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_EMBER, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) {
            Moves(MOVE_GASTRO_ACID, MOVE_CELEBRATE);
        }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_GASTRO_ACID); MOVE(player, MOVE_SPLASH); }
    } THEN {
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[0], MOVE_SPLASH);
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[1], MOVE_NONE);
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[2], MOVE_NONE);
        EXPECT_EQ(gBattleMons[B_POSITION_PLAYER_LEFT].moves[3], MOVE_NONE);
        EXPECT(!(gBattleResources->flags->flags[B_POSITION_PLAYER_LEFT] & RESOURCE_FLAG_LIVING_LIBRARY));
    }
}

SINGLE_BATTLE_TEST("Living Library keeps a Choice-locked move in later hands")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
        PLAYER(SPECIES_MEW) {
            Item(ITEM_CHOICE_SCARF);
            UniqueAbility(ABILITY_LIVING_LIBRARY);
            Moves(MOVE_TACKLE, MOVE_EMBER);
        }
        PLAYER(SPECIES_BULBASAUR) { Moves(MOVE_WATER_GUN, MOVE_LEAFAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        gBattleStruct->choicedMove[B_POSITION_PLAYER_LEFT] = MOVE_EMBER;
        EXPECT(TryDealLivingLibraryHand(B_POSITION_PLAYER_LEFT));
        EXPECT(HandContainsMove(B_POSITION_PLAYER_LEFT, MOVE_EMBER));
    }
}
