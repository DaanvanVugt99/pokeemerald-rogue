#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].power > 0);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].copycatBanned);
}

SINGLE_BATTLE_TEST("Living Shadow copies the first damaging move Marshadow survives each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MARSHADOW) { HP(300); MaxHP(300); Speed(50); Ability(ABILITY_TECHNICIAN); UniqueAbility(ABILITY_LIVING_SHADOW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_LIVING_SHADOW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Living Shadow only copies a damaging move once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MARSHADOW) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_TECHNICIAN); UniqueAbility(ABILITY_LIVING_SHADOW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_LIVING_SHADOW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LIVING_SHADOW);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        }
    }
}

SINGLE_BATTLE_TEST("Living Shadow does not activate if Marshadow faints")
{
    GIVEN {
        PLAYER(SPECIES_MARSHADOW) { HP(1); MaxHP(300); Speed(50); Ability(ABILITY_TECHNICIAN); UniqueAbility(ABILITY_LIVING_SHADOW); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(300); MaxHP(300); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_LIVING_SHADOW);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        }
    }
}
