#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Night Terror damages sleeping opposing Pokemon by 1/8 HP during Eclipse")
{
    u32 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_ECLIPSE; }

    GIVEN {
        PLAYER(SPECIES_DARKRAI) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_NIGHT_TERROR); Moves(setupMove); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Status1(STATUS1_SLEEP); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, setupMove); }
    } SCENE {
        if (setupMove == MOVE_ECLIPSE) {
            ABILITY_POPUP(player, ABILITY_NIGHT_TERROR);
            MESSAGE("Foe Wobbuffet is tormented!");
            HP_BAR(opponent);
        }
        else {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_NIGHT_TERROR);
                MESSAGE("Foe Wobbuffet is tormented!");
                HP_BAR(opponent);
            };
        }
    } THEN {
        if (setupMove == MOVE_ECLIPSE)
            EXPECT_EQ(opponent->hp, opponent->maxHP - opponent->maxHP / 8);
        else
            EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Night Terror stacks with Bad Dreams during Eclipse")
{
    GIVEN {
        PLAYER(SPECIES_DARKRAI) { Speed(100); Ability(ABILITY_BAD_DREAMS); UniqueAbility(ABILITY_NIGHT_TERROR); Moves(MOVE_ECLIPSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Status1(STATUS1_SLEEP); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ECLIPSE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NIGHT_TERROR);
        MESSAGE("Foe Wobbuffet is tormented!");
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_BAD_DREAMS);
        MESSAGE("Foe Wobbuffet is tormented!");
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP - opponent->maxHP / 4);
    }
}
