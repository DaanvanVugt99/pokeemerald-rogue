#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_PARTING_SHOT].effect == EFFECT_PARTING_SHOT);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
}

SINGLE_BATTLE_TEST("Bad Mouth uses Parting Shot after the first successful sound move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_BAD_MOUTH); Moves(MOVE_HYPER_VOICE); }
        PLAYER(SPECIES_CHANSEY) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_BAD_MOUTH);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PARTING_SHOT, player);
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_ATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(opponent->statStages[STAT_SPATK], DEFAULT_STAT_STAGE - 1);
        EXPECT_EQ(player->species, SPECIES_CHANSEY);
    }
}

SINGLE_BATTLE_TEST("Bad Mouth does not trigger after a non-sound move")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_BAD_MOUTH); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_CHANSEY) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_BAD_MOUTH);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_PARTING_SHOT, player);
        }
    }
}
