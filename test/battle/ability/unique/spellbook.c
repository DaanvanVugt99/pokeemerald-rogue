#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(!IS_MOVE_STATUS(MOVE_PSYBEAM));
    ASSUME(gBattleMoves[MOVE_MYSTICAL_FIRE].effect == EFFECT_SPECIAL_ATTACK_DOWN_HIT);
    ASSUME(gBattleMoves[MOVE_PSYBEAM].effect == EFFECT_CONFUSE_HIT);
    ASSUME(gBattleMoves[MOVE_MAGICAL_LEAF].effect == EFFECT_HIT);
    ASSUME(gBattleMoves[MOVE_WILL_O_WISP].effect == EFFECT_WILL_O_WISP);
    ASSUME(gBattleMoves[MOVE_CONFUSE_RAY].effect == EFFECT_CONFUSE);
    ASSUME(gBattleMoves[MOVE_LIGHT_SCREEN].effect == EFFECT_LIGHT_SCREEN);
    ASSUME(gBattleMoves[MOVE_REFLECT].effect == EFFECT_REFLECT);
    ASSUME(gBattleMoves[MOVE_CALM_MIND].effect == EFFECT_CALM_MIND);
    ASSUME(gBattleMoves[MOVE_LUCKY_CHANT].effect == EFFECT_LUCKY_CHANT);
    ASSUME(gBattleMoves[MOVE_MAGIC_ROOM].effect == EFFECT_MAGIC_ROOM);
    ASSUME(gBattleMoves[MOVE_WONDER_ROOM].effect == EFFECT_WONDER_ROOM);
}

SINGLE_BATTLE_TEST("Spellbook uses a random spell after a status move")
{
    GIVEN {
        PLAYER(SPECIES_DELPHOX) { Speed(100); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLBOOK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_SPELLBOOK, MOVE_PSYBEAM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_SPELLBOOK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PSYBEAM, player);
    }
}

SINGLE_BATTLE_TEST("Spellbook can choose field spell moves")
{
    GIVEN {
        PLAYER(SPECIES_DELPHOX) { Speed(100); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLBOOK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE, WITH_RNG(RNG_ROGUE_SPELLBOOK, MOVE_MAGIC_ROOM)); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_CELEBRATE, player);
        ABILITY_POPUP(player, ABILITY_SPELLBOOK);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MAGIC_ROOM, player);
    } THEN {
        EXPECT(gFieldStatuses & STATUS_FIELD_MAGIC_ROOM);
    }
}

SINGLE_BATTLE_TEST("Spellbook chooses from every spell")
{
    static const u16 expectedMoves[] =
    {
        MOVE_MYSTICAL_FIRE,
        MOVE_PSYBEAM,
        MOVE_MAGICAL_LEAF,
        MOVE_WILL_O_WISP,
        MOVE_CONFUSE_RAY,
        MOVE_LIGHT_SCREEN,
        MOVE_REFLECT,
        MOVE_CALM_MIND,
        MOVE_LUCKY_CHANT,
        MOVE_MAGIC_ROOM,
        MOVE_WONDER_ROOM,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_SPELLBOOK);

    GIVEN {
        PLAYER(SPECIES_DELPHOX) { Speed(100); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_SPELLBOOK); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_SPELLBOOK);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
