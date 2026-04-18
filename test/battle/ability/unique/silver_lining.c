#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Silver Lining blocks major status moves")
{
    u32 move;
    PARAMETRIZE { move = MOVE_TOXIC; }
    PARAMETRIZE { move = MOVE_WILL_O_WISP; }
    PARAMETRIZE { move = MOVE_SPORE; }
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SILVER_LINING); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(move); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, move); }
    } THEN {
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Silver Lining restores 1/16 HP after taking a hit")
{
    GIVEN {
        PLAYER(SPECIES_SHUCKLE) { HP(100); MaxHP(160); Ability(ABILITY_STURDY); UniqueAbility(ABILITY_SILVER_LINING); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_DRAGON_RAGE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_DRAGON_RAGE); }
    } THEN {
        EXPECT_EQ(player->hp, 70);
    }
}
