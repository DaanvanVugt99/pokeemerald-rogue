#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
}

SINGLE_BATTLE_TEST("Updraft triggers only on the first Fire-type move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_UPDRAFT); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        ABILITY_POPUP(player, ABILITY_UPDRAFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UPDRAFT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        }
    }
}

SINGLE_BATTLE_TEST("Updraft does not refresh after the user switches out and back in")
{
    GIVEN {
        PLAYER(SPECIES_CHARIZARD) { Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_UPDRAFT); Moves(MOVE_EMBER, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_UPDRAFT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_UPDRAFT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
        }
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
