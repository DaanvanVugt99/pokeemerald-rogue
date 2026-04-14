#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
    ASSUME(gBattleMoves[MOVE_X_SCISSOR].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_X_SCISSOR].power > 20);
    ASSUME(gBattleMoves[MOVE_INFESTED_TERRAIN].effect == EFFECT_INFESTED_TERRAIN);
}

SINGLE_BATTLE_TEST("Swarm Assault adds an extra Bug-type hit after contact moves")
{
    s16 firstHit;
    s16 secondHit;

    GIVEN {
        PLAYER(SPECIES_SCYTHER) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_SWARM_ASSAULT); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &firstHit);
        ABILITY_POPUP(player, ABILITY_SWARM_ASSAULT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_X_SCISSOR, player);
        HP_BAR(opponent, captureDamage: &secondHit);
    } THEN {
        EXPECT_GT(firstHit, 0);
        EXPECT_GT(secondHit, 0);
    }
}

SINGLE_BATTLE_TEST("Swarm Assault does not trigger after non-contact moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SWARM_ASSAULT; }

    GIVEN {
        PLAYER(SPECIES_SCYTHER) { Ability(ABILITY_SWARM); UniqueAbility(uniqueAbility); Moves(MOVE_EMBER); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Swarm Assault's extra hit is stronger in Infested Terrain", s16 swarmDamage)
{
    u16 setupMove;
    PARAMETRIZE { setupMove = MOVE_CELEBRATE; }
    PARAMETRIZE { setupMove = MOVE_INFESTED_TERRAIN; }

    GIVEN {
        PLAYER(SPECIES_SCYTHER) { Ability(ABILITY_SWARM); UniqueAbility(ABILITY_SWARM_ASSAULT); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_BATTLE_ARMOR); Moves(setupMove, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, setupMove); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SWARM_ASSAULT);
        HP_BAR(opponent, captureDamage: &results[i].swarmDamage);
    } FINALLY {
        EXPECT_GT(results[1].swarmDamage, results[0].swarmDamage);
    }
}
