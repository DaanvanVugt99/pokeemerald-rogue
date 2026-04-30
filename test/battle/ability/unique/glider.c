#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_AIR_SLASH].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_ELECTRIC_TERRAIN].effect == EFFECT_ELECTRIC_TERRAIN);
}

SINGLE_BATTLE_TEST("Glider gives +1 priority to the first Flying/Electric move after switch-in")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { Speed(50); Ability(ABILITY_STATIC); Moves(MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
    }
}

SINGLE_BATTLE_TEST("Glider is consumed by the first move used, even if it is not Flying/Electric")
{
    GIVEN {
        PLAYER(SPECIES_EMOLGA) { Speed(50); Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE, MOVE_AIR_SLASH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Speed(60); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_AIR_SLASH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AIR_SLASH, player);
    }
}

SINGLE_BATTLE_TEST("Glider's first Flying/Electric move bypasses Protect in Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_EMOLGA) { Ability(ABILITY_STATIC); Moves(MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ELECTRIC_TERRAIN, MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK); MOVE(opponent, MOVE_PROTECT); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        MESSAGE("Foe Wobbuffet protected itself!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_THUNDER_SHOCK, player);
        HP_BAR(opponent);
        NOT MESSAGE("Foe Wobbuffet protected itself!");
    }
}
