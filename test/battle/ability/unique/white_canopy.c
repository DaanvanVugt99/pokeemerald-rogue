#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("White Canopy summons snow and sets Grassy Terrain on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_WHITE_CANOPY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WHITE_CANOPY);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
        MESSAGE("Grass grew to cover the battlefield!");
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_SNOW);
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}
