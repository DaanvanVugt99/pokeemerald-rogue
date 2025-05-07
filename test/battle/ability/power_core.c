#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Power Core boosts physical and special moves", s16 damage)
{
    u32 move;
    u16 ability;

    PARAMETRIZE
    {
        move = MOVE_BODY_SLAM;
        ability = ABILITY_POWER_CORE;
    } // Physical
    PARAMETRIZE
    {
        move = MOVE_BODY_SLAM;
        ability = ABILITY_STEADFAST;
    } // Physical, control
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_POWER_CORE;
    } // Special
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_STEADFAST;
    } // Special, control

    GIVEN
    {
        PLAYER(SPECIES_REGISTEEL)
        {
            Ability(ability);
            Attack(100);
            SpAttack(100);
            Defense(100);
            SpDefense(100);
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
        EXPECT_GT(results[0].damage, results[1].damage); // Physical move boosted
        EXPECT_GT(results[2].damage, results[3].damage); // Special move boosted
    }
}

SINGLE_BATTLE_TEST("Power Core boosts physical moves with increased defense", s16 damage)
{
    u32 move;
    u16 ability;

    PARAMETRIZE
    {
        move = MOVE_BODY_SLAM;
        ability = ABILITY_POWER_CORE;
    } // Physical, boosted
    PARAMETRIZE
    {
        move = MOVE_BODY_SLAM;
        ability = ABILITY_STEADFAST;
    } // Physical, control

    GIVEN
    {
        PLAYER(SPECIES_REGISTEEL)
        {
            Ability(ability);
            Attack(100);
            Defense(100);
            Moves(MOVE_IRON_DEFENSE, MOVE_BODY_SLAM);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_IRON_DEFENSE); } // Boost Defense
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_IRON_DEFENSE, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BODY_SLAM, player);
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        // Physical move boosted by defense increase with Power Core
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Power Core boosts special moves with increased special defense", s16 damage)
{
    u32 move;
    u16 ability;

    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_POWER_CORE;
    } // Special, boosted
    PARAMETRIZE
    {
        move = MOVE_FLAMETHROWER;
        ability = ABILITY_STEADFAST;
    } // Special, control

    GIVEN
    {
        PLAYER(SPECIES_REGISTEEL)
        {
            Ability(ability);
            SpAttack(100);
            SpDefense(100);
            Moves(MOVE_CALM_MIND, MOVE_FLAMETHROWER);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_CALM_MIND); } // Boost SpDefense
        TURN { MOVE(player, move); }
    }
    SCENE
    {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CALM_MIND, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FLAMETHROWER, player);
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        // Special move boosted by special defense increase with Power Core
        EXPECT_GT(results[0].damage, results[1].damage);
    }
}
