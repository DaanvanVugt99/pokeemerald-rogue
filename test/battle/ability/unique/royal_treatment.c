#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_PARTING_SHOT].effect == EFFECT_PARTING_SHOT);
}

SINGLE_BATTLE_TEST("Royal Treatment heals the incoming Pokemon by one quarter after Parting Shot")
{
    s16 healed;

    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLAN) { Speed(100); Ability(ABILITY_FUR_COAT); Moves(MOVE_PARTING_SHOT); }
        PLAYER(SPECIES_WOBBUFFET) { HP(60); MaxHP(120); Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_PARTING_SHOT); SEND_OUT(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_ROYAL_TREATMENT);
        MESSAGE("Wobbuffet restored HP through Royal Treatment!");
        HP_BAR(player, captureDamage: &healed);
    } THEN {
        EXPECT_EQ(healed, -30);
        EXPECT_EQ(player->hp, 90);
    }
}

SINGLE_BATTLE_TEST("Royal Treatment does not heal after a regular switch")
{
    GIVEN {
        PLAYER(SPECIES_PERSIAN_ALOLAN) { Ability(ABILITY_FUR_COAT); Moves(MOVE_PARTING_SHOT); }
        PLAYER(SPECIES_WOBBUFFET) { HP(60); MaxHP(120); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { SWITCH(player, 1); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NONE_OF {
            ABILITY_POPUP(player, ABILITY_ROYAL_TREATMENT);
            MESSAGE("Wobbuffet restored HP through Royal Treatment!");
        }
    } THEN {
        EXPECT_EQ(player->hp, 60);
    }
}
