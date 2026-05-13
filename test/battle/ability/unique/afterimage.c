#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_SWIFT].makesContact);
}

SINGLE_BATTLE_TEST("Afterimage prevents contact effects when the user moves before the target")
{
    GIVEN {
        PLAYER(SPECIES_ZERAORA) { Speed(100); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
            MESSAGE("Zeraora was hurt!");
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Afterimage does not prevent contact effects when the target moves first")
{
    GIVEN {
        PLAYER(SPECIES_ZERAORA) { Speed(50); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Afterimage does not change non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_ZERAORA) { Speed(100); Moves(MOVE_SWIFT); }
        OPPONENT(SPECIES_CARVANHA) { Ability(ABILITY_ROUGH_SKIN); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWIFT); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWIFT, player);
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_ROUGH_SKIN);
        }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}
