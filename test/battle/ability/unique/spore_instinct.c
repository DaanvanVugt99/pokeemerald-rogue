#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPORE].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_LEECH_SEED].effect == EFFECT_LEECH_SEED);
}

SINGLE_BATTLE_TEST("Spore Instinct seeds targets after Brute Bonnet inflicts status if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_BRUTE_BONNET) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SPORE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_SCREAM_TAIL) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPORE, player);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_SPORE_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_SPORE_INSTINCT);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
            }
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
        if (hasOtherParadox)
            EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_LEECHSEED));
        else
            EXPECT(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_LEECHSEED);
    }
}

SINGLE_BATTLE_TEST("Spore Instinct does not trigger when the status fails")
{
    GIVEN {
        PLAYER(SPECIES_BRUTE_BONNET) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_SPORE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Status1(STATUS1_SLEEP_TURN(2)); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPORE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_SPORE_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_LEECH_SEED, player);
        }
    } THEN {
        EXPECT(opponent->status1 & STATUS1_SLEEP);
        EXPECT(!(gStatuses3[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)] & STATUS3_LEECHSEED));
    }
}
