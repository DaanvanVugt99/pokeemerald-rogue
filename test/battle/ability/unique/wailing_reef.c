#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].power > 0);
    ASSUME(gBattleMoves[MOVE_GROWL].soundMove);
    ASSUME(gBattleMoves[MOVE_GROWL].power == 0);
    ASSUME(!gBattleMoves[MOVE_SWIFT].soundMove);
    ASSUME(gBattleMoves[MOVE_SWIFT].power > 0);
}

SINGLE_BATTLE_TEST("Wailing Reef curses targets hit by sound-based moves")
{
    u16 move;
    bool32 shouldCurse;

    PARAMETRIZE { move = MOVE_HYPER_VOICE; shouldCurse = TRUE; }
    PARAMETRIZE { move = MOVE_GROWL; shouldCurse = TRUE; }
    PARAMETRIZE { move = MOVE_SWIFT; shouldCurse = FALSE; }

    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Speed(100); Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_WAILING_REEF); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (shouldCurse)
            ABILITY_POPUP(player, ABILITY_WAILING_REEF);
        else
            NOT ABILITY_POPUP(player, ABILITY_WAILING_REEF);
    } THEN {
        if (shouldCurse)
            EXPECT(opponent->status2 & STATUS2_CURSED);
        else
            EXPECT_EQ(opponent->status2 & STATUS2_CURSED, 0);
    }
}

SINGLE_BATTLE_TEST("Wailing Reef does not curse when a sound move has no effect")
{
    GIVEN {
        PLAYER(SPECIES_CURSOLA) { Speed(100); Ability(ABILITY_WEAK_ARMOR); UniqueAbility(ABILITY_WAILING_REEF); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_VOLTORB) { Speed(1); Ability(ABILITY_SOUNDPROOF); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_WAILING_REEF);
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_CURSED, 0);
    }
}
