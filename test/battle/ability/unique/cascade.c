#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].effect == EFFECT_ATTACK_DOWN);
    ASSUME(gBattleMoves[MOVE_TAIL_WHIP].effect == EFFECT_DEFENSE_DOWN);
}

static u32 CountLoweredMainStats(struct BattlePokemon *mon)
{
    u32 count = 0;

    if (mon->statStages[STAT_ATK] < DEFAULT_STAT_STAGE)
        count++;
    if (mon->statStages[STAT_DEF] < DEFAULT_STAT_STAGE)
        count++;
    if (mon->statStages[STAT_SPEED] < DEFAULT_STAT_STAGE)
        count++;
    if (mon->statStages[STAT_SPATK] < DEFAULT_STAT_STAGE)
        count++;
    if (mon->statStages[STAT_SPDEF] < DEFAULT_STAT_STAGE)
        count++;

    return count;
}

SINGLE_BATTLE_TEST("Cascade lowers a second random stat after lowering an opponent's stat")
{
    GIVEN {
        PLAYER(SPECIES_SPIRITOMB) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_CASCADE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        SCENE {
            MESSAGE("Foe Wobbuffet's Attack fell!");
            ABILITY_POPUP(player, ABILITY_CASCADE);
        }
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(CountLoweredMainStats(opponent), 2);
    }
}

SINGLE_BATTLE_TEST("Cascade does not trigger when no opposing stat was lowered")
{
    GIVEN {
        PLAYER(SPECIES_SPIRITOMB) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_CASCADE); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_KINGLER) { Ability(ABILITY_HYPER_CUTTER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(CountLoweredMainStats(opponent), 0);
    }
}

SINGLE_BATTLE_TEST("Cascade's follow-up does not bypass specific stat-loss immunity")
{
    GIVEN {
        PLAYER(SPECIES_SPIRITOMB) { Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_CASCADE); Moves(MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_KRABBY) { Ability(ABILITY_HYPER_CUTTER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
    }
}
