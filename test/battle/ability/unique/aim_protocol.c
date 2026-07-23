#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYDRO_PUMP].accuracy < 100);
    ASSUME(gBattleMoves[MOVE_HYDRO_PUMP].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_PROTECT].effect == EFFECT_PROTECT);
}

SINGLE_BATTLE_TEST("Aim Protocol makes Iron Bundle's moves never miss if it is the only Paradox")
{
    bool32 hasOtherParadox;

    PARAMETRIZE { hasOtherParadox = FALSE; }
    PARAMETRIZE { hasOtherParadox = TRUE; }

    GIVEN {
        PLAYER(SPECIES_IRON_BUNDLE) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_HYDRO_PUMP); }
        if (hasOtherParadox)
            PLAYER(SPECIES_IRON_TREADS) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
        else
            PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); MaxHP(200); Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (!hasOtherParadox)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
            HP_BAR(opponent);
            NOT MESSAGE("Iron Bundle's attack missed!");
        }
        else
        {
            NONE_OF {
                ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
                HP_BAR(opponent);
            }
        }
    } THEN {
        if (hasOtherParadox)
            EXPECT_EQ(opponent->hp, opponent->maxHP);
        else
            EXPECT_LT(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Aim Protocol does not bypass Protect")
{
    GIVEN {
        PLAYER(SPECIES_IRON_BUNDLE) { Speed(50); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_HYDRO_PUMP); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_HYDRO_PUMP, WITH_RNG(RNG_ACCURACY, FALSE)); }
    } SCENE {
        MESSAGE("Foe Wobbuffet protected itself!");
        NONE_OF {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_HYDRO_PUMP, player);
            HP_BAR(opponent);
            MESSAGE("Iron Bundle's attack missed!");
        }
    }
}
