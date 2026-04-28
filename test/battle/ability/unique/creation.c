#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_THUNDER_WAVE));
    ASSUME(IS_MOVE_STATUS(MOVE_CHARGE));
    ASSUME(IS_MOVE_STATUS(MOVE_RECOVER));
    ASSUME(IS_MOVE_STATUS(MOVE_TOXIC));
    ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_CHARGE].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_RECOVER].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_TOXIC].type == TYPE_POISON);
}

SINGLE_BATTLE_TEST("Creation sets Electric Terrain on the first successful Electric status move if the party shares Arceus's Plate type")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_ZAP_PLATE); Moves(MOVE_THUNDER_WAVE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_ELECTRIKE) { Speed(40); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT_EQ(GetBattlerCreationType(B_POSITION_PLAYER_LEFT), TYPE_ELECTRIC);
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Creation requires the party to share Arceus's Plate type")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_ZAP_PLATE); Moves(MOVE_THUNDER_WAVE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_MAGIKARP) { Speed(40); Ability(ABILITY_SWIFT_SWIM); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT_EQ(GetBattlerCreationType(B_POSITION_PLAYER_LEFT), TYPE_ELECTRIC);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

SINGLE_BATTLE_TEST("Creation counts Plate-typed Arceus teammates for the party restriction")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_ZAP_PLATE); Moves(MOVE_THUNDER_WAVE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_ARCEUS) { Speed(40); Ability(ABILITY_MULTITYPE); Item(ITEM_ZAP_PLATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Creation uses Arceus teammate Plate type over stored form type")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_ZAP_PLATE); Moves(MOVE_THUNDER_WAVE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_ARCEUS_ELECTRIC) { Speed(40); Ability(ABILITY_MULTITYPE); Item(ITEM_TOXIC_PLATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    }
}

SINGLE_BATTLE_TEST("Creation does not consume its trigger on a nonmatching status move")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_ZAP_PLATE); HP(200); MaxHP(400); Moves(MOVE_RECOVER, MOVE_THUNDER_WAVE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_ELECTRIKE) { Speed(40); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        NOT ABILITY_POPUP(player, ABILITY_CREATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_WAVE, player);
        ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Creation only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_ZAP_PLATE); Moves(MOVE_THUNDER_WAVE, MOVE_CHARGE); }
        PLAYER(SPECIES_PIKACHU) { Speed(50); Ability(ABILITY_STATIC); }
        PLAYER(SPECIES_ELECTRIKE) { Speed(40); Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CHARGE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CREATION);
        NOT ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Creation sets Acid Rain from Poison Arceus's first Poison status move")
{
    GIVEN {
        PLAYER(SPECIES_ARCEUS) { Speed(100); Ability(ABILITY_MULTITYPE); UniqueAbility(ABILITY_CREATION); Item(ITEM_TOXIC_PLATE); Moves(MOVE_TOXIC); }
        PLAYER(SPECIES_EKANS) { Speed(50); Ability(ABILITY_INTIMIDATE); }
        PLAYER(SPECIES_GRIMER) { Speed(40); Ability(ABILITY_STENCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TOXIC); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CREATION);
    } THEN {
        EXPECT_EQ(GetBattlerCreationType(B_POSITION_PLAYER_LEFT), TYPE_POISON);
        EXPECT(gBattleWeather & B_WEATHER_ACID_RAIN);
    }
}

SINGLE_BATTLE_TEST("Creation is assigned to Arceus")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_ARCEUS_NORMAL), ABILITY_CREATION);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_ARCEUS_ELECTRIC), ABILITY_CREATION);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_ARCEUS_POISON), ABILITY_CREATION);
    }
}
