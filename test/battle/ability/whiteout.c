#include "global.h"
#include "test/battle.h"

// Assuming B_WEATHER_HAIL covers both Hail and Snow mechanics for this ability,
// or use (B_WEATHER_HAIL | B_WEATHER_SNOW) if they are distinct relevant flags.
#define WHITEOUT_WEATHER (B_WEATHER_HAIL | B_WEATHER_SNOW)

SINGLE_BATTLE_TEST("Whiteout boosts Ice-type moves by 1.5x in Hail/Snow", s16 damage)
{
    u16 ability;
    u32 weather;
    PARAMETRIZE
    {
        ability = ABILITY_WHITEOUT;
        weather = WHITEOUT_WEATHER;
    } // Test case: Whiteout active in Hail/Snow
    PARAMETRIZE
    {
        ability = ABILITY_ICE_BODY;
        weather = WHITEOUT_WEATHER;
    } // Control 1: Different ability in Hail/Snow
    PARAMETRIZE
    {
        ability = ABILITY_WHITEOUT;
        weather = B_WEATHER_NONE;
    } // Control 2: Whiteout active, but no Hail/Snow
    PARAMETRIZE
    {
        ability = ABILITY_ICE_BODY;
        weather = B_WEATHER_NONE;
    } // Control 3: Different ability, no Hail/Snow (Baseline)

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_ICE_BEAM].type == TYPE_ICE);
        PLAYER(SPECIES_GLACEON) // A Pokemon that might logically have Whiteout
        {
            Ability(ability);
            Item(ITEM_NEVER_MELT_ICE); // Optional: Add item to ensure damage variation is primarily from ability
        }
        OPPONENT(SPECIES_WOBBUFFET) // Neutral target
        {
            HP(500); // Ensure it survives the hit
        }
    }
    WHEN
    {
        TURN
        {
            // Set weather *before* the move happens in the turn sequence
            if (weather != B_WEATHER_NONE)
                gBattleWeather = weather; // Force weather for the test turn
            MOVE(player, MOVE_ICE_BEAM);
            if (weather != B_WEATHER_NONE)
                gBattleWeather = B_WEATHER_NONE; // Reset weather after the turn if it was forced
        }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        // Compare Whiteout+Hail vs Control+Hail: Expect 1.5x boost
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[1].damage);
        // Compare Whiteout+Hail vs Whiteout+None: Expect 1.5x boost
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(1.5), results[2].damage);
        // Compare Control+Hail vs Control+None: Expect no difference (Hail itself doesn't boost Ice dmg)
        EXPECT_EQ(results[1].damage, results[3].damage);
        // Compare Whiteout+None vs Control+None: Expect no difference (baseline check)
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Whiteout does not boost non-Ice-type moves in Hail/Snow", s16 damage)
{
    u16 ability;
    PARAMETRIZE { ability = ABILITY_WHITEOUT; } // Test case: Whiteout active
    PARAMETRIZE { ability = ABILITY_ICE_BODY; } // Control: Different ability

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_TACKLE].type == TYPE_NORMAL); // Ensure the move is not Ice-type
        PLAYER(SPECIES_GLACEON)
        {
            Ability(ability);
        }
        OPPONENT(SPECIES_WOBBUFFET)
        {
            HP(500);
        }
    }
    WHEN
    {
        TURN
        {
            // Force Hail/Snow for the turn
            gBattleWeather = WHITEOUT_WEATHER;
            MOVE(player, MOVE_TACKLE);
            gBattleWeather = B_WEATHER_NONE; // Reset weather
        }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        // Expect damage to be the same regardless of Whiteout for non-Ice moves
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Whiteout boost does not occur if weather is suppressed by Cloud Nine", s16 damage)
{
    u16 opponentAbility;
    PARAMETRIZE { opponentAbility = ABILITY_CLOUD_NINE; } // Test Case: Weather suppressed
    PARAMETRIZE { opponentAbility = ABILITY_PRESSURE; }   // Control: Weather active

    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_ICE_BEAM].type == TYPE_ICE);
        PLAYER(SPECIES_GLACEON)
        {
            Ability(ABILITY_WHITEOUT);
        }
        OPPONENT(SPECIES_GOLDUCK)
        {
            Ability(opponentAbility);
            HP(500);
        }
    }
    WHEN
    {
        TURN
        {
            // Set Hail/Snow weather. Cloud Nine should negate its effects.
            // Note: Cloud Nine/Air Lock prevent effects, not the weather itself existing.
            // The damage calculation should check WEATHER_HAS_EFFECT which Cloud Nine sets to FALSE.
            gBattleWeather = WHITEOUT_WEATHER;
            MOVE(player, MOVE_ICE_BEAM);
            gBattleWeather = B_WEATHER_NONE; // Reset weather
        }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        // Expect damage with Cloud Nine (results[0]) to be UNBOOSTED.
        // Expect damage without Cloud Nine (results[1]) to be BOOSTED.
        // Therefore, the boosted damage should be 1.5x the unboosted damage.
        EXPECT_MUL_EQ(results[1].damage, Q_4_12(1.5), results[0].damage);
    }
}