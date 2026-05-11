#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SNARL].soundMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
}

SINGLE_BATTLE_TEST("War Choir raises Defense after a successful sound move")
{
    GIVEN {
        PLAYER(SPECIES_KOMMO_O) { Ability(ABILITY_BULLETPROOF); UniqueAbility(ABILITY_WAR_CHOIR); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        ABILITY_POPUP(player, ABILITY_WAR_CHOIR);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("War Choir does not raise Defense after a non-sound move")
{
    GIVEN {
        PLAYER(SPECIES_KOMMO_O) { Ability(ABILITY_BULLETPROOF); UniqueAbility(ABILITY_WAR_CHOIR); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WAR_CHOIR);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("War Choir does not trigger if the sound move fails")
{
    GIVEN {
        PLAYER(SPECIES_KOMMO_O) { Ability(ABILITY_BULLETPROOF); UniqueAbility(ABILITY_WAR_CHOIR); Moves(MOVE_SNARL); }
        OPPONENT(SPECIES_MIGHTYENA) { Ability(ABILITY_SOUNDPROOF); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_WAR_CHOIR);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
