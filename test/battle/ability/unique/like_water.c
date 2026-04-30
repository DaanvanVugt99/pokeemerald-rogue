#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SACRED_SWORD].type == TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_FIGHTING);
    ASSUME(gBattleMoves[MOVE_WATER_PULSE].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Like Water uses Water Pulse after the first Fighting move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_MIENSHAO) { Ability(ABILITY_REGENERATOR); UniqueAbility(ABILITY_LIKE_WATER); Moves(MOVE_SACRED_SWORD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SACRED_SWORD); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SACRED_SWORD, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_LIKE_WATER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}

SINGLE_BATTLE_TEST("Like Water does not trigger after non-Fighting moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_LIKE_WATER; }

    GIVEN {
        PLAYER(SPECIES_MIENSHAO) { Ability(ABILITY_REGENERATOR); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
