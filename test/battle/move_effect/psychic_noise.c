#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PSYCHIC_NOISE].effect == EFFECT_PSYCHIC_NOISE);
    ASSUME(gBattleMoves[MOVE_PSYCHIC_NOISE].argument == MOVE_EFFECT_PSYCHIC_NOISE);
    ASSUME(gBattleMoves[MOVE_RECOVER].effect == EFFECT_RESTORE_HP);
}

SINGLE_BATTLE_TEST("Psychic Noise blocks healing moves for two turns")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC_NOISE); MOVE(opponent, MOVE_RECOVER); }
        TURN { MOVE(opponent, MOVE_RECOVER, allowed: FALSE); }
        TURN { MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_NOISE, player);
        MESSAGE("Foe Wobbuffet was prevented from healing!");
        MESSAGE("Foe Wobbuffet was prevented from healing!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_STRUGGLE, opponent);
        MESSAGE("Foe Wobbuffet's Heal Block wore off!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponent);
    }
}

SINGLE_BATTLE_TEST("Psychic Noise's healing block is prevented by Aroma Veil")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MILCERY) { Ability(ABILITY_AROMA_VEIL); }
    } WHEN {
        TURN { MOVE(player, MOVE_PSYCHIC_NOISE); MOVE(opponent, MOVE_RECOVER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_NOISE, player);
        ABILITY_POPUP(opponent, ABILITY_AROMA_VEIL);
        MESSAGE("Foe Milcery is protected by an aromatic veil!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponent);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_HEAL_BLOCK));
    }
}

DOUBLE_BATTLE_TEST("Psychic Noise's healing block is prevented by a partner's Aroma Veil")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_MILCERY) { Ability(ABILITY_AROMA_VEIL); }
    } WHEN {
        TURN { MOVE(playerLeft, MOVE_PSYCHIC_NOISE, target: opponentLeft); MOVE(opponentLeft, MOVE_RECOVER); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYCHIC_NOISE, playerLeft);
        ABILITY_POPUP(opponentRight, ABILITY_AROMA_VEIL);
        MESSAGE("Foe Wobbuffet is protected by an aromatic veil!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RECOVER, opponentLeft);
    } THEN {
        EXPECT(!(gStatuses3[B_POSITION_OPPONENT_LEFT] & STATUS3_HEAL_BLOCK));
    }
}
