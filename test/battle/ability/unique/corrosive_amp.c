#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ACID_RAIN].effect == EFFECT_CORROSIVE_CLOUDS);
}

SINGLE_BATTLE_TEST("Corrosive Amp holder loses 1/16 max HP each turn in Acid Rain")
{
    GIVEN {
        PLAYER(SPECIES_TOXTRICITY_AMPED) {
            HP(200);
            MaxHP(200);
            Ability(ABILITY_PUNK_ROCK);
            UniqueAbility(ABILITY_CORROSIVE_AMP);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_ACID_RAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ACID_RAIN); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->hp, 176);
    }
}
