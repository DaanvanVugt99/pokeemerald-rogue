#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_IRON_HEAD].type == TYPE_STEEL);
    ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_SPIKES].effect == EFFECT_SPIKES);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].effect == EFFECT_STEALTH_ROCK);
    ASSUME(gBattleMoves[MOVE_STEALTH_ROCK].type == TYPE_ROCK);
}

SINGLE_BATTLE_TEST("Iron Stamp activates Spikes and Stealth Rock after a Steel-type move")
{
    s16 moveDamage;
    s16 hazardDamage;

    GIVEN {
        PLAYER(SPECIES_CUFANT) { Attack(1); Ability(ABILITY_SHEER_FORCE); UniqueAbility(ABILITY_IRON_STAMP); Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STEALTH_ROCK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_HEAD, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Spikes were scattered all around the opposing team!");
        MESSAGE("Pointed stones float in the air around the opposing team!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_HEAD, player);
        HP_BAR(opponent, captureDamage: &moveDamage);
        ABILITY_POPUP(player, ABILITY_IRON_STAMP);
        HP_BAR(opponent, captureDamage: &hazardDamage);
        MESSAGE("Foe Wobbuffet is hurt by spikes!");
    } THEN {
        EXPECT_GT(moveDamage, 0);
        EXPECT_EQ(hazardDamage, 200);
    }
}

SINGLE_BATTLE_TEST("Iron Stamp does not activate hazards after non-Steel moves")
{
    GIVEN {
        PLAYER(SPECIES_COPPERAJAH) { Attack(1); Ability(ABILITY_SHEER_FORCE); UniqueAbility(ABILITY_IRON_STAMP); Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(800); MaxHP(800); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STEALTH_ROCK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_IRON_STAMP);
            MESSAGE("Foe Wobbuffet is hurt by the spikes!");
        }
    }
}

SINGLE_BATTLE_TEST("Iron Stamp uses Stealth Rock if the target is not grounded for Spikes")
{
    s16 hazardDamage;

    GIVEN {
        PLAYER(SPECIES_COPPERAJAH) { Attack(1); Ability(ABILITY_SHEER_FORCE); UniqueAbility(ABILITY_IRON_STAMP); Moves(MOVE_SPIKES, MOVE_STEALTH_ROCK, MOVE_IRON_HEAD); }
        OPPONENT(SPECIES_PIDGEY) { HP(800); MaxHP(800); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPIKES); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_STEALTH_ROCK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_IRON_HEAD, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_IRON_STAMP);
        HP_BAR(opponent, captureDamage: &hazardDamage);
        MESSAGE("Pointed stones dug into Foe Pidgey!");
    } THEN {
        EXPECT_EQ(hazardDamage, 200);
    }
}
