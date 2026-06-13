#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].effect == EFFECT_SLEEP);
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].accuracy == 60);
    ASSUME(gBattleMoves[MOVE_HYPNOSIS].priority == 0);
    ASSUME(gBattleMoves[MOVE_ENCORE].effect == EFFECT_ENCORE);
    ASSUME(gBattleMoves[MOVE_ENCORE].priority == 0);
    ASSUME(gBattleMoves[MOVE_SNOWSCAPE].effect == EFFECT_SNOWSCAPE);
}

SINGLE_BATTLE_TEST("Lanakila Law gives Hypnosis priority in Snow")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLAN) { HP(100); MaxHP(100); Speed(1); Ability(ABILITY_SNOW_WARNING); UniqueAbility(ABILITY_LANAKILA_LAW); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPNOSIS, player);
        NONE_OF { ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT(opponent->status1 & STATUS1_SLEEP);
    }
}

SINGLE_BATTLE_TEST("Lanakila Law gives Encore priority in Snow")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLAN) { HP(100); MaxHP(100); Speed(1); Ability(ABILITY_SNOW_WARNING); UniqueAbility(ABILITY_LANAKILA_LAW); Moves(MOVE_CELEBRATE, MOVE_ENCORE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE, MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_ENCORE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
        EXPECT_EQ(gDisableStructs[B_POSITION_OPPONENT_LEFT].encoredMove, MOVE_CELEBRATE);
    }
}

SINGLE_BATTLE_TEST("Lanakila Law does not affect Hypnosis outside Snow")
{
    GIVEN {
        PLAYER(SPECIES_NINETALES_ALOLAN) { HP(100); MaxHP(100); Speed(1); Ability(ABILITY_SNOW_CLOAK); UniqueAbility(ABILITY_LANAKILA_LAW); Moves(MOVE_HYPNOSIS); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_HYPNOSIS, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_TACKLE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TACKLE, opponent);
    } THEN {
        EXPECT_LT(player->hp, player->maxHP);
        EXPECT(!(opponent->status1 & STATUS1_SLEEP));
    }
}
