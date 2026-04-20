#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAINING_KISS].kissingMove);
    ASSUME(gBattleMoves[MOVE_SWEET_KISS].kissingMove);
    ASSUME(gBattleMoves[MOVE_LOVELY_KISS].kissingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].kissingMove);
    ASSUME(gBattleMoves[MOVE_ATTRACT].effect == EFFECT_ATTRACT);
}

SINGLE_BATTLE_TEST("Heartthrob makes kissing moves use Attract afterwards")
{
    u16 move;

    PARAMETRIZE { move = MOVE_DRAINING_KISS; }
    PARAMETRIZE { move = MOVE_SWEET_KISS; }
    PARAMETRIZE { move = MOVE_LOVELY_KISS; }

    PASSES_RANDOMLY(gBattleMoves[move].accuracy, 100, RNG_ACCURACY);

    GIVEN {
        PLAYER(SPECIES_LUVDISC) { Gender(MON_FEMALE); Ability(ABILITY_SWIFT_SWIM); Moves(move); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, move); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        ABILITY_POPUP(player, ABILITY_HEARTTHROB);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_INFATUATION);
    }
}

SINGLE_BATTLE_TEST("Heartthrob does not trigger for non-kissing moves")
{
    GIVEN {
        PLAYER(SPECIES_LUVDISC) { Gender(MON_FEMALE); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEARTTHROB);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_INFATUATION));
    }
}

SINGLE_BATTLE_TEST("Heartthrob does not trigger if Attract cannot apply")
{
    GIVEN {
        PLAYER(SPECIES_LUVDISC) { Gender(MON_FEMALE); Ability(ABILITY_SWIFT_SWIM); Moves(MOVE_DRAINING_KISS); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAINING_KISS); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEARTTHROB);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ATTRACT, player);
        }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_INFATUATION));
    }
}
