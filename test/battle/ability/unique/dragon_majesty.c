#include "constants/moves.h"
#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TWISTER].type == TYPE_DRAGON);
    ASSUME(gBattleMoves[MOVE_TACKLE].type != TYPE_DRAGON);
}

SINGLE_BATTLE_TEST("Dragon Majesty suppresses the target's Ability after a Dragon-type hit")
{
    GIVEN {
        PLAYER(SPECIES_DRAGONITE) { Speed(50); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_DRAGON_MAJESTY); Moves(MOVE_TWISTER); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); Ability(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TWISTER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TWISTER); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
    }
}

SINGLE_BATTLE_TEST("Dragon Majesty does not suppress after non-Dragon-type moves")
{
    GIVEN {
        PLAYER(SPECIES_DRAGONITE) { Speed(50); Ability(ABILITY_INNER_FOCUS); UniqueAbility(ABILITY_DRAGON_MAJESTY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(40); Ability(ABILITY_SPEED_BOOST); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 2);
    }
}
