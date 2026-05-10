#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_FLAMETHROWER].power > gBattleMoves[MOVE_TACKLE].power);
}

SINGLE_BATTLE_TEST("Interrogation disables the target's strongest move after hitting it")
{
    GIVEN {
        PLAYER(SPECIES_GUMSHOOS) { Speed(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_INTERROGATION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_FLAMETHROWER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_INTERROGATION);
        MESSAGE("Foe Wobbuffet's Flamethrower was disabled!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_FLAMETHROWER);
        EXPECT_EQ((u32)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disableTimer, 3);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Interrogation lowers Defense if the target already moved this turn")
{
    GIVEN {
        PLAYER(SPECIES_GUMSHOOS) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_INTERROGATION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE, MOVE_FLAMETHROWER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_INTERROGATION);
        MESSAGE("Foe Wobbuffet's Flamethrower was disabled!");
        MESSAGE("Foe Wobbuffet's Defense fell!");
    } THEN {
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].disabledMove, MOVE_FLAMETHROWER);
        EXPECT_EQ(opponent->statStages[STAT_DEF], DEFAULT_STAT_STAGE - 1);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Interrogation only triggers once per switch-in")
{
    GIVEN {
        PLAYER(SPECIES_GUMSHOOS) { Speed(100); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_INTERROGATION); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_TACKLE, MOVE_FLAMETHROWER, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_INTERROGATION);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_INTERROGATION);
        }
    }
}
