#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HOWL].soundMove);
    ASSUME(gBattleMoves[MOVE_ROAR].soundMove);
    ASSUME(gBattleMoves[MOVE_SNARL].soundMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Charged Cry uses Charge after Howl")
{
    GIVEN {
        PLAYER(SPECIES_MANECTRIC) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_CHARGED_CRY); Moves(MOVE_HOWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HOWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HOWL, player);
        ABILITY_POPUP(player, ABILITY_CHARGED_CRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        MESSAGE("Manectric began charging power!");
    }
}

SINGLE_BATTLE_TEST("Charged Cry uses Charge after Roar")
{
    GIVEN {
        PLAYER(SPECIES_MANECTRIC) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_CHARGED_CRY); Moves(MOVE_ROAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROAR, player);
        MESSAGE("Foe Wynaut was dragged out!");
        ABILITY_POPUP(player, ABILITY_CHARGED_CRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        MESSAGE("Manectric began charging power!");
    } THEN {
        EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
    }
}

SINGLE_BATTLE_TEST("Charged Cry does not use Charge after failed Roar")
{
    GIVEN {
        PLAYER(SPECIES_MANECTRIC) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_CHARGED_CRY); Moves(MOVE_ROAR); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROAR); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHARGED_CRY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
    }
}

SINGLE_BATTLE_TEST("Charged Cry uses Charge after Snarl and boosts the next Electric move")
{
    s16 damageBefore;
    s16 damageAfter;

    GIVEN {
        PLAYER(SPECIES_MANECTRIC) { Speed(100); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_CHARGED_CRY); Moves(MOVE_SNARL, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_LIMBER); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SNARL, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &damageBefore);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        ABILITY_POPUP(player, ABILITY_CHARGED_CRY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        MESSAGE("Manectric began charging power!");
        HP_BAR(opponent, captureDamage: &damageAfter);
    } THEN {
        EXPECT_MUL_EQ(damageBefore, Q_4_12(2.0), damageAfter);
    }
}

SINGLE_BATTLE_TEST("Charged Cry does not trigger after non-sound moves")
{
    GIVEN {
        PLAYER(SPECIES_MANECTRIC) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_CHARGED_CRY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CHARGED_CRY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CHARGE, player);
        }
    }
}
