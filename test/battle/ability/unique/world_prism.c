#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TERA_STARSTORM].effect == EFFECT_TERA_STARSTORM);
}

SINGLE_BATTLE_TEST("World Prism can set terrain after Terapagos's first Tera Starstorm")
{
    GIVEN {
        PLAYER(SPECIES_TERAPAGOS_STELLAR) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TERA_STARSTORM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_STARSTORM, WITH_RNG(RNG_ROGUE_WORLD_PRISM, 0)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_STARSTORM, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_WORLD_PRISM);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT(!(gBattleWeather & B_WEATHER_ANY));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("World Prism can set weather after Terapagos's first Tera Starstorm")
{
    GIVEN {
        PLAYER(SPECIES_TERAPAGOS_STELLAR) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TERA_STARSTORM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_STARSTORM, WITH_RNG(RNG_ROGUE_WORLD_PRISM, 6)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_STARSTORM, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_WORLD_PRISM);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY));
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("World Prism only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_TERAPAGOS_STELLAR) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TERA_STARSTORM); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TERA_STARSTORM, WITH_RNG(RNG_ROGUE_WORLD_PRISM, 0)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TERA_STARSTORM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_STARSTORM, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_WORLD_PRISM);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TERA_STARSTORM, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_WORLD_PRISM);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_PLAIN_TERRAIN);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("World Prism does not trigger after other moves")
{
    GIVEN {
        PLAYER(SPECIES_TERAPAGOS_STELLAR) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_WORLD_PRISM);
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY));
        EXPECT(!(gBattleWeather & B_WEATHER_ANY));
        EXPECT(!(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]));
    }
}
