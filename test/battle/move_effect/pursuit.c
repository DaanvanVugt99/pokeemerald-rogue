#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PURSUIT].effect == EFFECT_PURSUIT);
}

SINGLE_BATTLE_TEST("Pursuited mon correctly switches out after it got hit and activated ability Tangling Hair")
{
    GIVEN {
        PLAYER(SPECIES_DUGTRIO) { Ability(ABILITY_TANGLING_HAIR); }
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_PURSUIT); }
    } SCENE {
        MESSAGE("Dugtrio, that's enough! Come back!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, opponent);
        ABILITY_POPUP(player, ABILITY_TANGLING_HAIR);
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, opponent);
        MESSAGE("Foe Wynaut's Speed fell!");
        MESSAGE("Go! Wobbuffet!");
    }
}

SINGLE_BATTLE_TEST("Pursuit intercepts a foe using a pivot move and doubles its power", s16 damage)
{
    bool32 pursuitFirst;
    PARAMETRIZE { pursuitFirst = TRUE; }
    PARAMETRIZE { pursuitFirst = FALSE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(60); Defense(100); HP(500); MaxHP(500); Moves(MOVE_VOLT_SWITCH); }
        PLAYER(SPECIES_WYNAUT) { Speed(30); }
        OPPONENT(SPECIES_PERSIAN) { Speed(pursuitFirst ? 100 : 40); Attack(100); SpDefense(100); HP(500); MaxHP(500); Moves(MOVE_PURSUIT); }
    } WHEN {
        if (pursuitFirst)
            TURN { MOVE(opponent, MOVE_PURSUIT); MOVE(player, MOVE_VOLT_SWITCH); SEND_OUT(player, 1); }
        else
            TURN { MOVE(player, MOVE_VOLT_SWITCH); MOVE(opponent, MOVE_PURSUIT); SEND_OUT(player, 1); }
    } SCENE {
        if (!pursuitFirst)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_VOLT_SWITCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, opponent);
        HP_BAR(player, captureDamage: &results[i].damage);
        if (pursuitFirst)
            ANIMATION(ANIM_TYPE_MOVE, MOVE_VOLT_SWITCH, player);
        MESSAGE("Go! Wynaut!");
        NOT ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, opponent);
    } FINALLY {
        EXPECT_GT(results[1].damage, results[0].damage * 17 / 10);
    }
}

SINGLE_BATTLE_TEST("Pursuit attacks the replacement after Baton Pass")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(80); Moves(MOVE_BATON_PASS); }
        PLAYER(SPECIES_WYNAUT) { Speed(30); HP(500); MaxHP(500); }
        OPPONENT(SPECIES_PERSIAN) { Speed(40); Moves(MOVE_PURSUIT); }
    } WHEN {
        TURN { MOVE(player, MOVE_BATON_PASS); SEND_OUT(player, 1); MOVE(opponent, MOVE_PURSUIT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
        MESSAGE("Go! Wynaut!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, opponent);
    }
}
