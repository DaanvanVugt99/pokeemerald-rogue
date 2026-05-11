#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Psychic Maw taunts on biting moves and only blocks the target's next turn")
{
    GIVEN {
        PLAYER(SPECIES_BRUXISH) { Speed(50); Ability(ABILITY_DAZZLING); UniqueAbility(ABILITY_PSYCHIC_MAW); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PSYCHIC_MAW);
    } THEN {
        EXPECT_EQ(opponent->pp[0], 39);
        EXPECT_EQ((int)gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].tauntTimer, 1);
    }
}

SINGLE_BATTLE_TEST("Psychic Maw does not trigger on non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_BRUXISH) { Ability(ABILITY_DAZZLING); UniqueAbility(ABILITY_PSYCHIC_MAW); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PSYCHIC_MAW);
            MESSAGE("Foe Wobbuffet fell for the taunt!");
        }
    }
}

SINGLE_BATTLE_TEST("Psychic Maw respects Oblivious")
{
    GIVEN {
        PLAYER(SPECIES_BRUXISH) { Ability(ABILITY_DAZZLING); UniqueAbility(ABILITY_PSYCHIC_MAW); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_OBLIVIOUS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PSYCHIC_MAW);
            MESSAGE("Foe Wobbuffet fell for the taunt!");
        }
    }
}
