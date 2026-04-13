#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].type == TYPE_GROUND);
    ASSUME(gBattleMoves[MOVE_EARTHQUAKE].damagesUnderground);
}

SINGLE_BATTLE_TEST("Subterranean halves super-effective damage in sandstorm", s16 damage)
{
    bool32 setSandstorm;

    PARAMETRIZE { setSandstorm = FALSE; }
    PARAMETRIZE { setSandstorm = TRUE; }

    GIVEN {
        PLAYER(SPECIES_STEELIX) { HP(300); MaxHP(300); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SUBTERRANEAN); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_SANDSTORM, MOVE_WATER_GUN); }
    } WHEN {
        if (setSandstorm)
            TURN { MOVE(opponent, MOVE_SANDSTORM); MOVE(player, MOVE_CELEBRATE); }
        else
            TURN { MOVE(opponent, MOVE_CELEBRATE); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, MOVE_WATER_GUN); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Subterranean halves super-effective damage while underground", s16 damage)
{
    u16 uniqueAbility;

    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_SUBTERRANEAN; }

    GIVEN {
        PLAYER(SPECIES_STEELIX) { Level(50); HP(1000); MaxHP(1000); Speed(20); Ability(ABILITY_STURDY); UniqueAbility(uniqueAbility); Moves(MOVE_DIG); }
        OPPONENT(SPECIES_DUGTRIO) { Level(1); Speed(10); Ability(ABILITY_SAND_VEIL); Moves(MOVE_EARTHQUAKE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DIG); MOVE(opponent, MOVE_EARTHQUAKE); }
    } SCENE {
        HP_BAR(player, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_MUL_EQ(results[1].damage, UQ_4_12(2.0), results[0].damage);
    }
}

SINGLE_BATTLE_TEST("Subterranean prevents flinching")
{
    GIVEN {
        PLAYER(SPECIES_STEELIX) { Speed(1); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SUBTERRANEAN); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(2); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_FAKE_OUT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_FAKE_OUT); MOVE(player, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_FAKE_OUT, opponent);
        NONE_OF { MESSAGE("Steelix flinched!"); }
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, player);
    }
}
