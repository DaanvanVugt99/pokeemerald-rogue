#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_SEISMIC_TOSS].effect == EFFECT_LEVEL_DAMAGE);
}

SINGLE_BATTLE_TEST("Cell Division heals 25 percent of hit damage at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_REUNICLUS) { Level(100); Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_CELL_DIVISION); HP(300); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Level(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_SEISMIC_TOSS); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SEISMIC_TOSS); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SEISMIC_TOSS, opponent);
        HP_BAR(player, damage: 100);
        ABILITY_POPUP(player, ABILITY_CELL_DIVISION);
        MESSAGE("Reuniclus's Cell Division restored its HP a little!");
        HP_BAR(player, damage: -25);
    } THEN {
        EXPECT_EQ(player->hp, 225);
    }
}

SINGLE_BATTLE_TEST("Cell Division does not heal if the user was not hit")
{
    GIVEN {
        PLAYER(SPECIES_REUNICLUS) { Ability(ABILITY_OVERCOAT); UniqueAbility(ABILITY_CELL_DIVISION); HP(300); MaxHP(400); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_CELL_DIVISION);
            MESSAGE("Reuniclus's Cell Division restored its HP a little!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 300);
    }
}

