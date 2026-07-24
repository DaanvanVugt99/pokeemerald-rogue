#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHADOW_BALL].type == TYPE_GHOST);
    ASSUME(gSpeciesInfo[SPECIES_GARDEVOIR].types[0] == TYPE_PSYCHIC);
    ASSUME(gSpeciesInfo[SPECIES_GARDEVOIR].types[1] == TYPE_FAIRY);
}

SINGLE_BATTLE_TEST("Illusion display typing hides the user's real type and effectiveness")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_ZOROARK_HISUIAN) { Ability(ABILITY_ILLUSION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_GARDEVOIR);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        u8 types[3];
        u32 playerBattler = GetBattlerAtPosition(B_POSITION_PLAYER_LEFT);
        u32 opponentBattler = GetBattlerAtPosition(B_POSITION_OPPONENT_LEFT);

        EXPECT(GetIllusionMonTypes(opponentBattler, types));
        EXPECT_EQ(types[0], TYPE_PSYCHIC);
        EXPECT_EQ(types[1], TYPE_FAIRY);
        EXPECT_EQ(types[2], TYPE_MYSTERY);
        EXPECT_EQ(
            CalcTypeEffectivenessMultiplierForUI(
                MOVE_SHADOW_BALL,
                TYPE_GHOST,
                playerBattler,
                opponentBattler,
                ABILITY_NONE,
                FALSE),
            UQ_4_12(2.0));
        EXPECT_EQ(
            CalcTypeEffectivenessMultiplier(
                MOVE_SHADOW_BALL,
                TYPE_GHOST,
                playerBattler,
                opponentBattler,
                GetBattlerAbility(opponentBattler),
                FALSE),
            UQ_4_12(0.0));
    }
}
