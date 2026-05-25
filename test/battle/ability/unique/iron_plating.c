#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_IRON_DEFENSE].effect == EFFECT_DEFENSE_UP_2);
    ASSUME(gBattleMoves[MOVE_SMACK_DOWN].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_SMACK_DOWN].accuracy == 100);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Iron Plating uses Iron Defense the first time Orthworm takes physical damage each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_ORTHWORM) { HP(200); MaxHP(200); Defense(50); Speed(1); Ability(ABILITY_EARTH_EATER); UniqueAbility(ABILITY_IRON_PLATING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Attack(200); Speed(100); Moves(MOVE_SMACK_DOWN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SMACK_DOWN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SMACK_DOWN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SMACK_DOWN, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_IRON_PLATING);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        NOT ABILITY_POPUP(player, ABILITY_IRON_PLATING);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Iron Plating does not trigger from special damage")
{
    GIVEN {
        PLAYER(SPECIES_ORTHWORM) { HP(100); MaxHP(100); Defense(50); SpDefense(50); Speed(1); Ability(ABILITY_EARTH_EATER); UniqueAbility(ABILITY_IRON_PLATING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { SpAttack(200); Speed(100); Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_IRON_PLATING);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
