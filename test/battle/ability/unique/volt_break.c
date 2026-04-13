#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GROWL].soundMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(gBattleMoves[MOVE_GROWL].priority == 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].priority == 0);
}

SINGLE_BATTLE_TEST("Volt Break suppresses the opposing ability like Gastro Acid on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Speed(50); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_VOLT_BREAK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_NINJASK) { Speed(40); Ability(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Volt Break gives +1 priority to sound-based moves")
{
    u16 move;
    PARAMETRIZE { move = MOVE_GROWL; }
    PARAMETRIZE { move = MOVE_TACKLE; }

    GIVEN {
        PLAYER(SPECIES_VOLTORB) { Speed(10); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_VOLT_BREAK); Moves(MOVE_GROWL, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (move == MOVE_GROWL) {
            MESSAGE("Voltorb used Growl!");
            MESSAGE("Foe Wobbuffet used Celebrate!");
        } else {
            MESSAGE("Foe Wobbuffet used Celebrate!");
            MESSAGE("Voltorb used Tackle!");
        }
    }
}
