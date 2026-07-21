#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_AIR_SLASH].makesContact);
    ASSUME(gBattleMoves[MOVE_DARK_PULSE].type == TYPE_DARK);
    ASSUME(gBattleMoves[MOVE_PSYCHIC].type != TYPE_DARK);
}

SINGLE_BATTLE_TEST("Spirit Feast makes the next Dark move drain half its damage after a contact hit")
{
    s16 damage;
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(500); SpAttack(100); UniqueAbility(ABILITY_SPIRIT_FEAST); Moves(MOVE_CELEBRATE, MOVE_DARK_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_DARK_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_PULSE, player);
        HP_BAR(opponent, captureDamage: &damage);
        ABILITY_POPUP(player, ABILITY_SPIRIT_FEAST);
        HP_BAR(player, captureDamage: &healed);
        MESSAGE("Foe Wobbuffet had its energy drained!");
    } THEN {
        EXPECT_MUL_EQ(damage, Q_4_12(-1.0 / 2.0), healed);
    }
}

SINGLE_BATTLE_TEST("Spirit Feast is not primed by non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(500); UniqueAbility(ABILITY_SPIRIT_FEAST); Moves(MOVE_CELEBRATE, MOVE_DARK_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_AIR_SLASH, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_AIR_SLASH); }
        TURN { MOVE(player, MOVE_DARK_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SPIRIT_FEAST);
            MESSAGE("Foe Wobbuffet had its energy drained!");
        }
    }
}

SINGLE_BATTLE_TEST("Spirit Feast is preserved through non-Dark attacks")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(500); SpAttack(100); UniqueAbility(ABILITY_SPIRIT_FEAST); Moves(MOVE_CELEBRATE, MOVE_PSYCHIC, MOVE_DARK_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_PSYCHIC); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DARK_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DARK_PULSE, player);
        ABILITY_POPUP(player, ABILITY_SPIRIT_FEAST);
    }
}

SINGLE_BATTLE_TEST("Spirit Feast is preserved when a Dark move misses")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(100); MaxHP(500); SpAttack(100); UniqueAbility(ABILITY_SPIRIT_FEAST); Moves(MOVE_CELEBRATE, MOVE_DARK_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_DARK_PULSE, hit: FALSE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_DARK_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPIRIT_FEAST);
    }
}
