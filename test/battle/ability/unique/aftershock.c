#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Aftershock rolls Magnitude 4 to 7 after a successful Ground move")
{
    u32 roll, power;
    PARAMETRIZE { roll = 0; power = 10; }
    PARAMETRIZE { roll = 1; power = 30; }
    PARAMETRIZE { roll = 2; power = 50; }
    PARAMETRIZE { roll = 3; power = 70; }

    GIVEN {
        PLAYER(SPECIES_GOLEM) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_AFTERSHOCK); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(400); MaxHP(400); }
    } WHEN {
        TURN { MOVE(player, MOVE_EARTHQUAKE, WITH_RNG(RNG_ROGUE_AFTERSHOCK, roll)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_EARTHQUAKE, player);
        HP_BAR(opponent);
        ABILITY_POPUP(player, ABILITY_AFTERSHOCK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNITUDE, player);
        HP_BAR(opponent);
    } THEN {
        EXPECT_EQ(gBattleStruct->magnitudeBasePower, power);
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Aftershock does not trigger after non-Ground moves", s16 damage)
{
    u16 uniqueAbility;
    PARAMETRIZE { uniqueAbility = ABILITY_NONE; }
    PARAMETRIZE { uniqueAbility = ABILITY_AFTERSHOCK; }

    GIVEN {
        PLAYER(SPECIES_GOLEM) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(uniqueAbility); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } SCENE {
        HP_BAR(opponent, captureDamage: &results[i].damage);
    } FINALLY {
        EXPECT_EQ(results[0].damage, results[1].damage);
    }
}

SINGLE_BATTLE_TEST("Aftershock does not trigger when the Ground move is blocked")
{
    GIVEN {
        PLAYER(SPECIES_GOLEM) { Ability(ABILITY_ROCK_HEAD); UniqueAbility(ABILITY_AFTERSHOCK); Moves(MOVE_EARTHQUAKE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_PROTECT); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_EARTHQUAKE, hit: FALSE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_AFTERSHOCK);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGNITUDE, player);
        }
    }
}
