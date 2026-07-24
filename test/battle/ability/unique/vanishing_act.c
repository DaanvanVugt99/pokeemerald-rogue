#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].split != SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_CELEBRATE].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Vanishing Act switches the user out after it attacks while disguised")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK) { Speed(100); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_VANISHING_ACT); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_VANISHING_ACT);
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WOBBUFFET);
    }
}

SINGLE_BATTLE_TEST("Vanishing Act does not switch after Illusion is broken")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_VANISHING_ACT); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_SCRATCH); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SCRATCH); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VANISHING_ACT);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_ZOROARK);
    }
}

SINGLE_BATTLE_TEST("Vanishing Act does not switch after a status move")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK) { Speed(100); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_VANISHING_ACT); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VANISHING_ACT);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_ZOROARK);
    }
}

SINGLE_BATTLE_TEST("Vanishing Act does not switch when the attack is protected against")
{
    GIVEN {
        PLAYER(SPECIES_ZOROARK) { Speed(1); Ability(ABILITY_ILLUSION); UniqueAbility(ABILITY_VANISHING_ACT); Moves(MOVE_SCRATCH); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_VANISHING_ACT);
        }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_ZOROARK);
    }
}
