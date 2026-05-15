#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SCRATCH].power > 0);
    ASSUME(gBattleMoves[MOVE_SCRATCH].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_ABSORB].power > 0);
    ASSUME(gBattleMoves[MOVE_ABSORB].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].power > 0);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
    ASSUME(gBattleMoves[MOVE_ROCK_SLIDE].power > 0);
    ASSUME(gBattleMoves[MOVE_ROCK_SLIDE].accuracy < 100);
    ASSUME(gBattleMoves[MOVE_DOUBLE_EDGE].effect == EFFECT_RECOIL_33);
}

SINGLE_BATTLE_TEST("Fossil Memory on Dracozolt chooses from Draco and Zolt memories")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        MESSAGE("Dracozolt's fossil memory awakened!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPARK, player);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_SPARK);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory on Arctozolt chooses from Arcto and Zolt memories")
{
    GIVEN {
        PLAYER(SPECIES_ARCTOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_AURORA_BEAM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AURORA_BEAM, player);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_AURORA_BEAM);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory on Dracovish chooses from Draco and Vish memories")
{
    GIVEN {
        PLAYER(SPECIES_DRACOVISH) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_WHIRLPOOL)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WHIRLPOOL, player);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_WHIRLPOOL);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory on Arctovish chooses from Arcto and Vish memories")
{
    GIVEN {
        PLAYER(SPECIES_ARCTOVISH) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_AQUA_JET)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AQUA_JET, player);
    } THEN {
        EXPECT_EQ(gCalledMove, MOVE_AQUA_JET);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory can trigger on repeated resisted moves without switching")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_THUNDER_FANG)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SPARK, player);
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_FANG, player);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not trigger on neutral damage")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SCRATCH, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not trigger on super effective damage")
{
    GIVEN {
        PLAYER(SPECIES_DRACOVISH) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_WATER_GUN); }
        OPPONENT(SPECIES_GEODUDE) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_WHIRLPOOL)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WATER_GUN, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not trigger on immunity")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_GASTLY) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not trigger on miss")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_ROCK_SLIDE); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ROCK_SLIDE, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            HP_BAR(opponent);
            ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        }
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not trigger when blocked by Protect")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            HP_BAR(opponent);
            ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        }
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not recurse from a resisted memory move")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_TWISTER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TWISTER, player);
        NOT ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory is suppressed by Neutralizing Gas")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); Moves(MOVE_ABSORB); }
        OPPONENT(SPECIES_KOFFING) { Ability(ABILITY_NEUTRALIZING_GAS); HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ABSORB, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ABSORB, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
    }
}

SINGLE_BATTLE_TEST("Fossil Memory does not trigger if the user faints from recoil first")
{
    GIVEN {
        PLAYER(SPECIES_DRACOZOLT) { Ability(ABILITY_NONE); UniqueAbility(ABILITY_FOSSIL_MEMORY); HP(1); MaxHP(100); Moves(MOVE_DOUBLE_EDGE); }
        OPPONENT(SPECIES_ARON) { HP(1000); MaxHP(1000); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DOUBLE_EDGE, WITH_RNG(RNG_ROGUE_FOSSIL_MEMORY, MOVE_SPARK)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DOUBLE_EDGE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_FOSSIL_MEMORY);
    }
}
