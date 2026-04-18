#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_TACKLE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_POUND].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_ECLIPSE].effect == EFFECT_ECLIPSE);
}

SINGLE_BATTLE_TEST("Shadow Carapace lowers the attacker's Sp. Def when hit by a special move")
{
    GIVEN {
        PLAYER(SPECIES_UMBREON) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_SHADOW_CARAPACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE - 1);
    }
}

SINGLE_BATTLE_TEST("Shadow Carapace does not lower Sp. Def when hit by a physical move")
{
    GIVEN {
        PLAYER(SPECIES_UMBREON) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_SHADOW_CARAPACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPDEF], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Shadow Carapace gains 20 percent evasion in Eclipse")
{
    PASSES_RANDOMLY(1, 5, RNG_ACCURACY);
    GIVEN {
        PLAYER(SPECIES_UMBREON) { Ability(ABILITY_SYNCHRONIZE); UniqueAbility(ABILITY_SHADOW_CARAPACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ECLIPSE, MOVE_POUND); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_ECLIPSE); }
        TURN { MOVE(opponent, MOVE_POUND); }
    } SCENE {
        NONE_OF {
            HP_BAR(player);
        }
    }
}
