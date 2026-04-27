#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sand Onslaught raises Speed after dealing damage on a shared-type team")
{
    GIVEN {
        PLAYER(SPECIES_GARCHOMP) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SAND_ONSLAUGHT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_GABITE);
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SAND_ONSLAUGHT);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Sand Onslaught does not raise Speed if a teammate shares no type with the user")
{
    GIVEN {
        PLAYER(SPECIES_GARCHOMP) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SAND_ONSLAUGHT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_GABITE);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SAND_ONSLAUGHT);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sand Onslaught does not raise Speed when no damage is dealt")
{
    GIVEN {
        PLAYER(SPECIES_GARCHOMP) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SAND_ONSLAUGHT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_GABITE);
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SAND_ONSLAUGHT);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

DOUBLE_BATTLE_TEST("Sand Onslaught does not raise Speed after damaging only an ally")
{
    GIVEN {
        PLAYER(SPECIES_GARCHOMP) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SAND_ONSLAUGHT); Moves(MOVE_SURF); }
        PLAYER(SPECIES_GABITE) { HP(400); MaxHP(400); Ability(ABILITY_SAND_VEIL); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_SANDSHREW);
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_PROTECT);
            MOVE(opponentRight, MOVE_PROTECT);
            MOVE(playerLeft, MOVE_SURF);
            MOVE(playerRight, MOVE_CELEBRATE);
        }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_SAND_ONSLAUGHT);
        }
    } THEN {
        EXPECT_EQ(playerLeft->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT(playerRight->hp < playerRight->maxHP);
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_EQ(opponentRight->hp, opponentRight->maxHP);
    }
}
