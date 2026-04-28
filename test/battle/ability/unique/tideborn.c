#include "global.h"
#include "pokemon.h"
#include "test/battle.h"

ASSUMPTIONS
{
    ASSUME(IS_MOVE_STATUS(MOVE_GROWL));
    ASSUME(IS_MOVE_STATUS(MOVE_TAIL_WHIP));
    ASSUME(!IS_MOVE_STATUS(MOVE_TACKLE));
}

SINGLE_BATTLE_TEST("Tideborn heals 1/4 max HP and cures status after the first status move each battle")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_TIDEBORN); Status1(STATUS1_BURN); HP(200); MaxHP(400); Moves(MOVE_GROWL); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ABILITY_POPUP(player, ABILITY_TIDEBORN);
        MESSAGE("Phione's Tideborn restored its HP a little!");
        HP_BAR(player, damage: -100);
        MESSAGE("Phione's status returned to normal!");
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Tideborn only triggers once per battle")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_TIDEBORN); Status1(STATUS1_BURN); HP(200); MaxHP(400); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_CELEBRATE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        ABILITY_POPUP(player, ABILITY_TIDEBORN);
        MESSAGE("Phione's Tideborn restored its HP a little!");
        HP_BAR(player, damage: -100);
        MESSAGE("Phione's status returned to normal!");
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        NOT ABILITY_POPUP(player, ABILITY_TIDEBORN);
    } THEN {
        EXPECT_EQ(player->hp, 300);
        EXPECT_EQ(player->status1, STATUS1_NONE);
    }
}

SINGLE_BATTLE_TEST("Tideborn does not trigger after a damaging move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_TIDEBORN); HP(200); MaxHP(400); Moves(MOVE_TACKLE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_TACKLE); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        NOT ABILITY_POPUP(player, ABILITY_TIDEBORN);
    } THEN {
        EXPECT_EQ(player->hp, 200);
    }
}

SINGLE_BATTLE_TEST("Tideborn does not consume its once per battle trigger on a failed status move")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Speed(100); Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_TIDEBORN); HP(200); MaxHP(400); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Moves(MOVE_PROTECT, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(opponent, MOVE_PROTECT); MOVE(player, MOVE_GROWL); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_PROTECT, opponent);
        NOT ABILITY_POPUP(player, ABILITY_TIDEBORN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ABILITY_POPUP(player, ABILITY_TIDEBORN);
        MESSAGE("Phione's Tideborn restored its HP a little!");
        HP_BAR(player);
    } THEN {
        EXPECT_EQ(player->hp, 300);
    }
}

SINGLE_BATTLE_TEST("Tideborn does not consume its once per battle trigger when the user has no HP or status to restore")
{
    GIVEN {
        PLAYER(SPECIES_PHIONE) { Speed(100); Ability(ABILITY_HYDRATION); UniqueAbility(ABILITY_TIDEBORN); HP(400); MaxHP(400); Moves(MOVE_GROWL, MOVE_TAIL_WHIP); }
        OPPONENT(SPECIES_WOBBUFFET) { Speed(50); Attack(200); Moves(MOVE_TACKLE, MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_GROWL); MOVE(opponent, MOVE_TACKLE); }
        TURN { MOVE(player, MOVE_TAIL_WHIP); MOVE(opponent, MOVE_CELEBRATE); }
    } SCENE {
        ANIMATION(ANIM_TYPE_MOVE, MOVE_GROWL, player);
        NOT ABILITY_POPUP(player, ABILITY_TIDEBORN);
        ANIMATION(ANIM_TYPE_MOVE, MOVE_TAIL_WHIP, player);
        ABILITY_POPUP(player, ABILITY_TIDEBORN);
        MESSAGE("Phione's Tideborn restored its HP a little!");
    } THEN {
        EXPECT_EQ(player->hp, player->maxHP);
    }
}

SINGLE_BATTLE_TEST("Tideborn is assigned to Phione")
{
    GIVEN {
        PLAYER(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
        OPPONENT(SPECIES_WOBBUFFET) { Moves(MOVE_CELEBRATE); }
    } WHEN {
        TURN { MOVE(player, MOVE_CELEBRATE); MOVE(opponent, MOVE_CELEBRATE); }
    } THEN {
        EXPECT_EQ(GetUniqueAbilityBySpecies(SPECIES_PHIONE), ABILITY_TIDEBORN);
    }
}
