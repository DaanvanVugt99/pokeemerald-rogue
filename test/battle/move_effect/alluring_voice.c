#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ALLURING_VOICE].effect == EFFECT_CONFUSE_HIT);
    ASSUME(gBattleMoves[MOVE_ALLURING_VOICE].secondaryEffectChance == 100);
}

SINGLE_BATTLE_TEST("Alluring Voice confuses a target that raised a stat this turn")
{
    u16 setupMove;

    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_SWORDS_DANCE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, setupMove); MOVE(player, MOVE_ALLURING_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ALLURING_VOICE, player);
        HP_BAR(opponent);
        if (setupMove == MOVE_SWORDS_DANCE)
            ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, opponent);
        else
            NOT ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, opponent);
    } THEN {
        if (setupMove == MOVE_SWORDS_DANCE)
            EXPECT(opponent->status2 & STATUS2_CONFUSION);
        else
            EXPECT(!(opponent->status2 & STATUS2_CONFUSION));
    }
}

SINGLE_BATTLE_TEST("Alluring Voice's confusion effect is removed by Sheer Force")
{
    GIVEN {
        PLAYER(SPECIES_NIDOKING) { Ability(ABILITY_SHEER_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(opponent, MOVE_SWORDS_DANCE); MOVE(player, MOVE_ALLURING_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ALLURING_VOICE, player);
        HP_BAR(opponent);
        NOT ANIMATION(ANIM_TYPE_STATUS, B_ANIM_STATUS_CONFUSION, opponent);
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_CONFUSION));
    }
}
