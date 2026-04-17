#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Divine Favor survives the first KO blow with 1 HP")
{
    GIVEN {
        PLAYER(SPECIES_TOGEKISS) { HP(10); MaxHP(10); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_DIVINE_FAVOR); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEWTWO) { Moves(MOVE_PSYSTRIKE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYSTRIKE); }
    } THEN {
        EXPECT_EQ(player->hp, 1);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}

SINGLE_BATTLE_TEST("Divine Favor only works once per battle")
{
    GIVEN {
        PLAYER(SPECIES_TOGEKISS) { HP(10); MaxHP(10); Speed(100); Ability(ABILITY_HUSTLE); UniqueAbility(ABILITY_DIVINE_FAVOR); Moves(MOVE_RECOVER, MOVE_CELEBRATE); }
        OPPONENT(SPECIES_MEWTWO) { Speed(50); Moves(MOVE_PSYSTRIKE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYSTRIKE); }
        TURN { MOVE(player, MOVE_RECOVER); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_PSYSTRIKE); }
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT(gBattleStruct->uniqueAbilityUsed[B_SIDE_PLAYER] & gBitTable[0]);
    }
}
