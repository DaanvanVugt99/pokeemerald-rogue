#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_CONFUSION].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Mood Swing can become Elated at end of turn")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_MOOD_SWING);
    GIVEN {
        PLAYER(SPECIES_SWOOBAT) { Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_MOOD_SWING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOOD_SWING);
        MESSAGE("Swoobat became elated!");
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Mood Swing can become Anxious at end of turn")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_MOOD_SWING);
    GIVEN {
        PLAYER(SPECIES_SWOOBAT) { Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_MOOD_SWING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOOD_SWING);
        MESSAGE("Swoobat became anxious!");
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniquePersistentStateActive);
    }
}

SINGLE_BATTLE_TEST("Mood Swing's Elated state boosts Sp. Atk", s16 damage)
{
    bool32 elated;

    PARAMETRIZE { elated = FALSE; }
    PARAMETRIZE { elated = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SWOOBAT) { Speed(100); SpAttack(100); Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_MOOD_SWING); Moves(MOVE_CELEBRATE, MOVE_CONFUSION); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); SpDefense(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_MOOD_SWING, elated)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CONFUSION, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOOD_SWING);
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Mood Swing's Anxious state boosts Speed")
{
    PASSES_RANDOMLY(1, 2, RNG_ROGUE_MOOD_SWING);
    GIVEN {
        PLAYER(SPECIES_SWOOBAT) { Speed(50); Ability(ABILITY_UNAWARE); UniqueAbility(ABILITY_MOOD_SWING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(60); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_MOOD_SWING);
        MESSAGE("Swoobat became anxious!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}
