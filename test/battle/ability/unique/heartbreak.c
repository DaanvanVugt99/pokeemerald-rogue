#include "global.h"
#include "test/battle.h"

SINGLE_BATTLE_TEST("Heartbreak infatuates with damaging moves regardless of gender")
{
    GIVEN {
        PLAYER(SPECIES_DELCATTY) { Gender(MON_MALE); Ability(ABILITY_NORMALIZE); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { Gender(MON_MALE); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_HEARTBREAK);
    } THEN {
        EXPECT(opponent->status2 & STATUS2_INFATUATION);
    }
}

SINGLE_BATTLE_TEST("Heartbreak respects Oblivious")
{
    GIVEN {
        PLAYER(SPECIES_DELCATTY) { Ability(ABILITY_NORMALIZE); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_SLOWPOKE) { Ability(ABILITY_OBLIVIOUS); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SCRATCH); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEARTBREAK);
        }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_INFATUATION));
    }
}

SINGLE_BATTLE_TEST("Heartbreak drains 1/8 HP from infatuated targets at end of turn")
{
    GIVEN {
        PLAYER(SPECIES_DELCATTY) { Ability(ABILITY_NORMALIZE); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); MaxHP(160); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, 430);
    }
}

SINGLE_BATTLE_TEST("Heartbreak does not drain Magic Guard targets")
{
    GIVEN {
        PLAYER(SPECIES_DELCATTY) { Ability(ABILITY_NORMALIZE); Moves(MOVE_DRAGON_RAGE); }
        OPPONENT(SPECIES_CLEFABLE) { Ability(ABILITY_MAGIC_GUARD); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_DRAGON_RAGE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(opponent->hp, opponent->maxHP - 40);
    }
}

SINGLE_BATTLE_TEST("Heartbreak does not infatuate through Substitute")
{
    GIVEN {
        PLAYER(SPECIES_DELCATTY) { Ability(ABILITY_NORMALIZE); Moves(MOVE_SCRATCH); }
        OPPONENT(SPECIES_WOBBUFFET) { HP(200); Moves(MOVE_SUBSTITUTE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_SUBSTITUTE); MOVE(player, MOVE_SCRATCH); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_HEARTBREAK);
        }
    } THEN {
        EXPECT(!(opponent->status2 & STATUS2_INFATUATION));
    }
}
