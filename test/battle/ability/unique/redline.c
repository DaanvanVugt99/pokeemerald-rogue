#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_AGILITY].effect == EFFECT_SPEED_UP_2);
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Redline raises Speed by 1 before the first turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(80); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_REDLINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_REDLINE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Redline raised its Speed!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Redline lowers Speed by 1 at the end of each turn")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_REDLINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_REDLINE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ABILITY_POPUP(player, ABILITY_REDLINE);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player);
        MESSAGE("Wobbuffet's Speed fell!");
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Redline no longer makes Electric moves use Speed", s16 damage)
{
    bool32 useAgility;

    PARAMETRIZE { useAgility = FALSE; }
    PARAMETRIZE { useAgility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(79); MaxHP(160); Speed(100); SpAttack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_REDLINE); Moves(MOVE_AGILITY, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        if (useAgility)
            TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
