#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHADOW_BONE].power > 0);
}

SINGLE_BATTLE_TEST("Funeral Dance makes bone moves trap the target")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK_ALOLAN) { Speed(100); Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_FUNERAL_DANCE); Moves(MOVE_SHADOW_BONE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(1); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BONE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FUNERAL_DANCE);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT_EQ(gDisableStructs[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].battlerPreventingEscape,
                  GetBattlerAtPosition(B_POSITION_PLAYER_LEFT));
        EXPECT_EQ(opponent->status1 & STATUS1_BURN, 0);
    }
}

SINGLE_BATTLE_TEST("Funeral Dance also burns if no ally remains")
{
    GIVEN {
        PLAYER(SPECIES_MAROWAK_ALOLAN) { Speed(100); Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_FUNERAL_DANCE); Moves(MOVE_SHADOW_BONE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(0); Speed(1); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BONE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FUNERAL_DANCE);
        STATUS_ICON(opponent, burn: TRUE);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_ESCAPE_PREVENTION);
        EXPECT(opponent->status1 & STATUS1_BURN);
    }
}
