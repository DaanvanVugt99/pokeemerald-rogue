#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_ROCK_THROW].priority == 0);
    ASSUME(gBattleMoves[MOVE_ROCK_POLISH].type == TYPE_ROCK);
    ASSUME(gBattleMoves[MOVE_ROCK_POLISH].split == SPLIT_STATUS);
    ASSUME(gBattleMoves[MOVE_ROCK_POLISH].priority == 0);
    ASSUME(gBattleMoves[MOVE_TACKLE].priority == 0);
}

SINGLE_BATTLE_TEST("Railgun Charge boosts Rock damage in Electric Terrain and ends the terrain", s16 damage)
{
    bool32 terrain;

    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLAN) { Attack(200); Speed(100); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_RAILGUN_CHARGE); Moves(MOVE_ROCK_THROW, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Defense(100); Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, terrain ? MOVE_ELECTRIC_TERRAIN : MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ROCK_THROW, WITH_RNG(RNG_DAMAGE_MODIFIER, 100)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
        if (terrain)
            ABILITY_POPUP(player, ABILITY_RAILGUN_CHARGE);
    } THEN {
        if (terrain)
            EXPECT(!(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN));
    } FINALLY {
        EXPECT_MUL_EQ(results[0].damage, UQ_4_12(1.5), results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Railgun Charge does not give status Rock moves priority in Electric Terrain")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLAN) { Speed(1); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_RAILGUN_CHARGE); Moves(MOVE_ROCK_POLISH, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_ELECTRIC_TERRAIN); }
        TURN { MOVE(player, MOVE_ROCK_POLISH); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_POLISH, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_ELECTRIC_TERRAIN);
    }
}

SINGLE_BATTLE_TEST("Railgun Charge gives Rock moves priority in Electric Terrain")
{
    bool32 terrain;

    PARAMETRIZE { terrain = FALSE; }
    PARAMETRIZE { terrain = TRUE; }

    GIVEN {
        PLAYER(SPECIES_GOLEM_ALOLAN) { Speed(1); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_RAILGUN_CHARGE); Moves(MOVE_ROCK_THROW, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_ELECTRIC_TERRAIN, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, terrain ? MOVE_ELECTRIC_TERRAIN : MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_ROCK_THROW); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        if (terrain)
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
        }
        else
        {
            ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_ROCK_THROW, player);
        }
    }
}
