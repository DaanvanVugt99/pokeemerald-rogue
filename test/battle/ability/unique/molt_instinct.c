#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_DRAGON_RAGE].effect == EFFECT_DRAGON_RAGE);
    ASSUME(gBattleMoves[MOVE_HARDEN].effect == EFFECT_DEFENSE_UP);
}

SINGLE_BATTLE_TEST("Molt Instinct cures status and uses Harden when Slither Wing is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SLITHER_WING) { HP(100); MaxHP(160); Speed(200); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_GREAT_TUSK) { Speed(50); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        if (hasOtherParadox)
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_MOLT_INSTINCT);
                ANIMATION(ANIM_TYPE_MOVE, MOVE_HARDEN, player);
            }
        }
        else
        {
            ABILITY_POPUP(player, ABILITY_MOLT_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HARDEN, player);
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_EQ(player->status1, STATUS1_BURN);
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE);
            EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
        }
        else
        {
            EXPECT_EQ(player->status1, STATUS1_NONE);
            EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
            EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
        }
    }
}

SINGLE_BATTLE_TEST("Molt Instinct only triggers once each battle")
{
    GIVEN {
        PLAYER(SPECIES_SLITHER_WING) { HP(100); MaxHP(160); Speed(200); Status1(STATUS1_BURN); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MOLT_INSTINCT);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HARDEN, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_MOLT_INSTINCT);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HARDEN, player);
        }
    } THEN {
        EXPECT_EQ(player->hp, 20);
        EXPECT_EQ(player->status1, STATUS1_NONE);
        EXPECT_EQ(player->statStages[STAT_DEF], DEFAULT_STAT_STAGE + 1);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
