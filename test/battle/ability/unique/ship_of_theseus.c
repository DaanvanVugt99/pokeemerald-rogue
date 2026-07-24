#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Ship of Theseus passes moves, PP, and both Abilities when switching")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            Ability(ABILITY_HUGE_POWER);
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            Moves(MOVE_TACKLE, MOVE_SWORDS_DANCE);
        }
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            UniqueAbility(ABILITY_BOG_BODY);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, moveSlot: 0); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_SHIP_OF_THESEUS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    } THEN {
        EXPECT_EQ(player->moves[0], MOVE_TACKLE);
        EXPECT_EQ(player->moves[1], MOVE_SWORDS_DANCE);
        EXPECT_EQ(player->pp[0], gBattleMoves[MOVE_TACKLE].pp - 2);
        EXPECT_EQ(GetBattlerPrimaryAbility(B_POSITION_PLAYER_LEFT), ABILITY_HUGE_POWER);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_SHIP_OF_THESEUS);
        EXPECT(!HasBattlerAbility(B_POSITION_PLAYER_LEFT, ABILITY_BOG_BODY));
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE1), MOVE_CELEBRATE);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_PP1), gBattleMoves[MOVE_CELEBRATE].pp);
    }
}

SINGLE_BATTLE_TEST("Ship of Theseus continues through successive replacements")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            Ability(ABILITY_PRANKSTER);
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            Moves(MOVE_THUNDER_WAVE, MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SPLASH); }
        PLAYER(SPECIES_CHANSEY) { Ability(ABILITY_NATURAL_CURE); Moves(MOVE_SOFT_BOILED); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 2); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHIP_OF_THESEUS);
        ABILITY_POPUP(player, ABILITY_SHIP_OF_THESEUS);
    } THEN {
        EXPECT_EQ(player->moves[0], MOVE_THUNDER_WAVE);
        EXPECT_EQ(GetBattlerPrimaryAbility(B_POSITION_PLAYER_LEFT), ABILITY_PRANKSTER);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_SHIP_OF_THESEUS);
    }
}

SINGLE_BATTLE_TEST("Ship of Theseus continues after its carrier faints")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            HP(1);
            MaxHP(1);
            Ability(ABILITY_SKILL_LINK);
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            Moves(MOVE_ICICLE_SPEAR);
        }
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_ICICLE_SPEAR); MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SHIP_OF_THESEUS);
    } THEN {
        EXPECT_EQ(player->moves[0], MOVE_ICICLE_SPEAR);
        EXPECT_EQ(GetBattlerPrimaryAbility(B_POSITION_PLAYER_LEFT), ABILITY_SKILL_LINK);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_SHIP_OF_THESEUS);
    }
}

SINGLE_BATTLE_TEST("Ship of Theseus keeps externally changed PP out of the replacement's party data")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            Moves(MOVE_TACKLE);
        }
        PLAYER(SPECIES_WOBBUFFET) {
            Speed(100);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) {
            Speed(50);
            Moves(MOVE_CELEBRATE, MOVE_EERIE_SPELL);
        }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, moveSlot: 0); MOVE(opponent, MOVE_EERIE_SPELL); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EERIE_SPELL, opponent);
    } THEN {
        EXPECT_EQ(player->pp[0], gBattleMoves[MOVE_TACKLE].pp - 4);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE1), MOVE_CELEBRATE);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_PP1), gBattleMoves[MOVE_CELEBRATE].pp);
    }
}

SINGLE_BATTLE_TEST("Ship of Theseus restores inherited PP without changing the replacement's party data")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            MovesWithPP({MOVE_TACKLE, 0});
        }
        PLAYER(SPECIES_WOBBUFFET) {
            Item(ITEM_LEPPA_BERRY);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->pp[0], 10);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_MOVE1), MOVE_CELEBRATE);
        EXPECT_EQ(GetMonData(&gPlayerParty[1], MON_DATA_PP1), gBattleMoves[MOVE_CELEBRATE].pp);
    }
}

SINGLE_BATTLE_TEST("Ship of Theseus preserves its passed Ability through Dynamax")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            Ability(ABILITY_HUGE_POWER);
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            Moves(MOVE_TACKLE);
        }
        PLAYER(SPECIES_WOBBUFFET) {
            Ability(ABILITY_SHADOW_TAG);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, moveSlot: 0, dynamax: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAX_STRIKE, player);
    } THEN {
        EXPECT(IsDynamaxed(B_POSITION_PLAYER_LEFT));
        EXPECT_EQ(GetBattlerPrimaryAbility(B_POSITION_PLAYER_LEFT), ABILITY_HUGE_POWER);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_SHIP_OF_THESEUS);
    }
}

SINGLE_BATTLE_TEST("Ship of Theseus preserves its passed Ability through Mega Evolution")
{
    GIVEN {
        PLAYER(SPECIES_MEW) {
            Ability(ABILITY_LEVITATE);
            UniqueAbility(ABILITY_SHIP_OF_THESEUS);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_VENUSAUR) {
            Ability(ABILITY_OVERGROW);
            Item(ITEM_VENUSAURITE);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE, megaEvolve: TRUE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->species, SPECIES_VENUSAUR_MEGA);
        EXPECT_EQ(GetBattlerPrimaryAbility(B_POSITION_PLAYER_LEFT), ABILITY_LEVITATE);
        EXPECT_EQ(GetBattlerUniqueAbility(B_POSITION_PLAYER_LEFT), ABILITY_SHIP_OF_THESEUS);
    }
}
