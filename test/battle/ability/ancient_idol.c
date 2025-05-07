#include "global.h"
#include "test/battle.h"

// Test: Ancient Idol makes Defense act as Attack and Special Defense as Special Attack
SINGLE_BATTLE_TEST("Ancient Idol makes Defense and Special Defense act as Attack and Special Attack", s16 damage)
{
    u16 ability;
    u32 move;
    PARAMETRIZE
    {
        move = MOVE_EARTHQUAKE;
        ability = ABILITY_ANCIENT_IDOL;
    }
    PARAMETRIZE
    {
        move = MOVE_EARTHQUAKE;
        ability = ABILITY_LEVITATE;
    }
    PARAMETRIZE
    {
        move = MOVE_PSYCHIC;
        ability = ABILITY_ANCIENT_IDOL;
    }
    PARAMETRIZE
    {
        move = MOVE_PSYCHIC;
        ability = ABILITY_LEVITATE;
    }
    GIVEN
    {
        PLAYER(SPECIES_CLAYDOL)
        {
            Ability(ability);
            Defense(300);
            SpDefense(300);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        // Check if the damage is significantly higher when using Ancient Idol
        EXPECT_GT(results[0].damage, results[1].damage); // Earthquake with Defense as Attack
        EXPECT_GT(results[2].damage, results[3].damage); // Psychic with Sp. Defense as Sp. Attack
    }
}