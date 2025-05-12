#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Acid rain deals 1/16 damage per turn")
{
    s16 acidrainDamage;

    GIVEN
    {
        PLAYER(SPECIES_KOFFING);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_CORROSIVE_CLOUDS); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet is scorched by acid rain!");
        HP_BAR(opponent, captureDamage : &acidrainDamage);
    }
    THEN { EXPECT_EQ(acidrainDamage, opponent->maxHP / 16); }
}

SINGLE_BATTLE_TEST("Acid rain damage does not affect Poison-type Pokémon")
{
    GIVEN
    {
        ASSUME(gSpeciesInfo[SPECIES_KOFFING].types[0] == TYPE_POISON);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(SPECIES_KOFFING);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_CORROSIVE_CLOUDS); }
    }
    SCENE
    {
        NOT MESSAGE("Foe Wobbuffet is scorched by acid rain!");
    }
}
