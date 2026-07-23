#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_ESPER_WING].effect == EFFECT_SPEED_UP_HIT);
    ASSUME(gBattleMoves[MOVE_AGILITY].effect == EFFECT_SPEED_UP_2);
}

SINGLE_BATTLE_TEST("Premonition sets Tailwind after Esper Wing raises Speed")
{
    GIVEN {
        PLAYER(SPECIES_BRAVIARY_HISUIAN) { Speed(100); Moves(MOVE_ESPER_WING); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_ESPER_WING); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_ESPER_WING, player);
        ABILITY_POPUP(player, ABILITY_PREMONITION);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAILWIND, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
        EXPECT_EQ(gSideTimers[B_SIDE_PLAYER].tailwindTimer, 3);
    }
}

SINGLE_BATTLE_TEST("Premonition activates for Speed boosts other than Esper Wing")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_PREMONITION); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_AGILITY, player);
        ABILITY_POPUP(player, ABILITY_PREMONITION);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
        EXPECT(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND);
    }
}

SINGLE_BATTLE_TEST("Premonition does not activate when Speed does not rise")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_PREMONITION); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_SCARY_FACE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_SCARY_FACE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_PREMONITION);
    } THEN {
        EXPECT(!(gSideStatuses[B_SIDE_PLAYER] & SIDE_STATUS_TAILWIND));
    }
}

SINGLE_BATTLE_TEST("Premonition does not reactivate while Tailwind is active")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Speed(100); UniqueAbility(ABILITY_PREMONITION); Moves(MOVE_AGILITY); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_AGILITY); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_PREMONITION);
        NOT ABILITY_POPUP(player, ABILITY_PREMONITION);
    }
}
