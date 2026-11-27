#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sheer Force boosts power, but removes secondary effects of moves", s16 damage)
{
    s32 j;
    u32 ability = 0, move = 0;

    for (j = 1; j < MOVES_COUNT; j++)
    {
        // This gets the boost and the buff... for some reason, according to GF
        // so we're just gonna ignore it from this test?
        if(gBattleMoves[j].effect == EFFECT_ELECTRO_SHOT)
            continue;

        if (gBattleMoves[j].sheerForceBoost && j != MOVE_ORDER_UP)
        {
            PARAMETRIZE { ability = ABILITY_ANGER_POINT; move = j; }
            PARAMETRIZE { ability = ABILITY_SHEER_FORCE; move = j; }
        }
    }

    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ability); Status1(move == MOVE_SNORE ? STATUS1_SLEEP : STATUS1_NONE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        if (move == MOVE_UPPER_HAND)
            TURN { MOVE(player, move); MOVE(opponent, MOVE_QUICK_ATTACK); }
        else
            TURN { MOVE(player, move); }
        if (gBattleMoves[move].effect == EFFECT_TWO_TURNS_ATTACK || gBattleMoves[move].effect == EFFECT_SEMI_INVULNERABLE) {
                TURN { SKIP_TURN(player); }
                TURN { ; }
        }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (ability == ABILITY_SHEER_FORCE) {
            NONE_OF {
                ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
                ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
                STATUS_ICON(opponent, STATUS1_FREEZE);
                STATUS_ICON(opponent, STATUS1_POISON);
                STATUS_ICON(opponent, STATUS1_BURN);
                STATUS_ICON(opponent, STATUS1_TOXIC_POISON);
                STATUS_ICON(opponent, STATUS1_PARALYSIS);
                MESSAGE("Wobbuffet is confused!");
                MESSAGE("Wobbuffet flinched!");
            }
            // Volt Tackle/Flare Blitz edge case: recoil happens, but target isn't statused
            if (gBattleMoves[move].effect == EFFECT_RECOIL_33_STATUS)
            {
                HP_BAR(player);
                MESSAGE("Tauros is hit with recoil!");
            }
        }
    } FINALLY {
        s32 j;
        for (j = 0; j < gBattleTestRunnerState->parametersCount; j+=2)
        {
            EXPECT_GT(results[j+1].damage, results[j].damage);
        }
    }
}

SINGLE_BATTLE_TEST("Sheer Force timing: Pickpocket steals after a boosted contact hit")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Ability(ABILITY_SHEER_FORCE); Item(ITEM_METAL_COAT); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_PICKPOCKET); }
    } WHEN {
        TURN { MOVE(player, MOVE_STOMP); }
    } THEN {
        EXPECT_EQ(player->item, ITEM_NONE);
        EXPECT_EQ(opponent->item, ITEM_METAL_COAT);
    }
}

SINGLE_BATTLE_TEST("Sheer Force timing: boosted hits suppress secondary effects, Life Orb recoil and Shell Bell healing")
{
    u16 item, move;
    PARAMETRIZE { item = ITEM_LIFE_ORB; move = MOVE_ACID_SPRAY; }
    PARAMETRIZE { item = ITEM_SHELL_BELL; move = MOVE_ACID_SPRAY; }
    PARAMETRIZE { item = ITEM_LIFE_ORB; move = MOVE_TACKLE; }
    PARAMETRIZE { item = ITEM_SHELL_BELL; move = MOVE_TACKLE; }
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { HP(100); MaxHP(200); Ability(ABILITY_SHEER_FORCE); Item(item); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, move); }
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
        if (move == MOVE_ACID_SPRAY)
            EXPECT_EQ(player->hp, 100);
        else if (item == ITEM_LIFE_ORB)
            EXPECT_LT(player->hp, 100);
        else
            EXPECT_GT(player->hp, 100);
    }
}

DOUBLE_BATTLE_TEST("Sheer Force timing: a boosted spread move triggers both exits in Speed order")
{
    GIVEN {
        PLAYER(SPECIES_TAUROS) { Speed(100); Ability(ABILITY_SHEER_FORCE); }
        PLAYER(SPECIES_WYNAUT) { Speed(80); }
        OPPONENT(SPECIES_GOLISOPOD) { Speed(10); MaxHP(500); HP(251); Ability(ABILITY_EMERGENCY_EXIT); }
        OPPONENT(SPECIES_WIMPOD) { Speed(20); MaxHP(500); HP(251); Ability(ABILITY_WIMP_OUT); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(30); }
        OPPONENT(SPECIES_WYNAUT) { Speed(40); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_ROCK_SLIDE); SEND_OUT(opponentRight, 2); SEND_OUT(opponentLeft, 3); }
    } SCENE {
        HP_BAR(opponentLeft);
        HP_BAR(opponentRight);
        ABILITY_POPUP(opponentRight, ABILITY_WIMP_OUT);
        ABILITY_POPUP(opponentLeft, ABILITY_EMERGENCY_EXIT);
    } THEN {
        EXPECT_EQ(opponentRight->species, SPECIES_WOBBUFFET);
        EXPECT_EQ(opponentLeft->species, SPECIES_WYNAUT);
    }
}
