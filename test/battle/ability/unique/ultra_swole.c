#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
}

SINGLE_BATTLE_TEST("Ultra Swole raises Defense after a successful punching move if it is the only Ultra Beast")
{
    GIVEN {
        PLAYER(SPECIES_BUZZWOLE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_MEGA_PUNCH); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ABILITY_POPUP(player, ABILITY_ULTRA_SWOLE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Ultra Swole does not raise Defense after a non-punching move")
{
    GIVEN {
        PLAYER(SPECIES_BUZZWOLE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_SWOLE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Ultra Swole does not raise Defense if another Ultra Beast is in the party")
{
    GIVEN {
        PLAYER(SPECIES_BUZZWOLE) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_MEGA_PUNCH); }
        PLAYER(SPECIES_NIHILEGO) { Ability(ABILITY_BEAST_BOOST); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_ULTRA_SWOLE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
    }
}
