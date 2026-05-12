#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID].type == TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_POISON);
    ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].type == TYPE_DRAGON);
    ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].power > 20);
}

SINGLE_BATTLE_TEST("Ultra Injection uses Dragon Pulse after Poison-type moves")
{
    GIVEN {
        PLAYER(SPECIES_NAGANADEL) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_ACID); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ACID, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_ULTRA_INJECTION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_PULSE, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Ultra Injection does not trigger after non-Poison moves")
{
    GIVEN {
        PLAYER(SPECIES_NAGANADEL) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ULTRA_INJECTION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_PULSE, player);
        }
    }
}

SINGLE_BATTLE_TEST("Ultra Injection does not trigger if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_NAGANADEL) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_ACID); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ACID); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_INJECTION);
    }
}

SINGLE_BATTLE_TEST("Ultra Injection's called Dragon Pulse uses 20 base power", s16 damage)
{
    u32 move;
    u32 uniqueAbility;

    PARAMETRIZE { move = MOVE_DRAGON_PULSE; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { move = MOVE_ACID; uniqueAbility = ABILITY_ULTRA_INJECTION; }

    GIVEN {
        PLAYER(SPECIES_NAGANADEL) { SpAttack(100); Ability(ABILITY_BEAST_BOOST); UniqueAbility(uniqueAbility); Moves(move); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); SpDefense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_GT(results[0].damage, results[1].damage);
        EXPECT_GT(results[1].damage, 0);
    }
}
