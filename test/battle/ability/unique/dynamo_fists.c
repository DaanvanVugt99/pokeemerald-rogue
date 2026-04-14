#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDERBOLT].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_THUNDER_WAVE].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_MEGA_PUNCH].punchingMove);
    ASSUME(!gBattleMoves[MOVE_TACKLE].punchingMove);
}

SINGLE_BATTLE_TEST("Dynamo Fists gives +1 priority to the next punching move after an Electric move")
{
    GIVEN {
        PLAYER(SPECIES_ELECTABUZZ) { Speed(50); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_DYNAMO_FISTS); Moves(MOVE_THUNDERBOLT, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERBOLT, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Dynamo Fists priority is consumed after the next punching move")
{
    GIVEN {
        PLAYER(SPECIES_ELECTABUZZ) { Speed(50); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_DYNAMO_FISTS); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ELECTRIC_TERRAIN); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ELECTRIC_TERRAIN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
    }
}

SINGLE_BATTLE_TEST("Dynamo Fists does not consume priority on non-punching moves")
{
    GIVEN {
        PLAYER(SPECIES_ELECTABUZZ) { Speed(50); Ability(ABILITY_STATIC); UniqueAbility(ABILITY_DYNAMO_FISTS); Moves(MOVE_THUNDERBOLT, MOVE_TACKLE, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDERBOLT); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEGA_PUNCH, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Dynamo Fists boosts that punching move by 1.2x in Electric Terrain", s16 punchDamage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_ELECTRIC_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_ELECTABUZZ) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_DYNAMO_FISTS); Moves(MOVE_THUNDER_WAVE, MOVE_MEGA_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_WAVE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_MEGA_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].punchDamage);
    } FINALLY {
        EXPECT_GT(results[1].punchDamage, results[0].punchDamage * 11 / 10);
    }
}
