#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GUST].type == TYPE_FLYING);
    ASSUME(gBattleMoves[MOVE_MINIMIZE].effect == EFFECT_MINIMIZE);
    ASSUME(gBattleMoves[MOVE_TAILWIND].effect == EFFECT_TAILWIND);
    ASSUME(gBattleMoves[MOVE_EXPLOSION].effect == EFFECT_EXPLOSION);
    ASSUME(gBattleMoves[MOVE_OMINOUS_WIND].effect == EFFECT_ALL_STATS_UP_HIT);
    ASSUME(gBattleMoves[MOVE_STOCKPILE].effect == EFFECT_STOCKPILE);
    ASSUME(gBattleMoves[MOVE_BATON_PASS].effect == EFFECT_BATON_PASS);
}

SINGLE_BATTLE_TEST("Balloon Burst uses a random rupture move when first dropping below half HP each battle")
{
    GIVEN {
        PLAYER(SPECIES_DRIFBLIM)
        {
            Ability(ABILITY_AFTERMATH);
            UniqueAbility(ABILITY_BALLOON_BURST);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_BALLOON_BURST, MOVE_TAILWIND)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        ABILITY_POPUP(player, ABILITY_BALLOON_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
    } THEN {
        EXPECT(gSideTimers[B_SIDE_PLAYER].tailwindTimer > 0);
    }
}

SINGLE_BATTLE_TEST("Balloon Burst only triggers once each battle")
{
    GIVEN {
        PLAYER(SPECIES_DRIFBLIM)
        {
            Ability(ABILITY_AFTERMATH);
            UniqueAbility(ABILITY_BALLOON_BURST);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_BALLOON_BURST, MOVE_MINIMIZE)); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_BALLOON_BURST, MOVE_TAILWIND)); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BALLOON_BURST);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_MINIMIZE, player);
        NOT ABILITY_POPUP(player, ABILITY_BALLOON_BURST);
    }
}

SINGLE_BATTLE_TEST("Balloon Burst does not trigger if already below half HP")
{
    GIVEN {
        PLAYER(SPECIES_DRIFBLIM)
        {
            Ability(ABILITY_AFTERMATH);
            UniqueAbility(ABILITY_BALLOON_BURST);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 - 1);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE, WITH_RNG(RNG_ROGUE_BALLOON_BURST, MOVE_MINIMIZE)); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_DRAGON_RAGE, opponent);
        HP_BAR(player);
        NOT ABILITY_POPUP(player, ABILITY_BALLOON_BURST);
    }
}

SINGLE_BATTLE_TEST("Balloon Burst chooses from every rupture move")
{
    static const u16 expectedMoves[] =
    {
        MOVE_GUST,
        MOVE_MINIMIZE,
        MOVE_TAILWIND,
        MOVE_EXPLOSION,
        MOVE_OMINOUS_WIND,
        MOVE_STOCKPILE,
        MOVE_BATON_PASS,
    };

    PASSES_RANDOMLY(ARRAY_COUNT(expectedMoves), ARRAY_COUNT(expectedMoves), RNG_ROGUE_BALLOON_BURST);

    GIVEN {
        PLAYER(SPECIES_DRIFBLIM)
        {
            Ability(ABILITY_AFTERMATH);
            UniqueAbility(ABILITY_BALLOON_BURST);
            HP(GetMonData(&PLAYER_PARTY[0], MON_DATA_MAX_HP) / 2 + 20);
            Moves(MOVE_CELEBRATE);
        }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_SHADOW_TAG); Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_DRAGON_RAGE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_BALLOON_BURST);
    } THEN {
        EXPECT_EQ(gCalledMove, expectedMoves[gBattleTestRunnerState->runTrial]);
    }
}
