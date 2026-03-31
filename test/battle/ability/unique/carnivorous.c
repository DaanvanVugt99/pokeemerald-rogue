#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GUST].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_SIGNAL_BEAM].type == TYPE_BUG);
    ASSUME(gBattleMoves[MOVE_VINE_WHIP].type == TYPE_GRASS);
}

SINGLE_BATTLE_TEST("Carnivorous removes only Grass's Flying/Bug weakness")
{
    u16 move;
    bool32 shouldBeNotVeryEffective;
    PARAMETRIZE { move = MOVE_GUST; shouldBeNotVeryEffective = FALSE; }
    PARAMETRIZE { move = MOVE_SIGNAL_BEAM; shouldBeNotVeryEffective = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(move); }
        OPPONENT(SPECIES_VICTREEBEL) { Ability(ABILITY_CHLOROPHYLL); UniqueAbility(ABILITY_CARNIVOROUS); }
    } WHEN {
        TURN { MOVE(player, move); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, move, player);
        HP_BAR(opponent);
        if (shouldBeNotVeryEffective)
            MESSAGE("It's not very effective…");
        else {
            NONE_OF {
                MESSAGE("It's super effective!");
                MESSAGE("It's not very effective…");
            }
        }
    }
}

SINGLE_BATTLE_TEST("Carnivorous makes Grass moves super effective against Flying and Bug")
{
    u16 attackerSpecies;
    u16 targetSpecies;
    bool32 shouldBeSuperEffective;
    PARAMETRIZE { attackerSpecies = SPECIES_ODDISH; targetSpecies = SPECIES_PIDGEY; shouldBeSuperEffective = FALSE; }
    PARAMETRIZE { attackerSpecies = SPECIES_BELLSPROUT; targetSpecies = SPECIES_PIDGEY; shouldBeSuperEffective = TRUE; }
    PARAMETRIZE { attackerSpecies = SPECIES_ODDISH; targetSpecies = SPECIES_CATERPIE; shouldBeSuperEffective = FALSE; }
    PARAMETRIZE { attackerSpecies = SPECIES_BELLSPROUT; targetSpecies = SPECIES_CATERPIE; shouldBeSuperEffective = TRUE; }

    GIVEN {
        PLAYER(attackerSpecies) { Ability(ABILITY_CHLOROPHYLL); Moves(MOVE_VINE_WHIP); }
        OPPONENT(targetSpecies);
    } WHEN {
        TURN { MOVE(player, MOVE_VINE_WHIP); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_VINE_WHIP, player);
        HP_BAR(opponent);
        if (shouldBeSuperEffective)
            MESSAGE("It's super effective!");
        else
            NOT MESSAGE("It's super effective!");
    }
}
