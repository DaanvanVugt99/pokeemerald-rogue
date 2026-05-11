#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
    ASSUME(gBattleMoves[MOVE_U_TURN].power > 20);
}

SINGLE_BATTLE_TEST("Break Formation uses 20 BP U-turn when first dropping below half HP")
{
    GIVEN {
        PLAYER(SPECIES_WISHIWASHI)
        {
            Ability(ABILITY_SCHOOLING);
            UniqueAbility(ABILITY_BREAK_FORMATION);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_BREAK_FORMATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Break Formation can switch out after Schooling changes Wishiwashi's form")
{
    GIVEN {
        PLAYER(SPECIES_WISHIWASHI)
        {
            Level(20);
            Ability(ABILITY_SCHOOLING);
            UniqueAbility(ABILITY_BREAK_FORMATION);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); SEND_OUT(player, 1); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_FORM_CHANGE, player);
        ABILITY_POPUP(player, ABILITY_BREAK_FORMATION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
        HP_BAR(opponent);
        MESSAGE("Go! Wynaut!");
    } THEN {
        EXPECT_EQ(player->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Break Formation does not trigger if already below half HP")
{
    GIVEN {
        PLAYER(SPECIES_WISHIWASHI)
        {
            Ability(ABILITY_SCHOOLING);
            UniqueAbility(ABILITY_BREAK_FORMATION);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 - 1);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BREAK_FORMATION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, player);
            MESSAGE("Go! Wynaut!");
        }
    } THEN {
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
