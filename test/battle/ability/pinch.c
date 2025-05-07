#include "global.h"
#include "test/battle.h"

// Swarm Ability Test
SINGLE_BATTLE_TEST("Swarm boosts Bug-type moves consistently", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 99; }
    PARAMETRIZE { hp = 33; }
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_BUG_BITE].type == TYPE_BUG);
        PLAYER(SPECIES_PINSIR)
        {
            Ability(ABILITY_SWARM);
            MaxHP(99);
            HP(hp);
            Attack(45);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(121); }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_BUG_BITE); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        if (hp <= 49)
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.5), results[i].damage); // 1.5x boost at low HP
        else
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.2), results[i].damage); // 1.2x boost otherwise
    }
}

// Vengeance Ability Test
SINGLE_BATTLE_TEST("Vengeance boosts Ghost-type moves consistently", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 99; }
    PARAMETRIZE { hp = 33; }
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_SHADOW_SNEAK].type == TYPE_GHOST);
        PLAYER(SPECIES_GOLURK)
        {
            Ability(ABILITY_VENGEANCE);
            MaxHP(99);
            HP(hp);
            Attack(75);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(121); }
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_SHADOW_SNEAK); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        if (hp <= 49)
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.5), results[i].damage); // 1.5x boost at low HP
        else
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.2), results[i].damage); // 1.2x boost otherwise
    }
}

// Blaze Ability Test
SINGLE_BATTLE_TEST("Blaze boosts Fire-type moves consistently", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 99; }
    PARAMETRIZE { hp = 33; }
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_FLAMETHROWER].type == TYPE_FIRE);
        PLAYER(SPECIES_CHARMANDER)
        {
            Ability(ABILITY_BLAZE);
            MaxHP(99);
            HP(hp);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_FLAMETHROWER); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        if (hp <= 49)
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.5), results[i].damage); // 1.5x boost at low HP
        else
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.2), results[i].damage); // 1.2x boost otherwise
    }
}

// Torrent Ability Test
SINGLE_BATTLE_TEST("Torrent boosts Water-type moves consistently", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 99; }
    PARAMETRIZE { hp = 33; }
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_BUBBLE].type == TYPE_WATER);
        PLAYER(SPECIES_SQUIRTLE)
        {
            Ability(ABILITY_TORRENT);
            MaxHP(99);
            HP(hp);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_BUBBLE); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        if (hp <= 49)
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.5), results[i].damage); // 1.5x boost at low HP
        else
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.2), results[i].damage); // 1.2x boost otherwise
    }
}

// Overgrow Ability Test
SINGLE_BATTLE_TEST("Overgrow boosts Grass-type moves consistently", s16 damage)
{
    u16 hp;
    PARAMETRIZE { hp = 99; }
    PARAMETRIZE { hp = 33; }
    GIVEN
    {
        ASSUME(gBattleMoves[MOVE_VINE_WHIP].type == TYPE_GRASS);
        PLAYER(SPECIES_BULBASAUR)
        {
            Ability(ABILITY_OVERGROW);
            MaxHP(99);
            HP(hp);
        }
        OPPONENT(SPECIES_WOBBUFFET);
    }
    WHEN
    {
        TURN { MOVE(player, MOVE_VINE_WHIP); }
    }
    SCENE
    {
        HP_BAR(opponent, captureDamage : &results[i].damage);
    }
    FINALLY
    {
        if (hp <= 49)
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.5), results[i].damage); // 1.5x boost at low HP
        else
            EXPECT_MUL_EQ(results[i].damage, Q_4_12(1.2), results[i].damage); // 1.2x boost otherwise
    }
}
