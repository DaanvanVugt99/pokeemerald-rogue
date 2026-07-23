#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RAGING_BULL].power > 0);
}

SINGLE_BATTLE_TEST("Horn Lock gives every Paldean Tauros breed the target's secondary type")
{
    u16 species;
    u8 expectedTargetSecondaryType;

    PARAMETRIZE { species = SPECIES_TAUROS_PALDEAN_COMBAT_BREED; expectedTargetSecondaryType = TYPE_WATER; }
    PARAMETRIZE { species = SPECIES_TAUROS_PALDEAN_BLAZE_BREED; expectedTargetSecondaryType = TYPE_FIRE; }
    PARAMETRIZE { species = SPECIES_TAUROS_PALDEAN_AQUA_BREED; expectedTargetSecondaryType = TYPE_WATER; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_INTIMIDATE); Moves(MOVE_RAGING_BULL); }
        OPPONENT(SPECIES_SWAMPERT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAGING_BULL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_FIGHTING);
        EXPECT_EQ(player->type2, TYPE_GROUND);
        EXPECT_EQ(opponent->type1, TYPE_WATER);
        EXPECT_EQ(opponent->type2, expectedTargetSecondaryType);
    }
}

SINGLE_BATTLE_TEST("Horn Lock swaps the changed secondary types again on repeated Raging Bull hits")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); UniqueAbility(ABILITY_HORN_LOCK); Moves(MOVE_RAGING_BULL); }
        OPPONENT(SPECIES_SWAMPERT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAGING_BULL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RAGING_BULL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HORN_LOCK);
        ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
        EXPECT_EQ(opponent->type1, TYPE_WATER);
        EXPECT_EQ(opponent->type2, TYPE_GROUND);
    }
}

SINGLE_BATTLE_TEST("Horn Lock swaps secondary types after a Raging Bull knockout")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); UniqueAbility(ABILITY_HORN_LOCK); Moves(MOVE_RAGING_BULL); }
        OPPONENT(SPECIES_SWAMPERT) { HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_RAGING_BULL); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_GROUND);
    }
}

SINGLE_BATTLE_TEST("Horn Lock type changes end when the user switches out")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); UniqueAbility(ABILITY_HORN_LOCK); Moves(MOVE_RAGING_BULL); }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SWAMPERT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAGING_BULL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
    }
}

SINGLE_BATTLE_TEST("Horn Lock does not swap secondary types through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Speed(1); UniqueAbility(ABILITY_HORN_LOCK); Moves(MOVE_RAGING_BULL); }
        OPPONENT(SPECIES_SWAMPERT) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_SUBSTITUTE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_RAGING_BULL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
        EXPECT_EQ(opponent->type1, TYPE_WATER);
        EXPECT_EQ(opponent->type2, TYPE_GROUND);
    }
}

SINGLE_BATTLE_TEST("Horn Lock does not swap secondary types while its user is Terastallized")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_HORN_LOCK); TeraType(TYPE_PSYCHIC); Moves(MOVE_RAGING_BULL); }
        OPPONENT(SPECIES_SWAMPERT) { HP(500); MaxHP(500); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RAGING_BULL, tera: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
        EXPECT_EQ(opponent->type1, TYPE_WATER);
        EXPECT_EQ(opponent->type2, TYPE_GROUND);
    }
}

SINGLE_BATTLE_TEST("Horn Lock does not swap a Terastallized target's secondary type")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(1); Speed(1); UniqueAbility(ABILITY_HORN_LOCK); Moves(MOVE_RAGING_BULL); }
        OPPONENT(SPECIES_SWAMPERT) { HP(500); MaxHP(500); Speed(100); TeraType(TYPE_WATER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE, tera: TRUE); MOVE(player, MOVE_RAGING_BULL); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_HORN_LOCK);
    } THEN {
        EXPECT_EQ(player->type1, TYPE_PSYCHIC);
        EXPECT_EQ(player->type2, TYPE_PSYCHIC);
        EXPECT_EQ(opponent->type1, TYPE_WATER);
        EXPECT_EQ(opponent->type2, TYPE_GROUND);
    }
}
