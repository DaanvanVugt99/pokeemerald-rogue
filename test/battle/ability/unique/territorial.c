#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_PLAIN_TERRAIN].effect == EFFECT_PLAIN_TERRAIN);
    ASSUME(gBattleMoves[MOVE_AERIAL_ACE].type == TYPE_FLYING);
}

SINGLE_BATTLE_TEST("Territorial doubles damage only when the target is terrain-affected", s16 damage)
{
    u16 targetAbility;
    PARAMETRIZE { targetAbility = ABILITY_LEVITATE; }
    PARAMETRIZE { targetAbility = ABILITY_SHADOW_TAG; }
    GIVEN {
        PLAYER(SPECIES_SPEAROW) { Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_GRASSY_TERRAIN, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(targetAbility); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GRASSY_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Territorial uses Focus Energy after Flying-type moves in Plain Terrain")
{
    GIVEN {
        PLAYER(SPECIES_FEAROW) { Speed(50); Ability(ABILITY_KEEN_EYE); UniqueAbility(ABILITY_TERRITORIAL); Moves(MOVE_PLAIN_TERRAIN, MOVE_AERIAL_ACE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PLAIN_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_AERIAL_ACE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AERIAL_ACE, player);
        ABILITY_POPUP(player, ABILITY_TERRITORIAL);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FOCUS_ENERGY, player);
    } THEN {
        EXPECT(gBattleMons[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].status2 & STATUS2_FOCUS_ENERGY);
    }
}
