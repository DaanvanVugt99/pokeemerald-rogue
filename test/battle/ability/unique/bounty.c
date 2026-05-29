#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_CELEBRATE));
    ASSUME(gBattleMoves[MOVE_TACKLE].power > 0);
}

SINGLE_BATTLE_TEST("Bounty does not activate if the target is not knocked out")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Level(100); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET);
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_BOUNTY);
    }
}

WILD_BATTLE_TEST("Bounty still pays out when knocking out the last opposing Pokemon")
{
    GIVEN {
        PLAYER(SPECIES_KROOKODILE) { Level(100); Attack(255); Ability(ABILITY_INTIMIDATE); UniqueAbility(ABILITY_BOUNTY); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(1); MaxHP(100); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(gPaydayMoney, 500);
    }
}
