#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SPARK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_WATER_PULSE].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_RAIN_DANCE].effect == EFFECT_RAIN_DANCE);
}

SINGLE_BATTLE_TEST("Electrocytes gives Electric-type moves STAB", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_ELECTROCYTES; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Attack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(uniqueAbility); Moves(MOVE_SPARK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Defense(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SPARK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Electrocytes gives Water-type moves a 30 percent chance to paralyze in rain")
{
    PASSES_RANDOMLY(3, 10, RNG_SECONDARY_EFFECT);
    GIVEN {
        PLAYER(SPECIES_WHISCASH) { Ability(ABILITY_OBLIVIOUS); Moves(MOVE_CELEBRATE, MOVE_WATER_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_RAIN_DANCE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_RAIN_DANCE); }
        TURN { MOVE(player, MOVE_WATER_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(opponent->status1 & STATUS1_PARALYSIS, 0);
    }
}

SINGLE_BATTLE_TEST("Electrocytes does not paralyze from Water-type moves outside rain")
{
    GIVEN {
        PLAYER(SPECIES_WHISCASH) { Ability(ABILITY_OBLIVIOUS); Moves(MOVE_WATER_PULSE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_PULSE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_PULSE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ELECTROCYTES);
            MESSAGE("Foe Wobbuffet is paralyzed! It may be unable to move!");
        }
    }
}
