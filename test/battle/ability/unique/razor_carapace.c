#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
}

SINGLE_BATTLE_TEST("Razor Carapace only damages contact attackers when the user has no attacking moves", s16 contactRecoil)
{
    u16 move4;

    PARAMETRIZE { move4 = MOVE_PROTECT; }
    PARAMETRIZE { move4 = MOVE_TACKLE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_RAZOR_CARAPACE); Moves(MOVE_CELEBRATE, MOVE_TOXIC, MOVE_RECOVER, move4); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        results[i].contactRecoil = gBattleMons[B_POSITION_OPPONENT_LEFT].maxHP - gBattleMons[B_POSITION_OPPONENT_LEFT].hp;
    } FINALLY {
        EXPECT_GT(results[0].contactRecoil, 0);
        EXPECT_EQ(results[1].contactRecoil, 0);
    }
}

SINGLE_BATTLE_TEST("Razor Carapace recoil is stronger than Iron Barbs alone when gated on", s16 contactRecoil)
{
    u16 ability;
    u16 uniqueAbility;

    PARAMETRIZE { ability = ABILITY_IRON_BARBS; uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { ability = ABILITY_SHADOW_TAG; uniqueAbility = ABILITY_RAZOR_CARAPACE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ability); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE, MOVE_TOXIC, MOVE_RECOVER, MOVE_PROTECT); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        results[i].contactRecoil = gBattleMons[B_POSITION_OPPONENT_LEFT].maxHP - gBattleMons[B_POSITION_OPPONENT_LEFT].hp;
    } FINALLY {
        EXPECT_GT(results[0].contactRecoil, 0);
        EXPECT_MUL_EQ(results[0].contactRecoil, Q_4_12(1.5), results[1].contactRecoil);
    }
}
