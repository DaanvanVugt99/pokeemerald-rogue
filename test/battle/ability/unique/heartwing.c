#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_CLAW].type == TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Heartwing sets Misty Terrain on switch-in if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_LATIAS)      { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_HEARTWING); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GARDEVOIR)   { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_ALTARIA)     { Ability(ABILITY_NATURAL_CURE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN);
        EXPECT(!(gFieldStatuses & STATUS_FIELD_TERRAIN_PERMANENT));
    }
}

SINGLE_BATTLE_TEST("Heartwing does not set Misty Terrain on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_LATIAS)      { Ability(ABILITY_LEVITATE); UniqueAbility(ABILITY_HEARTWING); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_GARDEVOIR)   { Ability(ABILITY_SYNCHRONIZE); }
        PLAYER(SPECIES_PIKACHU)     { Ability(ABILITY_STATIC); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT(!(gFieldStatuses & STATUS_FIELD_MISTY_TERRAIN));
    }
}

DOUBLE_BATTLE_TEST("Heartwing lets allied Dragon moves ignore Misty Terrain's damage reduction", s16 damage)
{
    u32 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_FREESTYLE; }
    PARAMETRIZE { uniqueAbility = ABILITY_HEARTWING; }

    GIVEN {
        PLAYER(SPECIES_GARCHOMP) { Speed(100); Moves(MOVE_DRAGON_CLAW, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); UniqueAbility(uniqueAbility); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(120); Moves(MOVE_MISTY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN {
            MOVE(playerLeft, MOVE_CELEBRATE);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_MISTY_TERRAIN);
        }
        TURN {
            MOVE(playerLeft, MOVE_DRAGON_CLAW, target: opponentLeft);
            MOVE(playerRight, MOVE_CELEBRATE);
            MOVE(opponentLeft, MOVE_CELEBRATE);
            MOVE(opponentRight, MOVE_CELEBRATE);
        }
    } SCENE {
        HP_BAR(opponentLeft, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(2.0), results[1].damage);
    }
}
