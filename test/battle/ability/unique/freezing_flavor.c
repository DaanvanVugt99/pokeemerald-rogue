#include "global.h"
#include "test/battle.h"

#if B_SNOW_WARNING >= GEN_9
SINGLE_BATTLE_TEST("Freezing Flavor summons snow on switch-in")
{
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_FREEZING_FLAVOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_FREEZING_FLAVOR);
        MESSAGE("It started to snow!");
        ANIMATION(ANIM_TYPE_GENERAL, B_ANIM_SNOW_CONTINUES);
    }
}
#endif

SINGLE_BATTLE_TEST("Freezing Flavor blocks burn, poison, and paralysis while snow is active")
{
    u32 move;
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_FREEZING_FLAVOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Freezing Flavor does not block those statuses if snow is gone")
{
    u32 move;
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_THUNDER_WAVE; }
    GIVEN {
        PLAYER(SPECIES_VANILLUXE) { Speed(1); Ability(ABILITY_SOUNDPROOF); UniqueAbility(ABILITY_FREEZING_FLAVOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Ability(ABILITY_NO_GUARD); Moves(MOVE_SUNNY_DAY, move); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUNNY_DAY); MOVE(player, MOVE_CELEBRATE); }
        TURN { MOVE(opponent, move); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_NE(player->status1, STATUS1_NONE);
    }
}
