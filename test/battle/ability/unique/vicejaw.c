#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BITE].power > 0);
    ASSUME(gBattleMoves[MOVE_VISE_GRIP].power > 0);
    ASSUME(gBattleMoves[MOVE_CRUNCH].power > 0);
    ASSUME(gBattleMoves[MOVE_FIRE_FANG].power > 0);
}

SINGLE_BATTLE_TEST("Vicejaw uses a random jaw move after Bite")
{
    GIVEN {
        PLAYER(SPECIES_MAWILE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_VICEJAW); Moves(MOVE_BITE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_BITE, WITH_RNG(RNG_ROGUE_VICEJAW, MOVE_CRUNCH)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BITE, player);
        ABILITY_POPUP(player, ABILITY_VICEJAW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CRUNCH, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp < gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].maxHP);
    }
}

SINGLE_BATTLE_TEST("Vicejaw uses a random jaw move after Vice Grip")
{
    GIVEN {
        PLAYER(SPECIES_MAWILE) { Ability(ABILITY_HYPER_CUTTER); UniqueAbility(ABILITY_VICEJAW); Moves(MOVE_VISE_GRIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_VISE_GRIP, WITH_RNG(RNG_ROGUE_VICEJAW, MOVE_FIRE_FANG)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VISE_GRIP, player);
        ABILITY_POPUP(player, ABILITY_VICEJAW);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FIRE_FANG, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].hp < gBattleMons[GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT)].maxHP);
    }
}
