#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SHADOW_FORCE].effect == EFFECT_SEMI_INVULNERABLE);
    ASSUME(gBattleMoves[MOVE_GRAVITY].effect == EFFECT_GRAVITY);
    ASSUME(gBattleMoves[MOVE_OMINOUS_WIND].power > 0);
}

SINGLE_BATTLE_TEST("Dark Dimension uses a random dimension move after Shadow Force")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Item(ITEM_POWER_HERB); Moves(MOVE_SHADOW_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_FORCE, WITH_RNG(RNG_ROGUE_DARK_DIMENSION, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_FORCE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_GRAVITY);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Dark Dimension triggers after Shadow Force hits without Power Herb")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Moves(MOVE_SHADOW_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_FORCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SKIP_TURN(player); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        MESSAGE("Giratina used Shadow Force!");
        MESSAGE("Giratina used Shadow Force!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_FORCE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MEAN_LOOK, player);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Dark Dimension can choose an attacking dimension move after Shadow Force")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Item(ITEM_POWER_HERB); Moves(MOVE_SHADOW_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_FORCE, WITH_RNG(RNG_ROGUE_DARK_DIMENSION, MOVE_OMINOUS_WIND)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_FORCE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_OMINOUS_WIND, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_LT(opponent->hp, opponent->maxHP);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Dark Dimension can choose every dimension move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_TRICK_ROOM,
        MOVE_MAGIC_ROOM,
        MOVE_WONDER_ROOM,
        MOVE_GRAVITY,
        MOVE_CURSE,
        MOVE_SPITE,
        MOVE_OMINOUS_WIND,
        MOVE_MEAN_LOOK,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_DARK_DIMENSION);

    GIVEN {
        PLAYER(SPECIES_GIRATINA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Item(ITEM_POWER_HERB); Moves(MOVE_SHADOW_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_FORCE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_FORCE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Dark Dimension does not trigger after other moves")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Moves(MOVE_SHADOW_BALL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_BALL, WITH_RNG(RNG_ROGUE_DARK_DIMENSION, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SHADOW_BALL, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

SINGLE_BATTLE_TEST("Dark Dimension only triggers once each battle")
{
    GIVEN {
        PLAYER(SPECIES_GIRATINA) { Speed(100); Ability(ABILITY_PRESSURE); UniqueAbility(ABILITY_DARK_DIMENSION); Item(ITEM_POWER_HERB); Moves(MOVE_SHADOW_FORCE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SHADOW_FORCE, WITH_RNG(RNG_ROGUE_DARK_DIMENSION, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SHADOW_FORCE, WITH_RNG(RNG_ROGUE_DARK_DIMENSION, MOVE_GRAVITY)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { SKIP_TURN(player); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GRAVITY, player);
        NOT ABILITY_POPUP(player, ABILITY_DARK_DIMENSION);
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
