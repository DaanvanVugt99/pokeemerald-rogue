#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_FURY_SWIPES].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_PAY_DAY].power > 0);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FAKE_OUT].type == TYPE_NORMAL);
}

SINGLE_BATTLE_TEST("Passive Income adds Pay Day after Normal-type moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
    }
}

SINGLE_BATTLE_TEST("Passive Income does not trigger after non-Normal moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_PASSIVE_INCOME; }

    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); UniqueAbility(uniqueAbility); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Passive Income adds Pay Day after Fury Swipes")
{
    GIVEN {
        PLAYER(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Moves(MOVE_FURY_SWIPES); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_FURY_SWIPES); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FURY_SWIPES, player);
        ABILITY_POPUP(player, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, player);
    }
}

SINGLE_BATTLE_TEST("Passive Income popup is shown on the user after Fake Out")
{
    GIVEN {
        PLAYER(SPECIES_INTELEON) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEOWTH) { Ability(ABILITY_PICKUP); UniqueAbility(ABILITY_PASSIVE_INCOME); Speed(100); Moves(MOVE_FAKE_OUT); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_FAKE_OUT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        HP_BAR(player);
        ABILITY_POPUP(opponent, ABILITY_PASSIVE_INCOME);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PAY_DAY, opponent);
    }
}
