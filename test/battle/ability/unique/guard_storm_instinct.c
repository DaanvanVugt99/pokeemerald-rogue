#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_DRAGON_PULSE].type == TYPE_DRAGON);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_BURNING_BULWARK].effect == EFFECT_PROTECT);
    ASSUME(gBattleMoves[MOVE_THUNDERCLAP].effect == EFFECT_SUCKER_PUNCH);
    ASSUME(gBattleMoves[MOVE_THUNDERCLAP].type == TYPE_ELECTRIC);
}

SINGLE_BATTLE_TEST("Guard Instinct uses Burning Bulwark after Gouging Fire's first Fire move if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_GOUGING_FIRE) { HP(500); MaxHP(500); Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_EMBER); }
        if (hasOtherParadox)
            PLAYER(SPECIES_SCREAM_TAIL) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(500); MaxHP(500); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_EMBER); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_GUARD_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_BULWARK, player);
            NOT HP_BAR(player);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_GUARD_INSTINCT);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_BURNING_BULWARK, player);
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_LT(player->hp, player->maxHP);
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT_EQ(player->hp, player->maxHP);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}

SINGLE_BATTLE_TEST("Storm Instinct uses Thunderclap after Raging Bolt's first Dragon move if it is the only Paradox")
{
    bool32 hasOtherParadox;
    s16 thunderclapDamage;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_RAGING_BOLT) { Level(100); SpAttack(300); Speed(100); Ability(ABILITY_PROTOSYNTHESIS); Moves(MOVE_DRAGON_PULSE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_GREAT_TUSK) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); SpDefense(100); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_PULSE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_PULSE, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_STORM_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERCLAP, player);
            HP_BAR(opponent, captureDamage: &thunderclapDamage);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_STORM_INSTINCT);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDERCLAP, player);
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT(!gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
        else
        {
            EXPECT_GT(thunderclapDamage, 0);
            EXPECT(gDisableStructs[GetBattlerAtPosition(B_POSITION_PLAYER_LEFT)].uniqueOncePerSwitchInUsed);
        }
    }
}
