#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Battle Formation gives +1 priority to status moves when the user has exactly one status move")
{
    GIVEN {
        PLAYER(SPECIES_LEDIAN)      { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_BATTLE_FORMATION); Moves(MOVE_TACKLE, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Battle Formation does not give +1 priority when the user has more than one status move")
{
    GIVEN {
        PLAYER(SPECIES_LEDIAN)      { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ABILITY_BATTLE_FORMATION); Moves(MOVE_TACKLE, MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
    }
}

SINGLE_BATTLE_TEST("Battle Formation reduces incoming damage by 20% when the user has exactly one status move", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_BATTLE_FORMATION; }
    PARAMETRIZE { ability = ABILITY_PRESSURE; }

    GIVEN {
        PLAYER(SPECIES_LEDIAN)      { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ability); Moves(MOVE_TACKLE, MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(0.8), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Battle Formation does not reduce damage when the user has more than one status move", s16 damage)
{
    u16 ability;

    PARAMETRIZE { ability = ABILITY_BATTLE_FORMATION; }
    PARAMETRIZE { ability = ABILITY_PRESSURE; }

    GIVEN {
        PLAYER(SPECIES_LEDIAN)      { Speed(50); Ability(ABILITY_SWARM); UniqueAbility(ability); Moves(MOVE_TACKLE, MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}
