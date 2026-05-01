#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Mystic Rite raises Sp. Atk by 1 after using a status move while terrain is active")
{
    GIVEN {
        PLAYER(SPECIES_DELPHOX) { Speed(100); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_MYSTIC_RITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Ability(ABILITY_ELECTRIC_SURGE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_MYSTIC_RITE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE + 1);
    }
}

SINGLE_BATTLE_TEST("Mystic Rite does not raise Sp. Atk without terrain")
{
    GIVEN {
        PLAYER(SPECIES_DELPHOX) { Speed(100); Ability(ABILITY_BLAZE); UniqueAbility(ABILITY_MYSTIC_RITE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_MYSTIC_RITE);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPATK], DEFAULT_STAT_STAGE);
    }
}
