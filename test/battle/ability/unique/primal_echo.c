#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].power > 40);
    ASSUME(!gBattleMoves[MOVE_TACKLE].soundMove);
    ASSUME(gItems[ITEM_LIFE_ORB].holdEffect == HOLD_EFFECT_LIFE_ORB);
}

SINGLE_BATTLE_TEST("Primal Echo only repeats a damaging sound move when Scream Tail is the only Paradox")
{
    bool32 hasOtherParadox;
    s16 echoHit;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_SCREAM_TAIL) { Level(100); Speed(100); SpAttack(300); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_HYPER_VOICE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_GREAT_TUSK) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); HP(2000); MaxHP(2000); Speed(50); SpDefense(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_PRIMAL_ECHO);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
            HP_BAR(opponent, captureDamage: &echoHit);
        }
        else
        {
            NONE_OF {
                ABILITY_POPUP(player, ABILITY_PRIMAL_ECHO);
            }
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
        }
        else
        {
            EXPECT_EQ(gCalledMove, MOVE_HYPER_VOICE);
            EXPECT_GT(echoHit, 0);
            EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
        }
    }
}

SINGLE_BATTLE_TEST("Primal Echo only triggers once per battle and not from the echoed sound move")
{
    GIVEN {
        PLAYER(SPECIES_SCREAM_TAIL) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_HYPER_VOICE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPER_VOICE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_PRIMAL_ECHO);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PRIMAL_ECHO);
        }
    } THEN {
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Primal Echo does not trigger after non-sound moves")
{
    GIVEN {
        PLAYER(SPECIES_SCREAM_TAIL) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        HP_BAR(opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_PRIMAL_ECHO);
        }
    } THEN {
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}

SINGLE_BATTLE_TEST("Primal Echo does not trigger if pending Life Orb damage would faint the user")
{
    GIVEN {
        PLAYER(SPECIES_SCREAM_TAIL) { MaxHP(100); HP(1); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Item(ITEM_LIFE_ORB); Moves(MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(2000); MaxHP(2000); Speed(200); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_HYPER_VOICE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        HP_BAR(opponent);
        NOT ABILITY_POPUP(player, ABILITY_PRIMAL_ECHO);
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_EQ(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0], 0);
    }
}
