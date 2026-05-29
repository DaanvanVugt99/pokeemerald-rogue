#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_TACKLE].makesContact);
    ASSUME(!gBattleMoves[MOVE_WATER_GUN].makesContact);
}

SINGLE_BATTLE_TEST("Static Charge gives +1 Speed on contact and only damages non-Ground attackers")
{
    u16 attackerSpecies;
    bool8 shouldRecoil;
    PARAMETRIZE { attackerSpecies = SPECIES_WOBBUFFET; shouldRecoil = TRUE; }
    PARAMETRIZE { attackerSpecies = SPECIES_SANDSLASH; shouldRecoil = FALSE; }
    GIVEN {
        PLAYER(SPECIES_RAICHU) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_CHARGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(attackerSpecies) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE + 1);
        EXPECT_EQ(opponent->hp < opponent->maxHP, shouldRecoil);
    }
}

SINGLE_BATTLE_TEST("Static Charge does not trigger on non-contact moves")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_CHARGE); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_WATER_GUN); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_WATER_GUN); }
    } THEN {
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}

SINGLE_BATTLE_TEST("Static Charge does not trigger if the holder faints from the contact move")
{
    GIVEN {
        PLAYER(SPECIES_RAICHU) { Ability(ABILITY_STATIC); UniqueAbility(ABILITY_STATIC_CHARGE); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_TACKLE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_TACKLE); }
    } THEN {
        EXPECT_EQ(player->hp, 0);
        EXPECT_EQ(player->statStages[STAT_SPEED], DEFAULT_STAT_STAGE);
        EXPECT_EQ(opponent->hp, opponent->maxHP);
    }
}
