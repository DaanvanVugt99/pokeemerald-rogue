#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RUINATION].effect == EFFECT_SUPER_FANG);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
    ASSUME(gBattleMoves[MOVE_STRENGTH_SAP].effect == EFFECT_STRENGTH_SAP);
    ASSUME(gBattleMoves[MOVE_HONE_CLAWS].effect == EFFECT_ATTACK_ACCURACY_UP);
    ASSUME(gBattleMoves[MOVE_TRICK_ROOM].effect == EFFECT_TRICK_ROOM);
    ASSUME(gBattleMoves[MOVE_SUNNY_DAY].effect == EFFECT_SUNNY_DAY);
}

SINGLE_BATTLE_TEST("Treasure rites consume Eclipse and invoke their associated move")
{
    u32 species;
    u32 ability;
    u32 calledMove;

    PARAMETRIZE { species = SPECIES_WO_CHIEN;  ability = ABILITY_WITHERING_SCRIPT; calledMove = MOVE_STRENGTH_SAP; }
    PARAMETRIZE { species = SPECIES_CHIEN_PAO; ability = ABILITY_SEVERING_RITE;    calledMove = MOVE_HONE_CLAWS; }
    PARAMETRIZE { species = SPECIES_TING_LU;   ability = ABILITY_EARTHEN_SEAL;     calledMove = MOVE_TRICK_ROOM; }
    PARAMETRIZE { species = SPECIES_CHI_YU;    ability = ABILITY_CINDER_EDICT;     calledMove = MOVE_SUNNY_DAY; }

    GIVEN {
        PLAYER(species) { HP(100); MaxHP(300); Speed(1); Moves(MOVE_RUINATION); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Attack(100); Speed(100); Moves(MOVE_ECLIPSE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); MOVE(player, MOVE_RUINATION); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ECLIPSE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RUINATION, player);
        ABILITY_POPUP(player, ability);
        MESSAGE("The eclipse faded.");
        ANIMATION(ANIM_TYPE_MOVE, calledMove, player);
    } THEN {
        EXPECT(!(gBattleWeather & B_WEATHER_ECLIPSE));
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);

        if (ability == ABILITY_WITHERING_SCRIPT)
        {
            EXPECT_GT(player->hp, 100);
            EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        }
        else if (ability == ABILITY_SEVERING_RITE)
        {
            EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
            EXPECT_EQ(player->statStages[STAT_ACC], DEFAULT_STAT_STAGE + 1);
        }
        else if (ability == ABILITY_EARTHEN_SEAL)
        {
            EXPECT(gFieldStatuses & STATUS_FIELD_TRICK_ROOM);
        }
        else if (ability == ABILITY_CINDER_EDICT)
        {
            EXPECT(gBattleWeather & B_WEATHER_SUN);
        }
    }
}

SINGLE_BATTLE_TEST("Treasure rites do not invoke Ruination payoffs outside Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_WO_CHIEN) { HP(100); MaxHP(300); Moves(MOVE_RUINATION); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Attack(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RUINATION); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RUINATION, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WITHERING_SCRIPT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_STRENGTH_SAP, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 100);
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Treasure rites only consume Eclipse once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_CHI_YU) { Speed(1); Moves(MOVE_RUINATION, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(100); Moves(MOVE_ECLIPSE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); MOVE(player, MOVE_RUINATION); }
        TURN { MOVE(opponent, MOVE_ECLIPSE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_RUINATION); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_CINDER_EDICT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SUNNY_DAY, player);
        NOT ABILITY_POPUP(player, ABILITY_CINDER_EDICT);
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
    }
}

SINGLE_BATTLE_TEST("Treasure rites do not consume Eclipse when Ruination fails")
{
    GIVEN {
        PLAYER(SPECIES_TING_LU) { Speed(1); Moves(MOVE_RUINATION, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_ECLIPSE, MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_RUINATION); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_EARTHEN_SEAL);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TRICK_ROOM, player);
        }
    } THEN {
        EXPECT(gBattleWeather & B_WEATHER_ECLIPSE);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TRICK_ROOM));
    }
}
