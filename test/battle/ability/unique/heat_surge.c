#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FLAME_CHARGE].power == 50);
}

SINGLE_BATTLE_TEST("Heat Surge does not trigger on the first turn out")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ERUPTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Heat Surge uses Flame Charge after turn 1 out when using Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ERUPTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Heat Surge does not trigger on non-Fire moves")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ERUPTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Heat Surge can trigger repeatedly after turn 1")
{
    GIVEN {
        PLAYER(SPECIES_CYNDAQUIL)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ERUPTION);
        ABILITY_POPUP(player, ABILITY_ERUPTION);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}

DOUBLE_BATTLE_TEST("Heat Surge targets the foe targeted by the Fire move")
{
    GIVEN {
        PLAYER(SPECIES_TYPHLOSION)   { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_ERUPTION); Moves(MOVE_CELEBRATE, MOVE_EMBER); }
        PLAYER(SPECIES_WOBBUFFET)    { Moves(MOVE_CELEBRATE); }

        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET)  { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_EMBER, target: opponentRight); }
    } THEN {
        EXPECT_EQ(opponentLeft->hp, opponentLeft->maxHP);
        EXPECT_LT(opponentRight->hp, opponentRight->maxHP);
    }
}

SINGLE_BATTLE_TEST("Heat Surge uses a 20 BP Flame Charge", s16 damage)
{
    bool32 heatSurge;

    PARAMETRIZE { heatSurge = FALSE; }
    PARAMETRIZE { heatSurge = TRUE; }

    GIVEN {
        PLAYER(SPECIES_TYPHLOSION) {
            Ability(ABILITY_BLAZE);
            UniqueAbility(heatSurge ? ABILITY_ERUPTION : ABILITY_NONE);
            Moves(MOVE_CELEBRATE, heatSurge ? MOVE_EMBER : MOVE_FLAME_CHARGE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, heatSurge ? MOVE_EMBER : MOVE_FLAME_CHARGE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (heatSurge) {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
            HP_BAR(opponent);
            ABILITY_POPUP(player, ABILITY_ERUPTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        } else {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAME_CHARGE, player);
        }
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(0.4), results[1].damage);
    }
}
