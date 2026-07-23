#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_TACKLE].strikeCount < 2);
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
}

SINGLE_BATTLE_TEST("Impact Protocol makes Iron Hands's first damaging move hit twice if it is the only Paradox", s16 hit1, s16 hit2)
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_HANDS) { Level(100); Attack(300); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_BUNDLE) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Defense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent, captureDamage: &results[i].hit1);
        if (!hasOtherParadox)
            HP_BAR(opponent, captureDamage: &results[i].hit2);
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT_GT(results[i].hit2, 0);
            EXPECT_MUL_EQ(results[i].hit1, UQ_4_12(0.2), results[i].hit2);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}

SINGLE_BATTLE_TEST("Impact Protocol only affects the first damaging move each switch-in")
{
    GIVEN {
        PLAYER(SPECIES_IRON_HANDS) { Level(100); Attack(300); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Defense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            HP_BAR(opponent);
        }
    } THEN {
        EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
    }
}
