#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Counterpunch changes Wobbuffet to punching form and raises Attack after Counter or Mirror Coat succeeds")
{
    u16 counterMove;
    u16 triggerMove;

    PARAMETRIZE { counterMove = MOVE_COUNTER;     triggerMove = MOVE_TACKLE; }
    PARAMETRIZE { counterMove = MOVE_MIRROR_COAT; triggerMove = MOVE_POWER_GEM; }

    GIVEN {
        ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(ABILITY_COUNTERPUNCH); Moves(counterMove, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); Moves(triggerMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, counterMove); MOVE(opponent, triggerMove); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET_PUNCHING);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Counterpunch shows Counterpunch when changing Wobbuffet's form")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_COUNTERPUNCH); Moves(MOVE_COUNTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COUNTER); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_COUNTERPUNCH);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHADOW_TAG);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET_PUNCHING);
    }
}

SINGLE_BATTLE_TEST("Counterpunch changes punching Wobbuffet back after a successful punching move")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
        PLAYER(SPECIES_WOBBUFFET_PUNCHING) { UniqueAbility(ABILITY_COUNTERPUNCH); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Counterpunch does not trigger when Counter fails")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); UniqueAbility(ABILITY_COUNTERPUNCH); Moves(MOVE_COUNTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COUNTER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Counterpunch prevents Stance Change from changing Wobbuffet's form")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_STANCE_CHANGE); Moves(MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Counterpunch is safe on non-Wobbuffet holders")
{
    GIVEN {
        PLAYER(SPECIES_MACHOP) { Speed(1); UniqueAbility(ABILITY_COUNTERPUNCH); Moves(MOVE_COUNTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); HP(1000); MaxHP(1000); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_COUNTER); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_MACHOP);
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 1);
    }
}
