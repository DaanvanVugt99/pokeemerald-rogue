#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
    ASSUME(gBattleMoves[MOVE_SPOTLIGHT].effect == EFFECT_FOLLOW_ME);
}

DOUBLE_BATTLE_TEST("Sporelight uses Spotlight after a healing move")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) { HP(50); MaxHP(400); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPORELIGHT); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RECOVER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Sporelight does not trigger after non-healing moves")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) { Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPORELIGHT); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_TACKLE, target: opponentLeft); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, playerLeft);
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
        }
    }
}

DOUBLE_BATTLE_TEST("Sporelight triggers after every healing move")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) { HP(50); MaxHP(400); Ability(ABILITY_EFFECT_SPORE); UniqueAbility(ABILITY_SPORELIGHT); Moves(MOVE_RECOVER); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_RECOVER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
        TURN { MOVE(playerLeft, MOVE_RECOVER); MOVE(playerRight, MOVE_CELEBRATE); MOVE(opponentLeft, MOVE_CELEBRATE); MOVE(opponentRight, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, playerLeft);
        ABILITY_POPUP(playerLeft, ABILITY_SPORELIGHT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPOTLIGHT, playerLeft);
    }
}

DOUBLE_BATTLE_TEST("Sporelight gives healing moves +1 priority in Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) {
            Speed(50);
            HP(50);
            MaxHP(400);
            Ability(ABILITY_EFFECT_SPORE);
            UniqueAbility(ABILITY_SPORELIGHT);
            Moves(MOVE_RECOVER, MOVE_CELEBRATE);
        }
        PLAYER(SPECIES_WOBBUFFET) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_MISTY_TERRAIN, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(opponentLeft, MOVE_MISTY_TERRAIN);
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
        TURN {
            MOVE(playerLeft, MOVE_RECOVER);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Shiinotic used Recover!");
        MESSAGE("Foe Wobbuffet used Tackle!");
    }
}

DOUBLE_BATTLE_TEST("Sporelight does not give healing moves priority outside Misty Terrain")
{
    GIVEN {
        PLAYER(SPECIES_SHIINOTIC) {
            Speed(50);
            HP(50);
            MaxHP(400);
            Ability(ABILITY_EFFECT_SPORE);
            UniqueAbility(ABILITY_SPORELIGHT);
            Moves(MOVE_RECOVER);
        }
        PLAYER(SPECIES_WOBBUFFET) { Speed(25); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(10); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_RECOVER);
            MOVE(opponentLeft, MOVE_TACKLE, target: playerLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Foe Wobbuffet used Tackle!");
        MESSAGE("Shiinotic used Recover!");
    }
}
