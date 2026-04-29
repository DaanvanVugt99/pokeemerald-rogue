#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_GRASSY_TERRAIN].effect == EFFECT_GRASSY_TERRAIN);
}

SINGLE_BATTLE_TEST("Root Network heals the incoming Pokemon by one sixth when the user switches out")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_SERPERIOR) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_ROOT_NETWORK); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(60); MaxHP(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROOT_NETWORK);
        MESSAGE("Wobbuffet restored HP through Root Network!");
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -20);
        EXPECT_EQ(player->hp, 80);
    }
}

SINGLE_BATTLE_TEST("Root Network heals the incoming Pokemon by one third in Grassy Terrain")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_SERPERIOR) { Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_ROOT_NETWORK); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(60); MaxHP(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_GRASSY_TERRAIN, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_GRASSY_TERRAIN); }
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROOT_NETWORK);
        MESSAGE("Wobbuffet restored HP through Root Network!");
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -40);
        EXPECT_EQ(player->hp, 107);
    }
}

SINGLE_BATTLE_TEST("Root Network does not heal if the user faints instead of switching out")
{
    GIVEN {
        PLAYER(SPECIES_SERPERIOR) { HP(1); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_ROOT_NETWORK); Moves(MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { HP(60); MaxHP(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(100); Moves(MOVE_QUICK_ATTACK); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_QUICK_ATTACK); SEND_OUT(player, 1); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ROOT_NETWORK);
            MESSAGE("Wobbuffet restored HP through Root Network!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 60);
    }
}

SINGLE_BATTLE_TEST("Root Network is the Serperior line's unique ability")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SNIVY), ABILITY_ROOT_NETWORK);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SERVINE), ABILITY_ROOT_NETWORK);
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_SERPERIOR), ABILITY_ROOT_NETWORK);
    }
}
