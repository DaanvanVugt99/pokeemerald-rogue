#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Acid Rain deals 1/16 damage per turn")
{
    s16 acidrainDamage;

    GIVEN
    {
        PLAYER(SPECIES_KOFFING);
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    }
    SCENE
    {
        MESSAGE("Foe Wobbuffet is scorched by the acid rain!");
        HP_BAR(opponent, captureDamage : &acidrainDamage);
    }
    THEN { EXPECT_EQ(acidrainDamage, opponent->maxHP / 16); }
}

SINGLE_BATTLE_TEST("Acid Rain damage does not hurt Poison and Bug-type Pokémon")
{
    u32 mon;
    PARAMETRIZE { mon = SPECIES_KOFFING; }    // Poison-type
    PARAMETRIZE { mon = SPECIES_BUTTERFREE; } // Bug-type
    GIVEN
    {
        ASSUME(gSpeciesInfo[SPECIES_KOFFING].types[0] == TYPE_POISON);
        ASSUME(gSpeciesInfo[SPECIES_BUTTERFREE].types[0] == TYPE_BUG);
        PLAYER(SPECIES_WOBBUFFET);
        OPPONENT(mon);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    }
    SCENE
    {
        switch (mon)
        {
        case SPECIES_KOFFING:
            NOT MESSAGE("Foe Koffing is scorched by the acid rain!");
            break;
        case SPECIES_BUTTERFREE:
            NOT MESSAGE("Foe Butterfree is scorched by the acid rain!");
            break;
        }
    }
}

SINGLE_BATTLE_TEST("Acid Rain damages other non-exempt Pokémon")
{
    u32 species;
    PARAMETRIZE { species = SPECIES_PIKACHU; }
    PARAMETRIZE { species = SPECIES_CHARIZARD; }
    PARAMETRIZE { species = SPECIES_WOBBUFFET; }
    GIVEN
    {
        PLAYER(SPECIES_KOFFING);
        OPPONENT(species);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
    }
    SCENE
    {
        if (species == SPECIES_PIKACHU)
            MESSAGE("Foe Pikachu is scorched by the acid rain!");
        else if (species == SPECIES_CHARIZARD)
            MESSAGE("Foe Charizard is scorched by the acid rain!");
        else if (species == SPECIES_WOBBUFFET)
            MESSAGE("Foe Wobbuffet is scorched by the acid rain!");
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Acid Rain lowers speed of other non-exempt Pokémon")
{
    GIVEN
    {
        PLAYER(SPECIES_KOFFING)
        {
            Speed(95);
        }
        OPPONENT(SPECIES_WOBBUFFET)
        {
            Speed(100);
        }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_ACID_RAIN); }
        TURN
        {
            MOVE(opponent, MOVE_CELEBRATE);
            MOVE(player, MOVE_CELEBRATE);
        }
    }
    SCENE
    {
        MESSAGE("Koffing used Celebrate!"); // Koffing should move first after Acid Rain
        MESSAGE("Foe Wobbuffet used Celebrate!");
    }
}
