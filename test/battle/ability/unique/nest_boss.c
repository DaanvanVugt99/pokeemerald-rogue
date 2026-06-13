#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].type == TYPE_NORMAL);
    ASSUME(gBattleMoves[MOVE_TAKE_DOWN].accuracy < 100);
    ASSUME(gBattleMoves[MOVE_SUCKER_PUNCH].effect == EFFECT_SUCKER_PUNCH);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
}

SINGLE_BATTLE_TEST("Nest Boss makes STAB moves never miss")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLAN) { Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_NEST_BOSS); Moves(MOVE_TAKE_DOWN); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1000); MaxHP(1000); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TAKE_DOWN, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAKE_DOWN, player);
        HP_BAR(opponent);
    }
}

SINGLE_BATTLE_TEST("Nest Boss uses Swords Dance when Sucker Punch fails")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLAN) { Speed(100); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_NEST_BOSS); Moves(MOVE_SUCKER_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUCKER_PUNCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_NEST_BOSS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
    }
}

SINGLE_BATTLE_TEST("Nest Boss does not use Swords Dance when Sucker Punch succeeds")
{
    GIVEN {
        PLAYER(SPECIES_RATICATE_ALOLAN) { Speed(100); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_NEST_BOSS); Moves(MOVE_SUCKER_PUNCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SUCKER_PUNCH); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_NEST_BOSS);
            ANIMATION(ANIM_TYPE_MOVE, MOVE_SWORDS_DANCE, player);
        }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE);
    }
}
