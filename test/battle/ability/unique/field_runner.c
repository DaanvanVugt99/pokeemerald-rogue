#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
    ASSUME(gBattleMoves[MOVE_SCARY_FACE].effect == EFFECT_SPEED_DOWN_2);
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
}

SINGLE_BATTLE_TEST("Field Runner sets Plain Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_STOUTLAND) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_FIELD_RUNNER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FIELD_RUNNER);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Field Runner prevents Speed drops in Plain Terrain")
{
    GIVEN {
        PLAYER(SPECIES_STOUTLAND) { Speed(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_FIELD_RUNNER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SCARY_FACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCARY_FACE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FIELD_RUNNER);
        ABILITY_POPUP(player, ABILITY_FIELD_RUNNER);
        MESSAGE("Stoutland's Field Runner prevents stat loss!");
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Field Runner only prevents Speed drops")
{
    GIVEN {
        PLAYER(SPECIES_STOUTLAND) { Speed(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_FIELD_RUNNER); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GROWL); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GROWL); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Field Runner is the Stoutland line's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_LILLIPUP), ABILITY_FIELD_RUNNER);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_HERDIER), ABILITY_FIELD_RUNNER);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_STOUTLAND), ABILITY_FIELD_RUNNER);
    }
}
