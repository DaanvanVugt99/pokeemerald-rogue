#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_PURSUIT].effect == EFFECT_PURSUIT);
    ASSUME(gBattleMoves[MOVE_PURSUIT].power == 40);
}

SINGLE_BATTLE_TEST("Bounty does not activate if the target is not knocked out")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Level(100); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_BOUNTY);
    }
}

WILD_BATTLE_TEST("Bounty still pays out when knocking out the last opposing Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Level(100); Attack(255); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gPaydayMoney, 500);
    }
}

SINGLE_BATTLE_TEST("Bounty hits a switching foe with Pursuit")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BOUNTY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, player);
    }
}

SINGLE_BATTLE_TEST("Bounty does not trigger when the opponent does not switch")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BOUNTY);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, player);
        }
    }
}

SINGLE_BATTLE_TEST("Bounty does not replace a chosen Pursuit against a switching foe")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_PURSUIT); }
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WYNAUT);
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_PURSUIT); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_BOUNTY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PURSUIT, player);
    }
}
