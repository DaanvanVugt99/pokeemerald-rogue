#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_QUICK_ATTACK].priority > 0);
    ASSUME(gBattleMoves[MOVE_U_TURN].effect == EFFECT_HIT_ESCAPE);
    ASSUME(gBattleMoves[MOVE_TACKLE].priority == 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].effect == EFFECT_HIT);
}

SINGLE_BATTLE_TEST("Singularity Airspace blocks opposing priority moves if Iron Jugulis is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_JUGULIS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_BUNDLE) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_SINGULARITY_AIRSPACE);
            NOT { HP_BAR(player); }
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
            HP_BAR(player);
        }
    } THEN {
        if (hasOtherParadox)
            EXPECT_LT(player->hp, player->maxHP);
        else
            EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Singularity Airspace blocks opposing switching moves if Iron Jugulis is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_JUGULIS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_HANDS) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_U_TURN); }
        OPPONENT(SPECIES_WYNAUT) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        if (hasOtherParadox)
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_U_TURN); SEND_OUT(opponent, 1); }
        else
            TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_U_TURN); }
    } SCENE {
        if (!hasOtherParadox)
        {
            ABILITY_POPUP(player, ABILITY_SINGULARITY_AIRSPACE);
            NONE_OF {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponent);
                HP_BAR(player);
                MESSAGE("2 sent out Wynaut!");
            }
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_U_TURN, opponent);
            HP_BAR(player);
            MESSAGE("2 sent out Wynaut!");
        }
    } THEN {
        if (hasOtherParadox)
        {
            EXPECT_EQ(opponent->species, SPECIES_WYNAUT);
            EXPECT_LT(player->hp, player->maxHP);
        }
        else
        {
            EXPECT_EQ(opponent->species, SPECIES_WOBBUFFET);
            EXPECT_EQ(player->hp, player->maxHP);
        }
    }
}

SINGLE_BATTLE_TEST("Singularity Airspace does not block ordinary opposing moves")
{
    GIVEN {
        PLAYER(SPECIES_IRON_JUGULIS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        HP_BAR(player);
        NOT { ABILITY_POPUP(player, ABILITY_SINGULARITY_AIRSPACE); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Singularity Airspace is ignored by Mold Breaker")
{
    GIVEN {
        PLAYER(SPECIES_IRON_JUGULIS) { HP(200); MaxHP(200); Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PINSIR) { Speed(50); Ability(ABILITY_MOLD_BREAKER); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_MOLD_BREAKER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, opponent);
        HP_BAR(player);
        NOT { ABILITY_POPUP(player, ABILITY_SINGULARITY_AIRSPACE); }
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
    }
}
