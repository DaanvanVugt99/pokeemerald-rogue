#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ETERNABEAM].effect == EFFECT_RECHARGE);
    ASSUME(gBattleMoves[MOVE_COSMIC_POWER].split == SPLIT_STATUS);
}

SINGLE_BATTLE_TEST("Endless Core allows Eternatus to use a status move while recharging")
{
    GIVEN {
        PLAYER(SPECIES_ETERNATUS) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ENDLESS_CORE); Moves(MOVE_ETERNABEAM, MOVE_COSMIC_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ETERNABEAM); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_COSMIC_POWER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ETERNABEAM, player);
        ABILITY_POPUP(player, ABILITY_ENDLESS_CORE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_COSMIC_POWER, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(player->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(!(player->status2 & STATUS2_RECHARGE));
    }
}

SINGLE_BATTLE_TEST("Endless Core limits recharge-turn choices to status moves")
{
    GIVEN {
        PLAYER(SPECIES_ETERNATUS) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_ENDLESS_CORE); Moves(MOVE_ETERNABEAM, MOVE_DRAGON_PULSE, MOVE_COSMIC_POWER); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ETERNABEAM); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ETERNABEAM, player);
    } THEN {
        u32 battler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u8 unusableMoves = CheckMoveLimitations(battler, 0, MOVE_LIMITATIONS_ALL);

        EXPECT(player->status2 & STATUS2_RECHARGE);
        EXPECT(unusableMoves & gBitTable[1]);
        EXPECT(!(unusableMoves & gBitTable[2]));
    }
}
