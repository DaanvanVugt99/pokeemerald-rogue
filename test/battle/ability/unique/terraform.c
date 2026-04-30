#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_IRON_HEAD].type == TYPE_STEEL);
}

SINGLE_BATTLE_TEST("Terraform removes terrain before a Ground-type move deals damage")
{
    GIVEN {
        PLAYER(SPECIES_EXCADRILL) { Speed(50); Attack(100); Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_TERRAFORM); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_EARTHQUAKE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ABILITY_POPUP(player, ABILITY_TERRAFORM);
        MESSAGE("The grass disappeared from the battlefield.");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gFieldStatuses & STATUS_FIELD_TERRAIN_ANY, 0);
    }
}

SINGLE_BATTLE_TEST("Terraform boosts the Ground-type move if it removes terrain", s16 damage)
{
    bool32 terrain;

    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_EXCADRILL) { Speed(50); Attack(100); Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_TERRAFORM); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_GRASSY_TERRAIN); }
    } WHEN {
        if (terrain)
            TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
        else
            TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.3), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Terraform preserves critical-hit calculation after removing terrain")
{
    GIVEN {
        PLAYER(SPECIES_EXCADRILL) { Speed(50); Attack(100); Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_TERRAFORM); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_EARTHQUAKE, criticalHit: TRUE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        ABILITY_POPUP(player, ABILITY_TERRAFORM);
        MESSAGE("The grass disappeared from the battlefield.");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Terraform does not remove terrain for non-Ground moves")
{
    GIVEN {
        PLAYER(SPECIES_EXCADRILL) { Speed(50); Attack(100); Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_TERRAFORM); Moves(MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Defense(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_GRASSY_TERRAIN); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_GRASSY_TERRAIN); MOVE(player, MOVE_IRON_HEAD); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRASSY_TERRAIN, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_TERRAFORM);
            MESSAGE("The grass disappeared from the battlefield.");
        }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_HEAD, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRASSY_TERRAIN);
    }
}
