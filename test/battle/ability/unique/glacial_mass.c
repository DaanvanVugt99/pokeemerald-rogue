#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
    ASSUME(gBattleMoves[MOVE_DOUBLE_SLAP].effect == EFFECT_MULTI_HIT);
    ASSUME(gBattleMoves[MOVE_DOUBLE_SLAP].makesContact);
}

SINGLE_BATTLE_TEST("Glacial Mass triggers once after a multi-hit contact move")
{
    GIVEN {
        PLAYER(SPECIES_DEWGONG) { Ability(ABILITY_THICK_FAT); UniqueAbility(ABILITY_GLACIAL_MASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SKILL_LINK); Moves(MOVE_DOUBLE_SLAP); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DOUBLE_SLAP); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Glacial Mass does not trigger on non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_DEWGONG) { Ability(ABILITY_THICK_FAT); UniqueAbility(ABILITY_GLACIAL_MASS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Glacial Mass does not make Knock Off remove the attacker's item")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_KNOCK_OFF].effect == EFFECT_KNOCK_OFF);
        ASSUME(gBattleMoves[MOVE_KNOCK_OFF].makesContact);
        PLAYER(SPECIES_WOBBUFFET) { Item(ITEM_LEFTOVERS); Moves(MOVE_KNOCK_OFF); }
        OPPONENT(SPECIES_DEWGONG) { Ability(ABILITY_THICK_FAT); UniqueAbility(ABILITY_GLACIAL_MASS); Item(ITEM_SITRUS_BERRY); }
    } WHEN {
        TURN { MOVE(player, MOVE_KNOCK_OFF); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_LEFTOVERS);
        EXPECT_EQ(opponent->item, ITEM_NONE);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE - 1);
    }
}
