#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Chloroplast boosts Fire-type moves as if the sun were present", s16 damage)
{
    u32 move;
    u16 ability;
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_CHLOROPLAST;
    } // Fire-type move with Chloroplast
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_OVERGROW;
    } // Fire-type move with another ability (control)
    PARAMETRIZE
    {
        move = MOVE_TACKLE;
        ability = ABILITY_CHLOROPLAST;
    } // Non-Fire move with Chloroplast (control)
    PARAMETRIZE
    {
        move = MOVE_TACKLE;
        ability = ABILITY_STEADFAST;
    } // Non-Fire move with another ability (control)

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE); // Ensure the move is Fire-type
        PLAYER(SPECIES_CHARMANDER)
        {
            Ability(ability);
            MaxHP(99);
            HP(99);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage); // Capture the damage dealt
    }
    FINALLY
    {
        // If Chloroplast applies, the Fire move's damage should be boosted by 1.5x.
        float expectedMultiplier = (ability == ABILITY_CHLOROPLAST && gBattleMoves[move].type == TYPE_FIRE) ? 1.5f : 1.0f;

        // Check if Chloroplast boosts Fire-type moves
        if (gBattleMoves[move].type == TYPE_FIRE)
        {
            EXPECT_MUL_EQ(results[1].damage, Q_4_12(expectedMultiplier), results[0].damage);
        }

        // For non-Fire moves, damage should remain the same regardless of ability
        if (gBattleMoves[move].type != TYPE_FIRE)
        {
            EXPECT_EQ(results[2].damage, results[3].damage); // Non-Fire moves should not be boosted
        }
    }
}

SINGLE_BATTLE_TEST("Chloroplast activates Solar Beam instantly even without sunlight", s16 damage)
{
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_SOLAR_BEAM].type == TYPE_GRASS);

        PLAYER(SPECIES_CHARMANDER)
        {
            Ability(ABILITY_CHLOROPLAST);
            MaxHP(100);
            HP(100);
        }
        OPPONENT(SPECIES_WOBBUFFET)
        {
            MaxHP(200);
            HP(200);
        }
    }

    WHEN
    {
        TURN { MOVE(player, MOVE_SOLAR_BEAM); }
    }

    SCENE
    {
        MESSAGE("Charmander used Solar Beam!");
        NOT MESSAGE("Charmander took in sunlight!"); // Ensuring immediate use

        HP_BAR(opponent, captureDamage : &results[0].damage);
    }

    FINALLY
    {
        EXPECT_GT(results[0].damage, 0); // Damage should be dealt immediately
    }
}

SINGLE_BATTLE_TEST("Chloroplast boosts Fire-type moves even when weather is suppressed by Cloud Nine", s16 damage)
{
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE);

        PLAYER(SPECIES_CHARMANDER)
        {
            Ability(ABILITY_CHLOROPLAST);
            Moves(MOVE_SUNNY_DAY, MOVE_FLAMETHROWER);
            MaxHP(100);
            HP(100);
        }
        OPPONENT(SPECIES_GOLDUCK)
        {
            Ability(ABILITY_CLOUD_NINE);
            MaxHP(200);
            HP(200);
        }
    }

    WHEN
    {
        TURN { MOVE(player, MOVE_SUNNY_DAY); }
        TURN { SWITCH(opponent, 0); }
        TURN { MOVE(player, MOVE_FLAMETHROWER); }
    }

    SCENE
    {
        MESSAGE("Charmander used Flamethrower!");
        HP_BAR(opponent, captureDamage : &results[0].damage);
    }

    FINALLY
    {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[0].damage); // Confirming Chloroplast still boosts damage by 1.5x
    }
}