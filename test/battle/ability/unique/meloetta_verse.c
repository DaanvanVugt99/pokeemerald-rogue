#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_RELIC_SONG].soundMove);
    ASSUME(gBattleMoves[MOVE_SNARL].soundMove);
    ASSUME(gBattleMoves[MOVE_HYPER_VOICE].soundMove);
}

SINGLE_BATTLE_TEST("Opening Verse: after a sound move, the next contact move raises Speed by 1")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_ARIA) { Speed(100); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_OPENING_VERSE); Moves(MOVE_RELIC_SONG, MOVE_QUICK_ATTACK); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_RELIC_SONG); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_RELIC_SONG, player);
        MESSAGE("Meloetta transformed!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        ABILITY_POPUP(player, ABILITY_OPENING_VERSE);
    } THEN {
        EXPECT_GT(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Final Step: after a contact move, the next sound move cannot miss and always crits")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_PIROUETTE) { Speed(100); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_FINAL_STEP); Moves(MOVE_QUICK_ATTACK, MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SNARL, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_QUICK_ATTACK, player);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Final Step is consumed by the first sound move")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_PIROUETTE) { Speed(100); Ability(ABILITY_SERENE_GRACE); UniqueAbility(ABILITY_FINAL_STEP); Moves(MOVE_QUICK_ATTACK, MOVE_SNARL, MOVE_HYPER_VOICE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SNARL, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_HYPER_VOICE, WITH_RNG(RNG_CRITICAL_HIT, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        MESSAGE("A critical hit!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_HYPER_VOICE, player);
        NOT MESSAGE("A critical hit!");
    }
}

SINGLE_BATTLE_TEST("Final Step is not consumed if the sound move is blocked before execution")
{
    GIVEN {
        PLAYER(SPECIES_MELOETTA_PIROUETTE) { Speed(90); Ability(ABILITY_TRUANT); UniqueAbility(ABILITY_FINAL_STEP); Moves(MOVE_QUICK_ATTACK, MOVE_SNARL); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_QUICK_ATTACK); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SNARL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SNARL, WITH_RNG(RNG_ACCURACY, FALSE)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_SNARL, player);
        MESSAGE("A critical hit!");
    }
}
