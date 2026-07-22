#include "global.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(gBattleMoves[MOVE_BATON_PASS].effect == EFFECT_BATON_PASS);
    ASSUME(gBattleMoves[MOVE_SWORDS_DANCE].effect == EFFECT_ATTACK_UP_2);
    ASSUME(gBattleMoves[MOVE_SUBSTITUTE].effect == EFFECT_SUBSTITUTE);
}

SINGLE_BATTLE_TEST("Wandering Hunter uses Baton Pass after its first KO and transfers effects")
{
    GIVEN {
        PLAYER(SPECIES_DECIDUEYE_HISUIAN) { Speed(100); MaxHP(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_WANDERING_HUNTER); Moves(MOVE_SWORDS_DANCE, MOVE_SUBSTITUTE, MOVE_TACKLE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_SWORDS_DANCE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_SUBSTITUTE); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(player, 1); SEND_OUT(opponent, 1); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WANDERING_HUNTER);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_BATON_PASS, player);
    } THEN {
        EXPECT_EQ(player->statStages[STAT_ATK], DEFAULT_STAT_STAGE + 2);
        EXPECT(player->status2 & STATUS2_SUBSTITUTE);
    }
}

SINGLE_BATTLE_TEST("Wandering Hunter activates only once per battle")
{
    GIVEN {
        PLAYER(SPECIES_DECIDUEYE_HISUIAN) { Speed(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_WANDERING_HUNTER); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
        PLAYER(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(player, 1); SEND_OUT(opponent, 1); }
        TURN { SWITCH(player, 0); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 2); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_WANDERING_HUNTER);
        NOT ABILITY_POPUP(player, ABILITY_WANDERING_HUNTER);
    }
}

SINGLE_BATTLE_TEST("Wandering Hunter does not activate without an ally to receive Baton Pass")
{
    GIVEN {
        PLAYER(SPECIES_DECIDUEYE_HISUIAN) { Speed(100); Ability(ABILITY_OVERGROW); UniqueAbility(ABILITY_WANDERING_HUNTER); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WYNAUT) { Speed(1); HP(1); Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(1); Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); SEND_OUT(opponent, 1); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_WANDERING_HUNTER);
    }
}
