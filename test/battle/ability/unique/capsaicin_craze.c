#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_LEAFAGE].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_LEAFAGE].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].type == TYPE_GRASS);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].split == SPLIT_SPECIAL);
    ASSUME(gBattleMoves[MOVE_FIRE_PUNCH].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_FIRE_PUNCH].split == SPLIT_PHYSICAL);
    ASSUME(gBattleMoves[MOVE_EMBER].type == TYPE_FIRE);
    ASSUME(gBattleMoves[MOVE_EMBER].split == SPLIT_SPECIAL);
}

SINGLE_BATTLE_TEST("Capsaicin Craze uses a random 40 BP Fire or Grass attack after using a move")
{
    u16 calledMove;

    PARAMETRIZE { calledMove = MOVE_LEAFAGE; }
    PARAMETRIZE { calledMove = MOVE_MAGICAL_LEAF; }
    PARAMETRIZE { calledMove = MOVE_FIRE_PUNCH; }
    PARAMETRIZE { calledMove = MOVE_EMBER; }

    GIVEN {
        PLAYER(SPECIES_SCOVILLAIN) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_CAPSAICIN_CRAZE, calledMove)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_CAPSAICIN_CRAZE);
        ANIMATION(ANIM_TYPE_MOVE, calledMove, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Capsaicin Craze does not recursively trigger from its called attack")
{
    GIVEN {
        PLAYER(SPECIES_SCOVILLAIN) { Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE, WITH_RNG(RNG_ROGUE_CAPSAICIN_CRAZE, MOVE_LEAFAGE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
        ABILITY_POPUP(player, ABILITY_CAPSAICIN_CRAZE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_LEAFAGE, player);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CAPSAICIN_CRAZE);
        }
    }
}

SINGLE_BATTLE_TEST("Capsaicin Craze called attacks do not apply their normal secondary effects")
{
    GIVEN {
        PLAYER(SPECIES_SCOVILLAIN) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_BATTLE_ARMOR); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_CAPSAICIN_CRAZE, MOVE_EMBER)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_CAPSAICIN_CRAZE);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EMBER, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(opponent->status1, STATUS1_NONE);
    }
}
