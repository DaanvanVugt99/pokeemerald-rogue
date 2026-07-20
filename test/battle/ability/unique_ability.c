#include "global.h"
#include "constants/rogue.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
}

#define TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY 2

static u32 DynamicSilverLiningCustomMonId(void)
{
    return OTID_FLAG_CUSTOM_MON
        | OTID_FLAG_DYNAMIC_CUSTOM_MON
        | (ABILITY_SILVER_LINING << 14)
        | (TEST_FORMAT_ORIGINAL_UNIQUE_ABILITY << 28);
}

SINGLE_BATTLE_TEST("Unique abilities can affect battle behavior")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT) { UniqueAbility(ABILITY_LEVITATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); }
        TURN { SWITCH(opponent, 1); }
    } SCENE {
        MESSAGE("2 sent out Wynaut!");
        NOT MESSAGE("Foe Wynaut is hurt by spikes!");
    }
}

SINGLE_BATTLE_TEST("Unique abilities are suppressed by Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WYNAUT) { UniqueAbility(ABILITY_LEVITATE); }
        OPPONENT(SPECIES_KOFFING) { Ability(ABILITY_NEUTRALIZING_GAS); Moves(MOVE_SPIKES, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SPIKES); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        s32 maxHP = GetMonData(&PLAYER_PARTY[1], MON_DATA_MAX_HP);

        HP_BAR(player, damage: maxHP / 8);
        MESSAGE("Wynaut is hurt by spikes!");
    }
}

SINGLE_BATTLE_TEST("Gastro Acid suppresses unique abilities")
{
    GIVEN {
        ASSUME(gBattleMoves[MOVE_GASTRO_ACID].effect == EFFECT_GASTRO_ACID);
        PLAYER(SPECIES_WOBBUFFET) { UniqueAbility(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GASTRO_ACID); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GASTRO_ACID); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Dynamic custom unique abilities can affect battle behavior")
{
    u32 customMonId = DynamicSilverLiningCustomMonId();

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) {
            CreateMon(gBattleTestRunnerState->data.currentMon, SPECIES_WOBBUFFET, 100, 0, TRUE, 0, OT_ID_CUSTOM_MON, customMonId);
            Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_TOXIC); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TOXIC); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SILVER_LINING);
        MESSAGE("Wobbuffet's Silver Lining prevents poisoning!");
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpeciesAndOtId(player->species, player->otId), ABILITY_SILVER_LINING);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}
