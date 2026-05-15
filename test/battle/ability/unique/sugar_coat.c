#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_ABSORB].effect == EFFECT_ABSORB);
    ASSUME(gBattleMoves[MOVE_DECORATE].effect == EFFECT_DECORATE);
}

SINGLE_BATTLE_TEST("Sugar Coat uses Decorate on itself after the first healing move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ALCREMIE) { HP(50); MaxHP(100); Speed(100); Ability(ABILITY_SWEET_VEIL); UniqueAbility(ABILITY_SUGAR_COAT); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ABILITY_POPUP(player, ABILITY_SUGAR_COAT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DECORATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Sugar Coat does not trigger if the healing move restores no HP")
{
    GIVEN {
        PLAYER(SPECIES_ALCREMIE) { HP(100); MaxHP(100); Speed(100); Ability(ABILITY_SWEET_VEIL); UniqueAbility(ABILITY_SUGAR_COAT); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SUGAR_COAT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DECORATE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Sugar Coat only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ALCREMIE) { HP(50); MaxHP(100); Speed(100); Ability(ABILITY_SWEET_VEIL); UniqueAbility(ABILITY_SUGAR_COAT); Moves(MOVE_RECOVER); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        ABILITY_POPUP(player, ABILITY_SUGAR_COAT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DECORATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SUGAR_COAT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DECORATE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Sugar Coat refreshes after switching out and back in")
{
    GIVEN {
        PLAYER(SPECIES_ALCREMIE) { HP(50); MaxHP(100); Speed(100); Ability(ABILITY_SWEET_VEIL); UniqueAbility(ABILITY_SUGAR_COAT); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(100); Speed(1); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SUGAR_COAT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DECORATE, player);
        ABILITY_POPUP(player, ABILITY_SUGAR_COAT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DECORATE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 2);
    }
}
