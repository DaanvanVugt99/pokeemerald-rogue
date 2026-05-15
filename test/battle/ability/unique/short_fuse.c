#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNARL].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_CHARGE].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Short Fuse uses Charge after Morpeko uses a Dark-type move")
{
    u16 species;

    PARAMETRIZE { species = SPECIES_MORPEKO_FULL_BELLY; }
    PARAMETRIZE { species = SPECIES_MORPEKO_HANGRY; }

    GIVEN {
        PLAYER(species) { Ability(ABILITY_HUNGER_SWITCH); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        ABILITY_POPUP(player, ABILITY_SHORT_FUSE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        MESSAGE("Morpeko began charging power!");
    }
}

SINGLE_BATTLE_TEST("Short Fuse does not trigger after non-Dark moves")
{
    GIVEN {
        PLAYER(SPECIES_MORPEKO) { Ability(ABILITY_HUNGER_SWITCH); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SHORT_FUSE);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
    }
}

SINGLE_BATTLE_TEST("Short Fuse Charge boosts Morpeko's next Electric move")
{
    s16 damageBefore;
    s16 damageAfter;

    GIVEN {
        PLAYER(SPECIES_MORPEKO) { Speed(100); Ability(ABILITY_HUNGER_SWITCH); Attack(100); SpAttack(100); Moves(MOVE_THUNDER_SHOCK, MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_LIMBER); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SNARL, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damageBefore);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        ABILITY_POPUP(player, ABILITY_SHORT_FUSE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        MESSAGE("Morpeko began charging power!");
        HP_BAR(opponent, captureDamage: &damageAfter);
    } THEN {
        EXPECT_MUL_EQ(damageBefore, Q_4_12(2.0), damageAfter);
    }
}
