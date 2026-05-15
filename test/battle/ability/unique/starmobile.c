#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BLAZING_TORQUE].target == MOVE_TARGET_SELECTED);
    ASSUME(gBattleMoves[MOVE_WICKED_TORQUE].target == MOVE_TARGET_SELECTED);
    ASSUME(gBattleMoves[MOVE_NOXIOUS_TORQUE].target == MOVE_TARGET_SELECTED);
    ASSUME(gBattleMoves[MOVE_COMBAT_TORQUE].target == MOVE_TARGET_SELECTED);
    ASSUME(gBattleMoves[MOVE_MAGICAL_TORQUE].target == MOVE_TARGET_SELECTED);
}

SINGLE_BATTLE_TEST("Starmobile uses a random Torque move when first dropping below half HP each battle")
{
    GIVEN {
        PLAYER(SPECIES_REVAVROOM)
        {
            Ability(ABILITY_OVERCOAT);
            UniqueAbility(ABILITY_STARMOBILE);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_STARMOBILE, MOVE_BLAZING_TORQUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_STARMOBILE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BLAZING_TORQUE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Starmobile only triggers once each battle")
{
    GIVEN {
        PLAYER(SPECIES_REVAVROOM)
        {
            Ability(ABILITY_OVERCOAT);
            UniqueAbility(ABILITY_STARMOBILE);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_STARMOBILE, MOVE_BLAZING_TORQUE)); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_STARMOBILE, MOVE_MAGICAL_TORQUE)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STARMOBILE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BLAZING_TORQUE, player);
        NOT ABILITY_POPUP(player, ABILITY_STARMOBILE);
    }
}

SINGLE_BATTLE_TEST("Starmobile does not trigger if already below half HP")
{
    GIVEN {
        PLAYER(SPECIES_REVAVROOM)
        {
            Ability(ABILITY_OVERCOAT);
            UniqueAbility(ABILITY_STARMOBILE);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 - 1);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_STARMOBILE, MOVE_BLAZING_TORQUE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_STARMOBILE);
    }
}

SINGLE_BATTLE_TEST("Starmobile chooses from every Torque move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_BLAZING_TORQUE,
        MOVE_WICKED_TORQUE,
        MOVE_NOXIOUS_TORQUE,
        MOVE_COMBAT_TORQUE,
        MOVE_MAGICAL_TORQUE,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_STARMOBILE);

    GIVEN {
        PLAYER(SPECIES_REVAVROOM)
        {
            Ability(ABILITY_OVERCOAT);
            UniqueAbility(ABILITY_STARMOBILE);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_STARMOBILE);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
