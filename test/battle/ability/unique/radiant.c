#include "global.h"
#include "constants/rogue.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_HEAT_WAVE].type == TYPE_FIRE);
}

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

SINGLE_BATTLE_TEST("Radiant sets sun after a successful Fire-type move when the party shares a type")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_LARVESTA) { Speed(90); Ability(ABILITY_FLAME_BODY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RADIANT);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
        EXPECT(!(gBattleWeather & B_WEATHER_SUN_PERMANENT));
    }
}

SINGLE_BATTLE_TEST("Radiant counts a teammate's dynamic custom typing when checking shared party type")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_SQUIRTLE) { Speed(90); Ability(ABILITY_TORRENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }

        CreateDynamicTypeMon(&PLAYER_PARTY[1], SPECIES_SQUIRTLE, TYPE_FIRE, 0);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_RADIANT);
    } THEN {
        EXPECT_EQ(GetTypeBySpecies(SPECIES_SQUIRTLE, 0, GetMonData(&PLAYER_PARTY[1], MON_DATA_OT_ID)), TYPE_FIRE);
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}

SINGLE_BATTLE_TEST("Radiant does not set sun if any party member does not share a type")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_EMBER); }
        PLAYER(SPECIES_SQUIRTLE) { Speed(90); Ability(ABILITY_TORRENT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_SPLASH); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_SPLASH); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_RADIANT);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_SUN));
    }
}

DOUBLE_BATTLE_TEST("Radiant still sets sun if a spread Fire move hits at least one target")
{
    GIVEN {
        PLAYER(SPECIES_VOLCARONA) { Speed(100); Ability(ABILITY_FLAME_BODY); UniqueAbility(ABILITY_RADIANT); Moves(MOVE_HEAT_WAVE); }
        PLAYER(SPECIES_LARVESTA) { Speed(90); Ability(ABILITY_FLAME_BODY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_SPLASH); }
        OPPONENT(SPECIES_WYNAUT) { Speed(110); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_HEAT_WAVE); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_SPLASH); MOVE(opponentRight, MOVE_PROTECT); }
    } SCENE {
        ABILITY_POPUP(playerLeft, ABILITY_RADIANT);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SUN);
    }
}
