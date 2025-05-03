#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Color Change counters Flamethrower")
{
    GIVEN
    {
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_COLOR_CHANGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_FLAMETHROWER); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used");
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_COLOR_CHANGE);
        MESSAGE("Kecleon transformed into the");
    }
    THEN
    {
        EXPECT_EQ(player->type1, TYPE_ROCK);
        EXPECT_EQ(player->type2, TYPE_ROCK);
    }
}

SINGLE_BATTLE_TEST("Color Change counters Thunderbolt")
{
    GIVEN
    {
        PLAYER(SPECIES_KECLEON) { Ability(ABILITY_COLOR_CHANGE); }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_THUNDERBOLT); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used");
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_COLOR_CHANGE);
        MESSAGE("Kecleon transformed into the");
    }
    THEN
    {
        EXPECT_EQ(player->type1, TYPE_ROCK);
        EXPECT_EQ(player->type2, TYPE_ROCK);
    }
}

SINGLE_BATTLE_TEST("Color Change does not activate on status moves")
{
    ASSUME(gBattleMoves[MOVE_TOXIC].power == 0); // Status move
    ASSUME(gBattleMoves[MOVE_TOXIC].type == TYPE_POISON);

    GIVEN
    {
        PLAYER(SPECIES_KECLEON)
        {
            Ability(ABILITY_COLOR_CHANGE);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_TOXIC); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used Toxic!");
        NONE_OF
        {
            ABILITY_POPUP(player, ABILITY_COLOR_CHANGE);
            MESSAGE("Kecleon transformed into the");
        }
    }
    THEN
    {
        EXPECT_EQ(player->type1, TYPE_NORMAL);
        EXPECT_EQ(player->type2, TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Color Change does not activate if the user is already that type")
{
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_KECLEON].types[0] == TYPE_NORMAL);
    ASSUME(gSpeciesInfo[SPECIES_KECLEON].types[1] == TYPE_NORMAL);

    GIVEN
    {
        PLAYER(SPECIES_KECLEON)
        {
            Ability(ABILITY_COLOR_CHANGE);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used Tackle!");
        HP_BAR(player);
        NONE_OF
        {
            ABILITY_POPUP(player, ABILITY_COLOR_CHANGE);
            MESSAGE("Kecleon transformed into the Normal type!");
        }
    }
    THEN
    {
        EXPECT_EQ(player->type1, TYPE_NORMAL);
        EXPECT_EQ(player->type2, TYPE_NORMAL);
    }
}

SINGLE_BATTLE_TEST("Color Change does not activate if the move has no effect")
{
    ASSUME(gSpeciesInfo[SPECIES_DIGLETT].types[0] == TYPE_GROUND);
    ASSUME(gSpeciesInfo[SPECIES_DIGLETT].types[1] == TYPE_GROUND);
    GIVEN
    {
        PLAYER(SPECIES_DIGLETT)
        {
            Ability(ABILITY_COLOR_CHANGE);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(opponent, MOVE_THUNDERBOLT); } // Electric move
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet used Thunderbolt!");
        MESSAGE("It doesn't affect Kecleon...");
        NONE_OF
        {
            ABILITY_POPUP(player, ABILITY_COLOR_CHANGE);
            MESSAGE("Kecleon transformed into the");
        }
    }
    THEN
    {
        EXPECT_EQ(player->type1, TYPE_GROUND);
        EXPECT_EQ(player->type2, TYPE_GROUND);
    }
}
