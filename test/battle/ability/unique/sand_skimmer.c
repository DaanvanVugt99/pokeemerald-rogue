#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Sand Skimmer prevents Intimidate in sandstorm")
{
    s16 turnOneHit;
    s16 turnTwoHit;

    GIVEN {
        PLAYER(SPECIES_EKANS) { Ability(ABILITY_SHED_SKIN); }
        PLAYER(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_GLIGAR) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SAND_SKIMMER); }
    } WHEN {
        TURN { MOVE(player, MOVE_SANDSTORM); MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SANDSTORM, player);
        HP_BAR(player, captureDamage: &turnOneHit);
        NONE_OF { ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_STATS_CHANGE, player); }
        ABILITY_POPUP(opponent, ABILITY_SAND_SKIMMER);
        MESSAGE("Foe Gligar's Sand Skimmer prevents stat loss!");
        HP_BAR(player, captureDamage: &turnTwoHit);
    } THEN {
        EXPECT_EQ(turnOneHit, turnTwoHit);
    }
}

SINGLE_BATTLE_TEST("Sand Skimmer does not prevent Intimidate outside sandstorm")
{
    s16 turnOneHit;
    s16 turnTwoHit;

    GIVEN {
        PLAYER(SPECIES_EKANS) { Ability(ABILITY_SHED_SKIN); }
        PLAYER(SPECIES_EKANS) { Ability(ABILITY_INTIMIDATE); }
        OPPONENT(SPECIES_GLIGAR) { Ability(ABILITY_SAND_VEIL); UniqueAbility(ABILITY_SAND_SKIMMER); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_TACKLE); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(player, captureDamage: &turnOneHit);
        NONE_OF {
            ABILITY_POPUP(opponent, ABILITY_SAND_SKIMMER);
        }
        HP_BAR(player, captureDamage: &turnTwoHit);
    } THEN {
        EXPECT_LT(turnTwoHit, turnOneHit);
    }
}
