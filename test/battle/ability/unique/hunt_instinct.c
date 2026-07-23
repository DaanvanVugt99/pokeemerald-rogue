#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].bitingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].bitingMove);
}

SINGLE_BATTLE_TEST("Hunt Instinct traps with the next biting move after a foe switches out if Roaring Moon is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_ROARING_MOON) { Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_BITE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_SCREAM_TAIL) { Speed(50); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_BITE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
            ABILITY_POPUP(player, ABILITY_HUNT_INSTINCT);
        else
            NOT ABILITY_POPUP(player, ABILITY_HUNT_INSTINCT);
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_EQ(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        }
        else
        {
            EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
            EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].battlerPreventingEscape,
                      GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
        }
    }
}

SINGLE_BATTLE_TEST("Hunt Instinct keeps its primed state through non-biting moves")
{
    GIVEN {
        PLAYER(SPECIES_ROARING_MOON) { Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_TACKLE, MOVE_BITE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_HUNT_INSTINCT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_HUNT_INSTINCT);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Hunt Instinct does not trap if no foe switched out")
{
    GIVEN {
        PLAYER(SPECIES_ROARING_MOON) { Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_BITE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_HUNT_INSTINCT);
    } THEN {
        EXPECT_EQ(opponent->status2 & STATUS2_ESCAPE_PREVENTION, 0);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}
