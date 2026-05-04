#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_WHIRLPOOL].effect == EFFECT_TRAP);
}

SINGLE_BATTLE_TEST("Abyss uses Whirlpool on switch-in and traps if all Pokémon on the team share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WAILMER) { Ability(ABILITY_OBLIVIOUS); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KYOGRE) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_ABYSS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(opponent, ABILITY_ABYSS);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_WHIRLPOOL, opponent);
    } THEN {
        EXPECT_NE(player->status2 & STATUS2_WRAPPED, 0);
    }
}

SINGLE_BATTLE_TEST("Abyss does not activate on switch-in if a teammate does not share a type with the user")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Ability(ABILITY_NO_GUARD); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_PIKACHU) { Ability(ABILITY_STATIC); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_KYOGRE) { Ability(ABILITY_NO_GUARD); UniqueAbility(ABILITY_ABYSS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(opponent, 1); MOVE(player, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(player->status2 & STATUS2_WRAPPED, 0);
    }
}
