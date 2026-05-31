#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DOODLE].effect == EFFECT_DOODLE);
    ASSUME(gSpeciesInfo[SPECIES_GRAFAIAI].abilities[2] == ABILITY_PRANKSTER);
    ASSUME(gSpeciesInfo[SPECIES_SQUAWKABILLY].abilities[0] == ABILITY_INTIMIDATE);
}

DOUBLE_BATTLE_TEST("Doodle copies the original target's ability to the user and ally")
{
    GIVEN {
        PLAYER(SPECIES_FROSLASS) { Item(ITEM_FROSLASSITE); Speed(100); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GRAFAIAI) { Ability(ABILITY_PRANKSTER); Speed(90); Moves(MOVE_DOODLE); }
        OPPONENT(SPECIES_BAYLEEF) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_SQUAWKABILLY) { Ability(ABILITY_INTIMIDATE); Speed(40); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE, megaEvolve: TRUE);
            MOVE(playerRight, MOVE_DOODLE, target: opponentRight);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        MESSAGE("Grafaiai used Doodle!");
        NONE_OF {
            ABILITY_POPUP(playerLeft, ABILITY_WHITEOUT);
            ABILITY_POPUP(playerRight, ABILITY_GRAFITTI_TAG);
        }
        MESSAGE("Froslass used Celebrate!");
    } THEN {
        EXPECT_EQ(playerRight->ability, ABILITY_INTIMIDATE);
    }
}
