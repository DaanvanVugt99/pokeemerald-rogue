#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_THUNDER_SHOCK].type == TYPE_ELECTRIC);
    ASSUME(gBattleMoves[MOVE_WATER_GUN].type == TYPE_WATER);
}

SINGLE_BATTLE_TEST("Redline makes Electric moves use Speed below half HP", s16 damage)
{
    u16 hp;
    bool32 useAgility;

    PARAMETRIZE { hp = 79; useAgility = FALSE; }
    PARAMETRIZE { hp = 79; useAgility = TRUE; }
    PARAMETRIZE { hp = 80; useAgility = FALSE; }
    PARAMETRIZE { hp = 80; useAgility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); MaxHP(160); Speed(100); SpAttack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_REDLINE); Moves(MOVE_AGILITY, MOVE_THUNDER_SHOCK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        if (useAgility)
            TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_THUNDER_SHOCK, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, Q_4_12(2.0), results[1].damage);
        EXPECT_EQ(results[2].damage, results[3].damage);
    }
}

SINGLE_BATTLE_TEST("Redline does not affect non-Electric move damage", s16 damage)
{
    bool32 useAgility;

    PARAMETRIZE { useAgility = FALSE; }
    PARAMETRIZE { useAgility = TRUE; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(79); MaxHP(160); Speed(100); SpAttack(100); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_REDLINE); Moves(MOVE_AGILITY, MOVE_WATER_GUN); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); }
    } WHEN {
        if (useAgility)
            TURN { MOVE(player, MOVE_AGILITY); }
        TURN { MOVE(player, MOVE_WATER_GUN, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Redline boosts turn order Speed below half HP")
{
    u16 hp;

    PARAMETRIZE { hp = 79; }
    PARAMETRIZE { hp = 80; }

    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { HP(hp); MaxHP(160); Speed(80); Ability(ABILITY_SHADOW_TAG); UniqueAbility(ABILITY_REDLINE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        if (hp < 80)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        }
    }
}
